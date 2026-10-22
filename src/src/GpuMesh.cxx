/*
 * Created by Radosław Głasek on 22.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#include "cad_graphics/GpuMesh.hxx"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

namespace cadg {
namespace {

void releaseHandles(Gl& gl, GLuint& vao, GLuint& vbo, GLuint& ebo) {
    if (ebo != 0) {
        gl.glDeleteBuffers(1, &ebo);
        ebo = 0;
    }
    if (vbo != 0) {
        gl.glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
    if (vao != 0) {
        gl.glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
}

void clearGlErrors(Gl& gl) {
    while (gl.glGetError() != GL_NO_ERROR) {
    }
}

} // namespace

GpuMesh::~GpuMesh() {
    assert(m_vao == 0 && m_vbo == 0 && m_ebo == 0 &&
        "GpuMesh destroyed with live GL handles; call destroy() first");
}

GpuMesh::GpuMesh(GpuMesh&& other) noexcept
    : m_vao(std::exchange(other.m_vao, 0)),
      m_vbo(std::exchange(other.m_vbo, 0)),
      m_ebo(std::exchange(other.m_ebo, 0)),
      m_indexCount(std::exchange(other.m_indexCount, 0)) {
}

GpuMesh& GpuMesh::operator=(GpuMesh&& other) noexcept {
    if (this == &other) {
        return *this;
    }
    assert(m_vao == 0 && m_vbo == 0 && m_ebo == 0 &&
        "move-assigned over a live GpuMesh; call destroy() first");
    m_vao = std::exchange(other.m_vao, 0);
    m_vbo = std::exchange(other.m_vbo, 0);
    m_ebo = std::exchange(other.m_ebo, 0);
    m_indexCount = std::exchange(other.m_indexCount, 0);
    return *this;
}

std::expected<GpuMesh, std::string> GpuMesh::upload(Gl& gl, const MeshData& mesh) {
    if (mesh.vertices.empty() || mesh.indices.empty()) {
        return std::unexpected("cannot upload an empty mesh");
    }
    if (mesh.indices.size() > static_cast<std::size_t>(std::numeric_limits<GLsizei>::max())) {
        return std::unexpected("mesh has too many indices for a GLsizei draw call");
    }
    if (const auto maxIndex = *std::ranges::max_element(mesh.indices);
        maxIndex >= mesh.vertices.size()) {
        return std::unexpected("mesh index is out of range");
    }

    clearGlErrors(gl);

    GpuMesh result;
    gl.glGenVertexArrays(1, &result.m_vao);
    gl.glGenBuffers(1, &result.m_vbo);
    gl.glGenBuffers(1, &result.m_ebo);

    if (result.m_vao == 0 || result.m_vbo == 0 || result.m_ebo == 0) {
        releaseHandles(gl, result.m_vao, result.m_vbo, result.m_ebo);
        return std::unexpected("failed to allocate GL mesh handles");
    }

    gl.glBindVertexArray(result.m_vao);

    gl.glBindBuffer(GL_ARRAY_BUFFER, result.m_vbo);
    gl.glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(mesh.vertices.size() * sizeof(Vertex)),
        mesh.vertices.data(),
        GL_STATIC_DRAW
        );

    gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result.m_ebo);
    gl.glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(mesh.indices.size() * sizeof(std::uint32_t)),
        mesh.indices.data(),
        GL_STATIC_DRAW
        );

    constexpr auto stride = static_cast<GLsizei>(sizeof(Vertex));
    gl.glEnableVertexAttribArray(0);
    gl.glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        stride,
        reinterpret_cast<const void*>(static_cast<std::uintptr_t>(offsetof(Vertex, position)))
        );

    gl.glEnableVertexAttribArray(1);
    gl.glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        stride,
        reinterpret_cast<const void*>(static_cast<std::uintptr_t>(offsetof(Vertex, normal)))
        );

    gl.glEnableVertexAttribArray(2);
    gl.glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        stride,
        reinterpret_cast<const void*>(static_cast<std::uintptr_t>(offsetof(Vertex, texCoord)))
        );

    gl.glBindVertexArray(0);
    gl.glBindBuffer(GL_ARRAY_BUFFER, 0);
    gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    if (const GLenum error = gl.glGetError(); error != GL_NO_ERROR) {
        releaseHandles(gl, result.m_vao, result.m_vbo, result.m_ebo);
        return std::unexpected("OpenGL error while uploading mesh (code " + std::to_string(error) + ")");
    }

    result.m_indexCount = static_cast<GLsizei>(mesh.indices.size());
    return result;
}

void GpuMesh::draw(Gl& gl) const {
    if (m_vao == 0 || m_indexCount == 0) {
        return;
    }
    gl.glBindVertexArray(m_vao);
    gl.glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
    gl.glBindVertexArray(0);
}

void GpuMesh::destroy(Gl& gl) {
    releaseHandles(gl, m_vao, m_vbo, m_ebo);
    m_indexCount = 0;
}

} // namespace cadg