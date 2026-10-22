/*
 * Created by Radosław Głasek on 22.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#ifndef CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_TEXTURE_DATA_HXX
#define CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_TEXTURE_DATA_HXX

#include <cstdint>
#include <expected>
#include <filesystem>
#include <span>
#include <string>
#include <vector>

namespace cadg {

/// @brief RGBA8 texture pixels
///
/// Loaders flip image files vertically so UV (0,0) addresses the bottom-left
/// texel, matching OpenGL's texture origin
struct TextureData final {
    int width = 0;
    int height = 0;
    std::vector<std::uint8_t> rgba8;

    [[nodiscard]] bool empty() const {
        return width <= 0 || height <= 0 || rgba8.size() != static_cast<std::size_t>(width) * height * 4;
    }

    static std::expected<TextureData, std::string> load(const std::filesystem::path& path);

    static std::expected<TextureData, std::string> fromEncoded(std::span<const std::uint8_t> encoded);

    static std::expected<TextureData, std::string> fromRgba8(
        int width,
        int height,
        std::span<const std::uint8_t> rgba8
        );
};

} // namespace cadg

#endif // CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_TEXTURE_DATA_HXX