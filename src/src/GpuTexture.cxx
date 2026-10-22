/*
 * Created by Radosław Głasek on 22.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#include "cad_graphics/GpuTexture.hxx"

#include <cassert>
#include <utility>

namespace cadg {

GpuTexture::~GpuTexture() {
    assert(m_texture == 0 && "GpuTexture destroyed with a live texture; call destroy() first");
}

GpuTexture::GpuTexture(GpuTexture&& other) noexcept
    : m_texture(std::exchange(other.m_texture, 0)) {
}

GpuTexture& GpuTexture::operator=(GpuTexture&& other) noexcept {
    if (this == &other) {
        return *this;
    }
    assert(m_texture == 0 && "move-assigned over a live GpuTexture; call destroy() first");
    m_texture = std::exchange(other.m_texture, 0);
    return *this;
}

std::expected<GpuTexture, std::string> GpuTexture::upload(Gl& gl, const TextureData& texture) {
    if (texture.empty()) {
        return std::unexpected("cannot upload an empty texture");
    }

    GpuTexture result;
    while (gl.glGetError() != GL_NO_ERROR) {
    }
    gl.glGenTextures(1, &result.m_texture);
    if (result.m_texture == 0) {
        return std::unexpected("glGenTextures failed");
    }

    gl.glBindTexture(GL_TEXTURE_2D, result.m_texture);
    gl.glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    gl.glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        texture.width,
        texture.height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        texture.rgba8.data()
        );
    gl.glGenerateMipmap(GL_TEXTURE_2D);
    gl.glBindTexture(GL_TEXTURE_2D, 0);

    if (const GLenum error = gl.glGetError(); error != GL_NO_ERROR) {
        result.destroy(gl);
        return std::unexpected("OpenGL error while uploading texture (code " + std::to_string(error) + ")");
    }

    return result;
}

void GpuTexture::bind(Gl& gl, const int unit) const {
    gl.glActiveTexture(GL_TEXTURE0 + unit);
    gl.glBindTexture(GL_TEXTURE_2D, m_texture);
}

void GpuTexture::destroy(Gl& gl) {
    if (m_texture != 0) {
        gl.glDeleteTextures(1, &m_texture);
        m_texture = 0;
    }
}

} // namespace cadg