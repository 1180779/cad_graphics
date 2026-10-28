/*
 * Created by Radosław Głasek on 28.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#include "cad_graphics/ShaderProgramGlBoundBorrow.hxx"

namespace cadg {

void ShaderProgramGlBoundBorrow::bind() const {
    m_borrowedProgram.bind(m_gl);
}

void ShaderProgramGlBoundBorrow::unbind() const {
    ShaderProgram::unbind(m_gl);
}

GLint ShaderProgramGlBoundBorrow::location(const std::string_view name) const {
    return m_borrowedProgram.location(m_gl, name);
}

bool ShaderProgramGlBoundBorrow::setInt(const std::string_view name, const int value) const {
    return m_borrowedProgram.setInt(m_gl, name, value);
}

bool ShaderProgramGlBoundBorrow::setFloat(const std::string_view name, const float value) const {
    return m_borrowedProgram.setFloat(m_gl, name, value);
}

bool ShaderProgramGlBoundBorrow::setVec2(const std::string_view name, const float x, const float y) const {
    return m_borrowedProgram.setVec2(m_gl, name, x, y);
}

bool ShaderProgramGlBoundBorrow::setVec2(const std::string_view name, const cadm::Vec2& value) const {
    return m_borrowedProgram.setVec2(m_gl, name, value);
}

bool ShaderProgramGlBoundBorrow::setVec3(const std::string_view name, const float x, const float y,
                                                const float z) const {
    return m_borrowedProgram.setVec3(m_gl, name, x, y, z);
}

bool ShaderProgramGlBoundBorrow::setVec3(const std::string_view name, const cadm::Vec3& value) const {
    return m_borrowedProgram.setVec3(m_gl, name, value);
}

bool ShaderProgramGlBoundBorrow::setVec4(const std::string_view name, const float x, const float y,
                                                const float z, const float w) const {
    return m_borrowedProgram.setVec4(m_gl, name, x, y, z, w);
}

bool ShaderProgramGlBoundBorrow::setVec4(const std::string_view name, const cadm::Vec4& value) const {
    return m_borrowedProgram.setVec4(m_gl, name, value);
}

bool ShaderProgramGlBoundBorrow::setMat4(const std::string_view name, const float* columnMajor) const {
    return m_borrowedProgram.setMat4(m_gl, name, columnMajor);
}

bool ShaderProgramGlBoundBorrow::setMat4(const std::string_view name, const cadm::Mat4& value) const {
    return m_borrowedProgram.setMat4(m_gl, name, value);
}

bool ShaderProgramGlBoundBorrow::setMat3(const std::string_view name, const float* columnMajor) const {
    return m_borrowedProgram.setMat3(m_gl, name, columnMajor);
}

bool ShaderProgramGlBoundBorrow::setMat3(const std::string_view name, const cadm::Mat3& value) const {
    return m_borrowedProgram.setMat3(m_gl, name, value);
}

ShaderProgramGlBoundBorrow ShaderProgram::borrowWithGlBound(Gl& gl) const {
    return ShaderProgramGlBoundBorrow{*this, gl};
}

} // namespace cadg