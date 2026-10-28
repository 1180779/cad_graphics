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

std::size_t componentSize(const GLenum type) {
    switch (type) {
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
        return 1;
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
    case GL_HALF_FLOAT:
        return 2;
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_FLOAT:
    case GL_FIXED:
        return 4;
    default:
        return 0;
    }
}

std::expected<void, std::string> validateMeshView(
    const MeshView& mesh,
    const GLint maxAttributes
) {
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

    for (std::size_t i = 0; i < mesh.layout.attributes.size(); ++i) {
        const VertexAttribute& attribute = mesh.layout.attributes[i];

        if (attribute.location >= static_cast<GLuint>(maxAttributes)) {
            return std::unexpected("vertex attribute location is out of range");
        }
        if (attribute.components < 1 || attribute.components > 4) {
            return std::unexpected("vertex attribute must have between 1 and 4 components");
        }

        const std::size_t typeSize = componentSize(attribute.type);
        if (typeSize == 0) {
            return std::unexpected("unsupported vertex attribute type");
        }

        const std::size_t attributeBytes = typeSize * static_cast<std::size_t>(attribute.components);
        if (attribute.offset > mesh.layout.stride
            || attributeBytes > mesh.layout.stride - attribute.offset) {
            return std::unexpected("vertex attribute exceeds the vertex stride");
        }

        for (std::size_t j = 0; j < i; ++j) {
            if (mesh.layout.attributes[j].location == attribute.location) {
                return std::unexpected("duplicate vertex attribute location");
            }
        }
    }

    return {};
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
    return upload(gl, viewOf(mesh));
}

std::expected<GpuMesh, std::string> GpuMesh::upload(Gl& gl, const MeshView& mesh) {
    GLint maxAttributes = 0;
    gl.glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttributes);
    if (maxAttributes <= 0) {
        return std::unexpected("could not query GL_MAX_VERTEX_ATTRIBS");
    }

    const auto describe = [&mesh](std::string message) {
        return mesh.name.empty()
                   ? message
                   : std::string(mesh.name) + ": " + message;
    };

    if (auto valid = validateMeshView(mesh, maxAttributes); !valid) {
        return std::unexpected(describe(valid.error()));
    }

    clearGlErrors(gl);

    GpuMesh result;
    gl.glGenVertexArrays(1, &result.m_vao);
    gl.glGenBuffers(1, &result.m_vbo);
    gl.glGenBuffers(1, &result.m_ebo);

    if (result.m_vao == 0 || result.m_vbo == 0 || result.m_ebo == 0) {
        releaseHandles(gl, result.m_vao, result.m_vbo, result.m_ebo);
        return std::unexpected(describe("failed to allocate GL mesh handles"));
    }

    gl.glBindVertexArray(result.m_vao);

    gl.glBindBuffer(GL_ARRAY_BUFFER, result.m_vbo);
    gl.glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(mesh.vertexCount * mesh.layout.stride),
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

    const auto stride = static_cast<GLsizei>(mesh.layout.stride);
    for (const VertexAttribute& attribute : mesh.layout.attributes) {
        gl.glEnableVertexAttribArray(attribute.location);
        gl.glVertexAttribPointer(
            attribute.location,
            attribute.components,
            attribute.type,
            attribute.normalized,
            stride,
            reinterpret_cast<const void*>(static_cast<std::uintptr_t>(attribute.offset))
        );
    }

    gl.glBindVertexArray(0);
    gl.glBindBuffer(GL_ARRAY_BUFFER, 0);
    gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    if (const GLenum error = gl.glGetError(); error != GL_NO_ERROR) {
        releaseHandles(gl, result.m_vao, result.m_vbo, result.m_ebo);
        return std::unexpected(describe(
            "OpenGL error while uploading mesh (code " + std::to_string(error) + ")"
        ));
    }

    result.m_indexCount = static_cast<GLsizei>(mesh.indices.size());
    return result;
}

void GpuMesh::destroy(Gl& gl) {
    releaseHandles(gl, m_vao, m_vbo, m_ebo);
    m_indexCount = 0;
}

} // namespace cadg