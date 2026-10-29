/*
 * Created by Radosław Głasek on 22.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#include "cad_graphics/GpuMesh.hxx"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <limits>
#include <span>
#include <utility>
#include <vector>

namespace cadg {
namespace {

void releaseHandles(Gl& gl, VertexArray& vertexArray, GLuint& vbo, GLuint& ebo) {
    vertexArray.destroy(gl);
    if (ebo != 0) {
        gl.glDeleteBuffers(1, &ebo);
        ebo = 0;
    }
    if (vbo != 0) {
        gl.glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
}

void clearGlErrors(Gl& gl) {
    while (gl.glGetError() != GL_NO_ERROR) {
    }
}

constexpr std::array gc_vertexAttributes{
    VertexAttribute{
        .location = 0,
        .components = 3,
        .offset = offsetof(Vertex, position)
    },
    VertexAttribute{
        .location = 1,
        .components = 3,
        .offset = offsetof(Vertex, normal)
    },
    VertexAttribute{
        .location = 2,
        .components = 2,
        .offset = offsetof(Vertex, texCoord)
    },
};

MeshView viewOf(const MeshData& mesh) {
    return {
        .name = mesh.name,
        .vertices = std::as_bytes(std::span(mesh.vertices)),
        .vertexCount = mesh.vertices.size(),
        .indices = std::span(mesh.indices),
        .layout = {
            .stride = sizeof(Vertex),
            .attributes = gc_vertexAttributes,
        },
    };
}

std::expected<void, std::string> validateMeshView(const MeshView& mesh) {
    if (mesh.vertexCount == 0 || mesh.vertices.empty() || mesh.indices.empty()) {
        return std::unexpected("cannot upload an empty mesh");
    }
    if (mesh.layout.stride == 0) {
        return std::unexpected("vertex stride must be non-zero");
    }
    if (mesh.layout.stride > static_cast<std::size_t>(std::numeric_limits<GLsizei>::max())) {
        return std::unexpected("vertex stride is too large for a GLsizei draw call");
    }
    if (mesh.layout.attributes.empty()) {
        return std::unexpected("mesh has no vertex attributes");
    }
    if (mesh.vertexCount > std::numeric_limits<std::size_t>::max() / mesh.layout.stride) {
        return std::unexpected("vertex buffer size overflow");
    }

    const std::size_t requiredBytes = mesh.vertexCount * mesh.layout.stride;
    if (mesh.vertices.size() < requiredBytes) {
        return std::unexpected("vertex buffer is smaller than vertexCount * stride");
    }
    if (requiredBytes > static_cast<std::size_t>(std::numeric_limits<GLsizeiptr>::max())) {
        return std::unexpected("vertex buffer is too large for a GLsizeiptr upload");
    }
    if (mesh.indices.size() > static_cast<std::size_t>(std::numeric_limits<GLsizei>::max())) {
        return std::unexpected("mesh has too many indices for a GLsizei draw call");
    }
    if (const auto maxIndex = *std::ranges::max_element(mesh.indices);
        maxIndex >= mesh.vertexCount) {
        return std::unexpected("mesh index is out of range");
    }

    return {};
}

} // namespace

GpuMesh::~GpuMesh() {
    assert(!m_vertexArray.valid() && m_vbo == 0 && m_ebo == 0 &&
        "GpuMesh destroyed with live GL handles; call destroy() first");
}

GpuMesh::GpuMesh(GpuMesh&& other) noexcept
    : m_vertexArray(std::move(other.m_vertexArray)),
      m_vbo(std::exchange(other.m_vbo, 0)),
      m_ebo(std::exchange(other.m_ebo, 0)),
      m_indexCount(std::exchange(other.m_indexCount, 0)) {
}

GpuMesh& GpuMesh::operator=(GpuMesh&& other) noexcept {
    if (this == &other) {
        return *this;
    }
    assert(!m_vertexArray.valid() && m_vbo == 0 && m_ebo == 0 &&
        "move-assigned over a live GpuMesh; call destroy() first");
    m_vertexArray = std::move(other.m_vertexArray);
    m_vbo = std::exchange(other.m_vbo, 0);
    m_ebo = std::exchange(other.m_ebo, 0);
    m_indexCount = std::exchange(other.m_indexCount, 0);
    return *this;
}

std::expected<GpuMesh, std::string> GpuMesh::upload(Gl& gl, const MeshData& mesh) {
    return upload(gl, viewOf(mesh));
}

std::expected<GpuMesh, std::string> GpuMesh::upload(Gl& gl, const MeshView& mesh) {
    const auto describe = [&mesh](std::string message) {
        return mesh.name.empty()
                   ? message
                   : std::string(mesh.name) + ": " + message;
    };

    if (auto valid = validateMeshView(mesh); !valid) {
        return std::unexpected(describe(valid.error()));
    }

    clearGlErrors(gl);

    GpuMesh result;
    gl.glCreateBuffers(1, &result.m_vbo);
    gl.glCreateBuffers(1, &result.m_ebo);

    if (result.m_vbo == 0 || result.m_ebo == 0) {
        releaseHandles(gl, result.m_vertexArray, result.m_vbo, result.m_ebo);
        return std::unexpected(describe("failed to allocate GL mesh handles"));
    }

    auto vertexArray = VertexArray::create(gl, result.m_vbo, mesh.layout, result.m_ebo);
    if (!vertexArray) {
        releaseHandles(gl, result.m_vertexArray, result.m_vbo, result.m_ebo);
        return std::unexpected(describe(std::move(vertexArray.error())));
    }
    result.m_vertexArray = std::move(*vertexArray);
    gl.glNamedBufferData(
        result.m_vbo,
        static_cast<GLsizeiptr>(mesh.vertexCount * mesh.layout.stride),
        mesh.vertices.data(),
        GL_STATIC_DRAW
    );
    gl.glNamedBufferData(
        result.m_ebo,
        static_cast<GLsizeiptr>(mesh.indices.size() * sizeof(std::uint32_t)),
        mesh.indices.data(),
        GL_STATIC_DRAW
    );

    if (const GLenum error = gl.glGetError(); error != GL_NO_ERROR) {
        releaseHandles(gl, result.m_vertexArray, result.m_vbo, result.m_ebo);
        return std::unexpected(describe(
            "OpenGL error while uploading mesh (code " + std::to_string(error) + ")"
        ));
    }

    result.m_indexCount = static_cast<GLsizei>(mesh.indices.size());
    return result;
}

void GpuMesh::destroy(Gl& gl) {
    releaseHandles(gl, m_vertexArray, m_vbo, m_ebo);
    m_indexCount = 0;
}

} // namespace cadg