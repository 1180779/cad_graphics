/*
 * Created by Radosław Głasek on 22.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#include "cad_graphics/ShaderProgram.hxx"

#include <array>
#include <cassert>
#include <fstream>
#include <sstream>
#include <utility>
#include <vector>

namespace cadg {
namespace {

std::expected<std::string, std::string> readTextFile(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return std::unexpected("failed to open shader file: " + path.string());
    }

    std::ostringstream stream;
    stream << file.rdbuf();
    if (!file.good() && !file.eof()) {
        return std::unexpected("failed to read shader file: " + path.string());
    }
    return stream.str();
}

std::string shaderLog(Gl& gl, const GLuint shader) {
    GLint length = 0;
    gl.glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    if (length <= 1) {
        return {};
    }

    std::vector<GLchar> buffer(static_cast<std::size_t>(length));
    GLsizei written = 0;
    gl.glGetShaderInfoLog(shader, length, &written, buffer.data());
    return std::string(buffer.data(), static_cast<std::size_t>(written));
}

std::string programLog(Gl& gl, const GLuint program) {
    GLint length = 0;
    gl.glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    if (length <= 1) {
        return {};
    }

    std::vector<GLchar> buffer(static_cast<std::size_t>(length));
    GLsizei written = 0;
    gl.glGetProgramInfoLog(program, length, &written, buffer.data());
    return std::string(buffer.data(), static_cast<std::size_t>(written));
}

GLenum toGLStage(const ShaderStage stage) {
    switch (stage) {
    case ShaderStage::Vertex:
        return GL_VERTEX_SHADER;
    case ShaderStage::Fragment:
        return GL_FRAGMENT_SHADER;
    case ShaderStage::Geometry:
        return GL_GEOMETRY_SHADER;
    case ShaderStage::TessControl:
        return GL_TESS_CONTROL_SHADER;
    case ShaderStage::TessEvaluation:
        return GL_TESS_EVALUATION_SHADER;
    }
    return GL_VERTEX_SHADER;
}

std::expected<GLuint, std::string> compileStage(
    Gl& gl,
    const ShaderSource& source,
    const std::string& stageName
    ) {
    const GLuint shader = gl.glCreateShader(toGLStage(source.stage));
    if (shader == 0) {
        return std::unexpected("glCreateShader failed for " + stageName);
    }

    const char* text = source.source.c_str();
    gl.glShaderSource(shader, 1, &text, nullptr);
    gl.glCompileShader(shader);

    GLint success = GL_FALSE;
    gl.glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        const std::string log = shaderLog(gl, shader);
        gl.glDeleteShader(shader);
        const std::string label = source.debugName.empty() ? stageName : source.debugName;
        return std::unexpected(
            "shader compilation failed (" + label + "):\n" + (log.empty() ? "<no log>" : log)
            );
    }

    return shader;
}

} // namespace

ShaderProgram::ShaderProgram() = default;

ShaderProgram::~ShaderProgram() {
    assert(m_program == 0 && "ShaderProgram destroyed with a live program; call destroy() first");
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
    : m_program(std::exchange(other.m_program, 0)),
      m_uniformCache(std::move(other.m_uniformCache)) {
    other.m_uniformCache.clear();
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {
    if (this == &other) {
        return *this;
    }
    assert(m_program == 0 && "move-assigned over a live ShaderProgram; call destroy() first");
    m_program = std::exchange(other.m_program, 0);
    m_uniformCache = std::move(other.m_uniformCache);
    other.m_uniformCache.clear();
    return *this;
}

std::string ShaderProgram::stageName(const ShaderStage stage) {
    switch (stage) {
    case ShaderStage::Vertex:
        return "vertex";
    case ShaderStage::Fragment:
        return "fragment";
    case ShaderStage::Geometry:
        return "geometry";
    case ShaderStage::TessControl:
        return "tess-control";
    case ShaderStage::TessEvaluation:
        return "tess-evaluation";
    }
    return "unknown";
}

std::expected<ShaderProgram, std::string> ShaderProgram::fromSources(
    Gl& gl,
    const std::span<const ShaderSource> stages
    ) {
    if (stages.empty()) {
        return std::unexpected("no shader stages supplied");
    }

    std::vector<GLuint> shaders;
    shaders.reserve(stages.size());

    for (const auto& stage : stages) {
        const std::string stageLabel = stageName(stage.stage);
        auto compiled = compileStage(gl, stage, stageLabel);
        if (!compiled) {
            for (const GLuint shader : shaders) {
                gl.glDeleteShader(shader);
            }
            return std::unexpected(std::move(compiled.error()));
        }
        shaders.push_back(*compiled);
    }

    const GLuint program = gl.glCreateProgram();
    if (program == 0) {
        for (const GLuint shader : shaders) {
            gl.glDeleteShader(shader);
        }
        return std::unexpected("glCreateProgram failed");
    }

    for (const GLuint shader : shaders) {
        gl.glAttachShader(program, shader);
    }

    gl.glLinkProgram(program);

    for (const GLuint shader : shaders) {
        gl.glDetachShader(program, shader);
        gl.glDeleteShader(shader);
    }

    GLint success = GL_FALSE;
    gl.glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        const std::string log = programLog(gl, program);
        gl.glDeleteProgram(program);
        return std::unexpected("shader linking failed:\n" + (log.empty() ? "<no log>" : log));
    }

    ShaderProgram result;
    result.m_program = program;
    return result;
}

std::expected<ShaderProgram, std::string> ShaderProgram::fromFiles(
    Gl& gl,
    const std::span<const ShaderFile> files
    ) {
    if (files.empty()) {
        return std::unexpected("no shader files supplied");
    }

    std::vector<ShaderSource> sources;
    sources.reserve(files.size());
    for (const auto& file : files) {
        auto text = readTextFile(file.path);
        if (!text) {
            return std::unexpected(std::move(text.error()));
        }
        sources.push_back(ShaderSource{
            .stage = file.stage,
            .source = std::move(*text),
            .debugName = file.path.string(),
        });
    }

    return fromSources(gl, sources);
}

std::expected<ShaderProgram, std::string> ShaderProgram::fromFiles(
    Gl& gl,
    const std::filesystem::path& vertexPath,
    const std::filesystem::path& fragmentPath
    ) {
    const std::array files{
        ShaderFile{.stage = ShaderStage::Vertex, .path = vertexPath},
        ShaderFile{.stage = ShaderStage::Fragment, .path = fragmentPath},
    };
    return fromFiles(gl, files);
}

void ShaderProgram::destroy(Gl& gl) {
    if (m_program != 0) {
        gl.glDeleteProgram(m_program);
        m_program = 0;
    }
    m_uniformCache.clear();
}

void ShaderProgram::bind(Gl& gl) const {
    gl.glUseProgram(m_program);
}

void ShaderProgram::unbind(Gl& gl) {
    gl.glUseProgram(0);
}

GLint ShaderProgram::location(Gl& gl, const std::string_view name) const {
    const std::string key{name};
    if (const auto it = m_uniformCache.find(key); it != m_uniformCache.end()) {
        return it->second;
    }

    const GLint result = gl.glGetUniformLocation(m_program, key.c_str());
    m_uniformCache.emplace(key, result);
    return result;
}

bool ShaderProgram::setInt(Gl& gl, const std::string_view name, const int value) const {
    const GLint loc = location(gl, name);
    if (loc < 0) {
        return false;
    }
    gl.glUniform1i(loc, value);
    return true;
}

bool ShaderProgram::setFloat(Gl& gl, const std::string_view name, const float value) const {
    const GLint loc = location(gl, name);
    if (loc < 0) {
        return false;
    }
    gl.glUniform1f(loc, value);
    return true;
}

bool ShaderProgram::setVec2(Gl& gl, const std::string_view name, const float x, const float y) const {
    const GLint loc = location(gl, name);
    if (loc < 0) {
        return false;
    }
    gl.glUniform2f(loc, x, y);
    return true;
}

bool ShaderProgram::setVec2(Gl& gl, const std::string_view name, const cadm::Vec2& value) const {
    return setVec2(gl, name, value.x, value.y);
}

bool ShaderProgram::setVec3(Gl& gl, const std::string_view name, const float x, const float y, const float z) const {
    const GLint loc = location(gl, name);
    if (loc < 0) {
        return false;
    }
    gl.glUniform3f(loc, x, y, z);
    return true;
}

bool ShaderProgram::setVec3(Gl& gl, const std::string_view name, const cadm::Vec3& value) const {
    return setVec3(gl, name, value.x, value.y, value.z);
}

bool ShaderProgram::setVec4(
    Gl& gl,
    const std::string_view name,
    const float x,
    const float y,
    const float z,
    const float w
    ) const {
    const GLint loc = location(gl, name);
    if (loc < 0) {
        return false;
    }
    gl.glUniform4f(loc, x, y, z, w);
    return true;
}

bool ShaderProgram::setVec4(Gl& gl, const std::string_view name, const cadm::Vec4& value) const {
    return setVec4(gl, name, value.x, value.y, value.z, value.w);
}

bool ShaderProgram::setMat4(Gl& gl, const std::string_view name, const float* columnMajor) const {
    const GLint loc = location(gl, name);
    if (loc < 0) {
        return false;
    }
    gl.glUniformMatrix4fv(loc, 1, GL_FALSE, columnMajor);
    return true;
}

bool ShaderProgram::setMat4(Gl& gl, const std::string_view name, const cadm::Mat4& value) const {
    return setMat4(gl, name, value.data);
}

bool ShaderProgram::setMat3(Gl& gl, const std::string_view name, const float* columnMajor) const {
    const GLint loc = location(gl, name);
    if (loc < 0) {
        return false;
    }
    gl.glUniformMatrix3fv(loc, 1, GL_FALSE, columnMajor);
    return true;
}

bool ShaderProgram::setMat3(Gl& gl, std::string_view name, const cadm::Mat3& value) const {
    return setMat3(gl, name, value.data);
}

} // namespace cadg