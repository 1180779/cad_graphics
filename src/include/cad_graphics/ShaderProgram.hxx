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

/// @brief Compiled and linked GLSL program
///
/// The destructor never issues GL calls. it only asserts that the handles were
/// released
class ShaderProgram final {
public:
    ShaderProgram();
    ~ShaderProgram();

    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    ShaderProgram(ShaderProgram&& other) noexcept;
    ShaderProgram& operator=(ShaderProgram&& other) noexcept;

    static std::expected<ShaderProgram, std::string> fromSources(
        Gl& gl,
        std::span<const ShaderSource> stages
        );

    static std::expected<ShaderProgram, std::string> fromFiles(
        Gl& gl,
        std::span<const ShaderFile> files
        );

    static std::expected<ShaderProgram, std::string> fromFiles(
        Gl& gl,
        const std::filesystem::path& vertexPath,
        const std::filesystem::path& fragmentPath
        );

    /// @brief Delete the GL program
    /// @pre Context must be current
    void destroy(Gl& gl);

    [[nodiscard]] bool valid() const {
        return m_program != 0;
    }

    [[nodiscard]] GLuint id() const {
        return m_program;
    }

    void bind(Gl& gl) const;

    static void unbind(Gl& gl);

    /// @brief Cached glGetUniformLocation lookup
    /// @returns -1 when the uniform does not exist
    [[nodiscard]] GLint location(Gl& gl, std::string_view name) const;

    bool setInt(Gl& gl, std::string_view name, int value) const;
    bool setFloat(Gl& gl, std::string_view name, float value) const;
    bool setVec2(Gl& gl, std::string_view name, float x, float y) const;
    bool setVec2(Gl& gl, std::string_view name, const cadm::Vec2& value) const;
    bool setVec3(Gl& gl, std::string_view name, float x, float y, float z) const;
    bool setVec3(Gl& gl, std::string_view name, const cadm::Vec3& value) const;
    bool setVec4(Gl& gl, std::string_view name, float x, float y, float z, float w) const;
    bool setVec4(Gl& gl, std::string_view name, const cadm::Vec4& value) const;

    /// @param columnMajor 16 floats in column-major order
    bool setMat4(Gl& gl, std::string_view name, const float* columnMajor) const;
    bool setMat4(Gl& gl, std::string_view name, const cadm::Mat4& value) const;

    /// @param columnMajor 9 floats in column-major order
    bool setMat3(Gl& gl, std::string_view name, const float* columnMajor) const;
    bool setMat3(Gl& gl, std::string_view name, const cadm::Mat3& value) const;

private:
    static std::string stageName(ShaderStage stage);

    GLuint m_program = 0;
    mutable std::unordered_map<std::string, GLint> m_uniformCache;
};

} // namespace cadg


#endif // CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_SHADER_PROGRAM_HXX