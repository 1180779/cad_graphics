/*
 * Created by Radosław Głasek on 22.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#ifndef CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_SHADER_PROGRAM_HXX
#define CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_SHADER_PROGRAM_HXX

#include <expected>
#include <filesystem>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>

#include "Gl.hxx"

#include "cad_math/Vec2.hxx"
#include "cad_math/Vec3.hxx"
#include "cad_math/Vec4.hxx"
#include "cad_math/Mat3.hxx"
#include "cad_math/Mat4.hxx"

namespace cadg {

enum class ShaderStage {
    Vertex,
    Fragment,
    Geometry,
    TessControl,
    TessEvaluation,
};

struct ShaderSource {
    ShaderStage stage;
    std::string source;
    std::string debugName;
};

struct ShaderFile {
    ShaderStage stage;
    std::filesystem::path path;
};

class ShaderProgramGlBoundBorrow;

/// @brief Compiled and linked GLSL program
///
/// The destructor never issues GL calls. it only asserts that the handles were
/// released
class ShaderProgram final {
    using Vec2f = cadm::Vec<float, 2>;
    using Vec3f = cadm::Vec<float, 3>;
    using Vec4f = cadm::Vec<float, 4>;
    using Mat3f = cadm::Mat<float, 3, 3>;
    using Mat4f = cadm::Mat<float, 4, 4>;

public:
    ShaderProgram();
    ~ShaderProgram();

    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    ShaderProgram(ShaderProgram&& other) noexcept;
    ShaderProgram& operator=(ShaderProgram&& other) noexcept;

    ShaderProgramGlBoundBorrow borrowWithGlBound(Gl& gl) const;
    
    static std::expected<ShaderProgram, std::string> fromSources(
        Gl& gl,
        std::span<const ShaderSource> stages
        );
    static std::expected<ShaderProgram, std::string> fromSources(
        Gl* gl,
        std::span<const ShaderSource> stages
    );

    static std::expected<ShaderProgram, std::string> fromFiles(
        Gl& gl,
        std::span<const ShaderFile> files
    );
    static std::expected<ShaderProgram, std::string> fromFiles(
        Gl* gl,
        std::span<const ShaderFile> files
    );

    static std::expected<ShaderProgram, std::string> fromFiles(
        Gl& gl,
        std::filesystem::path vertexPath,
        std::filesystem::path fragmentPath
    );
    static std::expected<ShaderProgram, std::string> fromFiles(
        Gl* gl,
        std::filesystem::path vertexPath,
        std::filesystem::path fragmentPath
    );

    /// @brief Delete the GL program
    /// @pre Context must be current
    void destroy(Gl& gl);
    void destroy(Gl* gl);


    [[nodiscard]] bool valid() const {
        return m_program != 0;
    }

    [[nodiscard]] GLuint id() const {
        return m_program;
    }

    void bind(Gl& gl) const;
    void bind(Gl* gl) const;

    static void unbind(Gl& gl);

    /// @brief Cached glGetUniformLocation lookup
    /// @returns -1 when the uniform does not exist
    [[nodiscard]] GLint location(Gl& gl, std::string_view name) const;
    [[nodiscard]] GLint location(Gl* gl, std::string_view name) const;

    bool uniform1i(Gl& gl, std::string_view name, int value) const; // NOLINT(*-use-nodiscard)
    bool uniform1i(Gl* gl, std::string_view name, int value) const; // NOLINT(*-use-nodiscard)
    bool uniform2i(Gl& gl, std::string_view name, int x, int y) const; // NOLINT(*-use-nodiscard)
    bool uniform2i(Gl* gl, std::string_view name, int x, int y) const; // NOLINT(*-use-nodiscard)
    bool uniform2i(Gl& gl, std::string_view name, const cadm::Vec2I& value) const; // NOLINT(*-use-nodiscard)
    bool uniform2i(Gl* gl, std::string_view name, const cadm::Vec2I& value) const; // NOLINT(*-use-nodiscard)
    /// Float setters use 32-bit GPU types, independently of cadm::cadf.
    bool uniform1f(Gl& gl, std::string_view name, float value) const; // NOLINT(*-use-nodiscard)
    bool uniform1f(Gl* gl, std::string_view name, float value) const; // NOLINT(*-use-nodiscard)
    bool uniform2f(Gl& gl, std::string_view name, float x, float y) const; // NOLINT(*-use-nodiscard)
    bool uniform2f(Gl* gl, std::string_view name, float x, float y) const; // NOLINT(*-use-nodiscard)
    bool uniform2f(Gl& gl, std::string_view name, const Vec2f& value) const; // NOLINT(*-use-nodiscard)
    bool uniform2f(Gl* gl, std::string_view name, const Vec2f& value) const; // NOLINT(*-use-nodiscard)
    bool uniform3f(Gl& gl, std::string_view name, float x, float y, float z) const; // NOLINT(*-use-nodiscard)
    bool uniform3f(Gl* gl, std::string_view name, float x, float y, float z) const; // NOLINT(*-use-nodiscard)
    bool uniform3f(Gl& gl, std::string_view name, const Vec3f& value) const; // NOLINT(*-use-nodiscard)
    bool uniform3f(Gl* gl, std::string_view name, const Vec3f& value) const; // NOLINT(*-use-nodiscard)
    bool uniform4f(Gl& gl, std::string_view name, float x, float y, float z, float w) const; // NOLINT(*-use-nodiscard)
    bool uniform4f(Gl* gl, std::string_view name, float x, float y, float z, float w) const; // NOLINT(*-use-nodiscard)
    bool uniform4f(Gl& gl, std::string_view name, const Vec4f& value) const; // NOLINT(*-use-nodiscard)
    bool uniform4f(Gl* gl, std::string_view name, const Vec4f& value) const; // NOLINT(*-use-nodiscard)

    /// @param columnMajor 16 floats in column-major order
    bool uniformMatrix4fv(Gl& gl, std::string_view name, const float* columnMajor) const; // NOLINT(*-use-nodiscard)
    bool uniformMatrix4fv(Gl* gl, std::string_view name, const float* columnMajor) const; // NOLINT(*-use-nodiscard)
    bool uniformMatrix4f(Gl& gl, std::string_view name, const Mat4f& value) const; // NOLINT(*-use-nodiscard)
    bool uniformMatrix4f(Gl* gl, std::string_view name, const Mat4f& value) const; // NOLINT(*-use-nodiscard)

    /// @param columnMajor 9 floats in column-major order
    bool uniformMatrix3fv(Gl& gl, std::string_view name, const float* columnMajor) const; // NOLINT(*-use-nodiscard)
    bool uniformMatrix3fv(Gl* gl, std::string_view name, const float* columnMajor) const; // NOLINT(*-use-nodiscard)
    bool uniformMatrix3f(Gl& gl, std::string_view name, const Mat3f& value) const; // NOLINT(*-use-nodiscard)
    bool uniformMatrix3f(Gl* gl, std::string_view name, const Mat3f& value) const; // NOLINT(*-use-nodiscard)


private:
    static std::string stageName(ShaderStage stage);

    GLuint m_program = 0;
    mutable std::unordered_map<std::string, GLint> m_uniformCache;
};

} // namespace cadg


#endif // CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_SHADER_PROGRAM_HXX