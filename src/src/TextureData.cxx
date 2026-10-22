/*
 * Created by Radosław Głasek on 22.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#include "cad_graphics/TextureData.hxx"

#include <QImage>
#include <QString>

#include <limits>

namespace cadg {
namespace {

QString toQString(const std::filesystem::path& path) {
#if defined(_WIN32)
    return QString::fromStdWString(path.wstring());
#else
    return QString::fromStdString(path.string());
#endif
}

std::expected<TextureData, std::string> fromQImage(const QImage& image, const std::string& label) {
    if (image.isNull()) {
        return std::unexpected("image is empty: " + label);
    }

    const QImage rgba = image.convertToFormat(QImage::Format_RGBA8888).mirrored(false, true);
    if (rgba.isNull()) {
        return std::unexpected("failed to convert image to RGBA8: " + label);
    }

    TextureData result;
    result.width = rgba.width();
    result.height = rgba.height();
    result.rgba8.assign(rgba.constBits(), rgba.constBits() + rgba.sizeInBytes());
    return result;
}

} // namespace

std::expected<TextureData, std::string> TextureData::load(const std::filesystem::path& path) {
    QImage image;
    if (!image.load(toQString(path))) {
        return std::unexpected("failed to load texture image: " + path.string());
    }
    return fromQImage(image, path.string());
}

std::expected<TextureData, std::string> TextureData::fromEncoded(const std::span<const std::uint8_t> encoded) {
    if (encoded.empty()) {
        return std::unexpected("encoded texture data is empty");
    }
    if (encoded.size() > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
        return std::unexpected("encoded texture data is too large");
    }

    const QImage image = QImage::fromData(encoded.data(), static_cast<int>(encoded.size()));
    return fromQImage(image, "embedded texture");
}

std::expected<TextureData, std::string> TextureData::fromRgba8(
    const int width,
    const int height,
    const std::span<const std::uint8_t> rgba8
    ) {
    if (width <= 0 || height <= 0) {
        return std::unexpected("texture dimensions must be positive");
    }
    if (rgba8.size() != static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4) {
        return std::unexpected("RGBA8 texture data size does not match its dimensions");
    }

    TextureData result;
    result.width = width;
    result.height = height;
    result.rgba8.assign(rgba8.begin(), rgba8.end());
    return result;
}

} // namespace cadg