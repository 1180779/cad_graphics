/*
 * Created by Radosław Głasek on 22.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#ifndef CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_MESH_DATA_HXX
#define CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_MESH_DATA_HXX

#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>

#include "cad_math/Vec2.hxx"
#include "cad_math/Vec3.hxx"

namespace cadg {

/// @brief Vertex layout
///
/// Attribute locations: <br/>
///   0 - position  (<tt>Vec3</tt>) <br/>
///   1 - normal    (<tt>Vec3</tt>) <br/>
///   2 - texCoord  (<tt>Vec2</tt>)
struct Vertex final {
    cadm::Vec3 position{};
    cadm::Vec3 normal{};
    cadm::Vec2 texCoord{};
};

static_assert(std::is_standard_layout_v<Vertex>);
static_assert(sizeof(Vertex) == 8 * sizeof(float));
static_assert(offsetof(Vertex, position) == 0 * sizeof(float));
static_assert(offsetof(Vertex, normal) == 3 * sizeof(float));
static_assert(offsetof(Vertex, texCoord) == 6 * sizeof(float));

/// @brief Indexed triangle mesh data
struct MeshData final {
    std::string name;
    std::vector<Vertex> vertices;
    std::vector<std::uint32_t> indices;

    [[nodiscard]] bool empty() const {
        return vertices.empty() || indices.empty();
    }
};

} // namespace cadg

#endif // CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_MESH_DATA_HXX