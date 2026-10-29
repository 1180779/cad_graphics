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

bool ShaderProgramGlBoundBorrow::uniform1i(const std::string_view name, const int value) const {
    return m_borrowedProgram.uniform1i(m_gl, name, value);
}

bool ShaderProgramGlBoundBorrow::uniform1f(const std::string_view name, const float value) const {
    return m_borrowedProgram.uniform1f(m_gl, name, value);
}

bool ShaderProgramGlBoundBorrow::uniform2f(const std::string_view name, const float x, const float y) const {
    return m_borrowedProgram.uniform2f(m_gl, name, x, y);
}

bool ShaderProgramGlBoundBorrow::uniform2f(const std::string_view name, const Vec2f& value) const {
    return m_borrowedProgram.uniform2f(m_gl, name, value);
}

bool ShaderProgramGlBoundBorrow::uniform3f(const std::string_view name, const float x, const float y,
                                                const float z) const {
    return m_borrowedProgram.uniform3f(m_gl, name, x, y, z);
}

bool ShaderProgramGlBoundBorrow::uniform3f(const std::string_view name, const Vec3f& value) const {
    return m_borrowedProgram.uniform3f(m_gl, name, value);
}

bool ShaderProgramGlBoundBorrow::uniform4f(const std::string_view name, const float x, const float y,
                                                const float z, const float w) const {
    return m_borrowedProgram.uniform4f(m_gl, name, x, y, z, w);
}

bool ShaderProgramGlBoundBorrow::uniform4f(const std::string_view name, const Vec4f& value) const {
    return m_borrowedProgram.uniform4f(m_gl, name, value);
}

bool ShaderProgramGlBoundBorrow::uniformMatrix4fv(const std::string_view name, const float* columnMajor) const {
    return m_borrowedProgram.uniformMatrix4fv(m_gl, name, columnMajor);
}

bool ShaderProgramGlBoundBorrow::uniformMatrix4f(const std::string_view name, const Mat4f& value) const {
    return m_borrowedProgram.uniformMatrix4f(m_gl, name, value);
}

bool ShaderProgramGlBoundBorrow::uniformMatrix3fv(const std::string_view name, const float* columnMajor) const {
    return m_borrowedProgram.uniformMatrix3fv(m_gl, name, columnMajor);
}

bool ShaderProgramGlBoundBorrow::uniformMatrix3f(const std::string_view name, const Mat3f& value) const {
    return m_borrowedProgram.uniformMatrix3f(m_gl, name, value);
}

ShaderProgramGlBoundBorrow ShaderProgram::borrowWithGlBound(Gl& gl) const {
    return ShaderProgramGlBoundBorrow{*this, gl};
}

} // namespace cadg