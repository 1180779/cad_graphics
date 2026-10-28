/*
 * Created by Radosław Głasek on 28.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#ifndef CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_SHADER_PROGRAM_GL_BOUND_HXX
#define CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_SHADER_PROGRAM_GL_BOUND_HXX

#include <string_view>

#include "Gl.hxx"
#include "ShaderProgram.hxx"

#include "cad_math/Vec2.hxx"
#include "cad_math/Vec3.hxx"
#include "cad_math/Vec4.hxx"
#include "cad_math/Mat3.hxx"
#include "cad_math/Mat4.hxx"

namespace cadg {

class ShaderProgramGlBoundBorrow final {
public:
    ShaderProgramGlBoundBorrow(const ShaderProgram& program, Gl& gl)
        : m_borrowedProgram(program), m_gl(gl) {}
    ShaderProgramGlBoundBorrow(const ShaderProgram& program, Gl* gl)
        : m_borrowedProgram(program), m_gl(*gl) {}

    [[nodiscard]] bool valid() const {
        return m_borrowedProgram.valid();
    }

    [[nodiscard]] GLuint id() const {
        return m_borrowedProgram.id();
    }

    void bind() const;
    void unbind() const;

    [[nodiscard]] GLint location(std::string_view name) const;

    bool setInt(std::string_view name, int value) const; // NOLINT(*-use-nodiscard)
    bool setIVec2(std::string_view name, int x, int y) const { // NOLINT(*-use-nodiscard)
        return m_borrowedProgram.setIVec2(m_gl, name, x, y);
    }
    bool setFloat(std::string_view name, float value) const; // NOLINT(*-use-nodiscard)
    bool setVec2(std::string_view name, float x, float y) const; // NOLINT(*-use-nodiscard)
    bool setVec2(std::string_view name, const cadm::Vec2& value) const; // NOLINT(*-use-nodiscard)
    bool setVec3(std::string_view name, float x, float y, float z) const; // NOLINT(*-use-nodiscard)
    bool setVec3(std::string_view name, const cadm::Vec3& value) const; // NOLINT(*-use-nodiscard)
    bool setVec4(std::string_view name, float x, float y, float z, float w) const; // NOLINT(*-use-nodiscard)
    bool setVec4(std::string_view name, const cadm::Vec4& value) const; // NOLINT(*-use-nodiscard)

    /// @param columnMajor 16 floats in column-major order
    bool setMat4(std::string_view name, const float* columnMajor) const; // NOLINT(*-use-nodiscard)
    bool setMat4(std::string_view name, const cadm::Mat4& value) const; // NOLINT(*-use-nodiscard)

    /// @param columnMajor 9 floats in column-major order
    bool setMat3(std::string_view name, const float* columnMajor) const; // NOLINT(*-use-nodiscard)
    bool setMat3(std::string_view name, const cadm::Mat3& value) const; // NOLINT(*-use-nodiscard)
private:
    const ShaderProgram& m_borrowedProgram;
    Gl& m_gl;
};


} // namespace cadg

#endif // CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_SHADER_PROGRAM_GL_BOUND_HXX
