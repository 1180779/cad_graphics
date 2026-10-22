/*
 * Created by Radosław Głasek on 22.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#ifndef CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_MODEL_HXX
#define CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_MODEL_HXX

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "Bounds.hxx"
#include "Material.hxx"
#include "MeshData.hxx"

#include "cad_math/Mat4.hxx"

namespace cadg {

/// @brief One drawable CPU-side mesh instance in model space
struct ModelPart final {
    std::shared_ptr<const MeshData> mesh;
    std::size_t materialIndex = 0;
    /// @brief Model-space transform applied to the mesh
    cadm::Mat4 nodeTransform = cadm::Mat4::identity();
};

/// @brief CPU-side model
struct Model final {
    std::string name;
    std::vector<ModelPart> parts;
    std::vector<Material> materials;

    [[nodiscard]] Bounds bounds() const;
};

} // namespace cadg

#endif // CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_MODEL_HXX