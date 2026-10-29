/*
 * Created by Radosław Głasek on 29.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#include "cad_graphics/VertexArray.hxx"

#include <cassert>
#include <cstdint>
#include <limits>
#include <utility>

namespace cadg {
namespace {

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
    case GL_DOUBLE:
        return 8;
    default:
        return 0;
    }
}

std::expected<void, std::string> validateLayout(Gl& gl, const VertexLayout& layout) {
    if (layout.stride == 0 || layout.stride > static_cast<std::size_t>(std::numeric_limits<GLsizei>::max())) {
        return std::unexpected("vertex stride must be non-zero and fit in a GLsizei");
    }
    if (layout.attributes.empty()) {
        return std::unexpected("vertex layout has no attributes");
    }
    GLint maxAttributes = 0;
    GLint maxStride = 0;
    gl.glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttributes);
    gl.glGetIntegerv(GL_MAX_VERTEX_ATTRIB_STRIDE, &maxStride);
    if (maxAttributes <= 0 || maxStride <= 0) {
        return std::unexpected("could not query vertex attribute limits");
    }
    if (layout.stride > static_cast<std::size_t>(maxStride)) {
        return std::unexpected("vertex stride exceeds GL_MAX_VERTEX_ATTRIB_STRIDE");
    }
    for (std::size_t i = 0; i < layout.attributes.size(); ++i) {
        const auto& attribute = layout.attributes[i];
        if (attribute.location >= static_cast<GLuint>(maxAttributes)) {
            return std::unexpected("vertex attribute location is out of range");
        }
        if (attribute.components < 1 || attribute.components > 4) {
            return std::unexpected("vertex attribute must have between 1 and 4 components");
        }
        const auto typeSize = componentSize(attribute.type);
        if (typeSize == 0) {
            return std::unexpected("unsupported vertex attribute type");
        }
        if (const auto attributeBytes = typeSize * static_cast<std::size_t>(attribute.components);
            attribute.offset > layout.stride || attributeBytes > layout.stride - attribute.offset) {
            return std::unexpected("vertex attribute exceeds the vertex stride");
        }
        for (std::size_t j = 0; j < i; ++j) {
            if (layout.attributes[j].location == attribute.location) {
                return std::unexpected("duplicate vertex attribute location");
            }
        }
    }
    return {};
}

} // namespace

VertexArray::~VertexArray() {
    assert(m_vao == 0 && "VertexArray destroyed with a live handle; call destroy() first");
}

VertexArray::VertexArray(VertexArray&& other) noexcept
    : m_vao(std::exchange(other.m_vao, 0)) {
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept {
    if (this != &other) {
        assert(m_vao == 0 && "move-assigned over a live VertexArray; call destroy() first");
        m_vao = std::exchange(other.m_vao, 0);
    }
    return *this;
}

std::expected<VertexArray, std::string> VertexArray::create(
    Gl& gl, const GLuint vertexBuffer, const VertexLayout& layout, const GLuint indexBuffer
) {
    if (!gl.glIsBuffer(vertexBuffer)) {
        return std::unexpected("vertex buffer must refer to an existing buffer object");
    }
    if (indexBuffer != 0 && !gl.glIsBuffer(indexBuffer)) {
        return std::unexpected("index buffer must refer to an existing buffer object");
    }
    if (auto valid = validateLayout(gl, layout); !valid) {
        return std::unexpected(std::move(valid.error()));
    }

    VertexArray result;
    gl.glGenVertexArrays(1, &result.m_vao);
    if (result.m_vao == 0) {
        return std::unexpected("failed to allocate vertex array");
    }
    gl.glBindVertexArray(result.m_vao);
    gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    for (const auto& [location, components, type, normalized, offset] : layout.attributes) {
        gl.glEnableVertexAttribArray(location);
        gl.glVertexAttribPointer(
            location, components, type, normalized,
            static_cast<GLsizei>(layout.stride),
            reinterpret_cast<const void*>(static_cast<std::uintptr_t>(offset))
        );
    }
    gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    if (const GLenum error = gl.glGetError(); error != GL_NO_ERROR) {
        result.destroy(gl);
        return std::unexpected("OpenGL error while configuring vertex array (code " + std::to_string(error) + ")");
    }
    return result;
}

std::expected<VertexArray, std::string> VertexArray::create(Gl* gl, const GLuint vertexBuffer,
                                                            const VertexLayout& layout,
                                                            const GLuint indexBuffer) {
    return create(*gl, vertexBuffer, layout, indexBuffer);
}

void VertexArray::bind(Gl& gl) const {
    gl.glBindVertexArray(m_vao);
}

void VertexArray::bind(Gl* gl) const {
    gl->glBindVertexArray(m_vao);
}

void VertexArray::unbind(Gl& gl) {
    gl.glBindVertexArray(0);
}

void VertexArray::unbind(Gl* gl) {
    gl->glBindVertexArray(0);
}

void VertexArray::destroy(Gl& gl) {
    if (m_vao != 0) {
        gl.glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
}

void VertexArray::destroy(Gl* gl) {
    if (m_vao != 0) {
        gl->glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
}

} // namespace cadg