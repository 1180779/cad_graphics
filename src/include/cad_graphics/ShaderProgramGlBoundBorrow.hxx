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
    using Vec2f = cadm::Vec<float, 2>;
    using Vec3f = cadm::Vec<float, 3>;
    using Vec4f = cadm::Vec<float, 4>;
    using Mat3f = cadm::Mat<float, 3, 3>;
    using Mat4f = cadm::Mat<float, 4, 4>;

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

    bool uniform1i(std::string_view name, int value) const; // NOLINT(*-use-nodiscard)
    bool uniform2i(std::string_view name, int x, int y) const { // NOLINT(*-use-nodiscard)
        return m_borrowedProgram.uniform2i(m_gl, name, x, y);
    }
    bool uniform1f(std::string_view name, float value) const; // NOLINT(*-use-nodiscard)
    bool uniform2f(std::string_view name, float x, float y) const; // NOLINT(*-use-nodiscard)
    bool uniform2f(std::string_view name, const Vec2f& value) const; // NOLINT(*-use-nodiscard)
    bool uniform3f(std::string_view name, float x, float y, float z) const; // NOLINT(*-use-nodiscard)
    bool uniform3f(std::string_view name, const Vec3f& value) const; // NOLINT(*-use-nodiscard)
    bool uniform4f(std::string_view name, float x, float y, float z, float w) const; // NOLINT(*-use-nodiscard)
    bool uniform4f(std::string_view name, const Vec4f& value) const; // NOLINT(*-use-nodiscard)

    /// @param columnMajor 16 floats in column-major order
    bool uniformMatrix4fv(std::string_view name, const float* columnMajor) const; // NOLINT(*-use-nodiscard)
    bool uniformMatrix4f(std::string_view name, const Mat4f& value) const; // NOLINT(*-use-nodiscard)

    /// @param columnMajor 9 floats in column-major order
    bool uniformMatrix3fv(std::string_view name, const float* columnMajor) const; // NOLINT(*-use-nodiscard)
    bool uniformMatrix3f(std::string_view name, const Mat3f& value) const; // NOLINT(*-use-nodiscard)
private:
    const ShaderProgram& m_borrowedProgram;
    Gl& m_gl;
};


} // namespace cadg

#endif // CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_SHADER_PROGRAM_GL_BOUND_HXX
