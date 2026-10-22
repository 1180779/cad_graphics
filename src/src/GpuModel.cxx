/*
 * Created by Radosław Głasek on 22.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#include "cad_graphics/GpuModel.hxx"

#include <cassert>
#include <unordered_map>
#include <utility>

namespace cadg {

GpuModel::~GpuModel() {
    assert(m_meshes.empty() && m_materials.empty() &&
        "GpuModel destroyed with live resources; call destroy() first");
}

GpuModel::GpuModel(GpuModel&& other) noexcept
    : m_meshes(std::move(other.m_meshes)),
      m_materials(std::move(other.m_materials)),
      m_parts(std::move(other.m_parts)) {
    other.m_meshes.clear();
    other.m_materials.clear();
    other.m_parts.clear();
}

GpuModel& GpuModel::operator=(GpuModel&& other) noexcept {
    if (this == &other) {
        return *this;
    }
    assert(m_meshes.empty() && m_materials.empty() &&
        "move-assigned over a live GpuModel; call destroy() first");
    m_meshes = std::move(other.m_meshes);
    m_materials = std::move(other.m_materials);
    m_parts = std::move(other.m_parts);
    other.m_meshes.clear();
    other.m_materials.clear();
    other.m_parts.clear();
    return *this;
}

std::expected<GpuModel, std::string> GpuModel::upload(Gl& gl, const Model& model) {
    if (model.parts.empty() || model.materials.empty()) {
        return std::unexpected("cannot upload a model with no parts or materials");
    }

    GpuModel result;
    std::unordered_map<const MeshData*, std::size_t> meshIndices;

    for (const auto& part : model.parts) {
        if (!part.mesh) {
            continue;
        }
        if (meshIndices.contains(part.mesh.get())) {
            continue;
        }

        auto mesh = GpuMesh::upload(gl, *part.mesh);
        if (!mesh) {
            result.destroy(gl);
            return std::unexpected("failed to upload mesh '" + part.mesh->name + "': " + mesh.error());
        }
        meshIndices.emplace(part.mesh.get(), result.m_meshes.size());
        result.m_meshes.push_back(std::move(*mesh));
    }

    if (result.m_meshes.empty()) {
        return std::unexpected("model has no drawable meshes");
    }

    result.m_materials.reserve(model.materials.size());
    for (const auto& material : model.materials) {
        auto uploaded = GpuMaterial::upload(gl, material);
        if (!uploaded) {
            result.destroy(gl);
            return std::unexpected("failed to upload material '" + material.name + "': " + uploaded.error());
        }
        result.m_materials.push_back(std::move(*uploaded));
    }

    result.m_parts.reserve(model.parts.size());
    for (const auto& part : model.parts) {
        if (!part.mesh) {
            continue;
        }
        const auto meshIt = meshIndices.find(part.mesh.get());
        if (meshIt == meshIndices.end()) {
            continue;
        }
        const std::size_t materialIndex = part.materialIndex < result.m_materials.size()
                                              ? part.materialIndex
                                              : 0;
        result.m_parts.push_back(GpuPart{
            .meshIndex = meshIt->second,
            .materialIndex = materialIndex,
            .nodeTransform = part.nodeTransform,
        });
    }

    if (result.m_parts.empty()) {
        result.destroy(gl);
        return std::unexpected("model has no drawable parts");
    }

    return result;
}

void GpuModel::destroy(Gl& gl) {
    for (auto& material : m_materials) {
        material.destroy(gl);
    }
    m_materials.clear();

    for (auto& mesh : m_meshes) {
        mesh.destroy(gl);
    }
    m_meshes.clear();

    m_parts.clear();
}

} // namespace cadg