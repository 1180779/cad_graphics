/*
 * Created by Radosław Głasek on 22.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#ifndef CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_GPU_MODEL_HXX
#define CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_GPU_MODEL_HXX

#include <cstddef>
#include <expected>
#include <string>
#include <vector>

#include "Gl.hxx"
#include "GpuMaterial.hxx"
#include "GpuMesh.hxx"
#include "Model.hxx"

namespace cadg {

struct GpuPart final {
    std::size_t meshIndex = 0;
    std::size_t materialIndex = 0;
    cadm::Mat4 nodeTransform = cadm::Mat4::identity();
};

/// @brief GPU-side Model
///
/// Meshes are uploaded once per unique MeshData and shared by all parts that
/// reference the same mesh
/// 
/// The destructor never issues GL calls. it only asserts that the handles were
/// released
class GpuModel final {
public:
    GpuModel() = default;
    ~GpuModel();

    GpuModel(const GpuModel&) = delete;
    GpuModel& operator=(const GpuModel&) = delete;

    GpuModel(GpuModel&& other) noexcept;
    GpuModel& operator=(GpuModel&& other) noexcept;

    static std::expected<GpuModel, std::string> upload(Gl& gl, const Model& model);

    void destroy(Gl& gl);

    [[nodiscard]] bool valid() const {
        return !m_meshes.empty();
    }

    [[nodiscard]] const std::vector<GpuPart>& parts() const {
        return m_parts;
    }

    [[nodiscard]] const GpuMesh& mesh(std::size_t index) const {
        return m_meshes.at(index);
    }

    [[nodiscard]] const GpuMaterial& material(std::size_t index) const {
        return m_materials.at(index);
    }

private:
    std::vector<GpuMesh> m_meshes;
    std::vector<GpuMaterial> m_materials;
    std::vector<GpuPart> m_parts;
};

} // namespace cadg

#endif // CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_GPU_MODEL_HXX