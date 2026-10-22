/*
 * Created by Radosław Głasek on 22.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#ifndef CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_GPU_TEXTURE_HXX
#define CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_GPU_TEXTURE_HXX

#include <expected>
#include <string>

#include "Gl.hxx"
#include "TextureData.hxx"

namespace cadg {

/// @brief Owns one GL_TEXTURE_2D
///
/// The destructor never issues GL calls. it only asserts that the handles were
/// released
class GpuTexture final {
public:
    GpuTexture() = default;
    ~GpuTexture();

    GpuTexture(const GpuTexture&) = delete;
    GpuTexture& operator=(const GpuTexture&) = delete;

    GpuTexture(GpuTexture&& other) noexcept;
    GpuTexture& operator=(GpuTexture&& other) noexcept;

    static std::expected<GpuTexture, std::string> upload(Gl& gl, const TextureData& texture);

    void bind(Gl& gl, int unit) const;

    void destroy(Gl& gl);

    [[nodiscard]] bool valid() const {
        return m_texture != 0;
    }

    [[nodiscard]] GLuint id() const {
        return m_texture;
    }

private:
    GLuint m_texture = 0;
};

} // namespace cadg


#endif // CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_GPU_TEXTURE_HXX