/*
 * Created by Radosław Głasek on 22.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#ifndef CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_MATERIAL_HXX
#define CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_MATERIAL_HXX

#include <memory>
#include <string>

#include "TextureData.hxx"

#include "cad_math/Vec3.hxx"

namespace cadg {

/// @brief CPU-side Phong material
struct Material final {
    std::string name;
    cadm::Vec3 diffuseColor{0.8f, 0.8f, 0.8f};
    cadm::Vec3 specularColor{0.2f, 0.2f, 0.2f};
    float shininess = 32.0f;
    float opacity = 1.0f;
    bool doubleSided = false;

    std::shared_ptr<const TextureData> diffuseMap;

    [[nodiscard]] bool hasAnyTexture() const {
        return diffuseMap != nullptr;
    }
};

} // namespace cadg

#endif // CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_MATERIAL_HXX