/*
 * Created by Radosław Głasek on 28.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#ifndef CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_VERTEX_LAYOUT_HXX
#define CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_VERTEX_LAYOUT_HXX

#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>
#include <type_traits>

#include "Gl.hxx"

namespace cadg {

struct VertexAttribute final {
    GLuint location{};
    GLint components{};
    GLenum type{GL_FLOAT};
    GLboolean normalized{GL_FALSE};
    std::size_t offset{};
};

struct VertexLayout final {
    std::size_t stride{};
    std::span<const VertexAttribute> attributes;
};

struct MeshView final {
    std::string_view name;
    std::span<const std::byte> vertices;
    std::size_t vertexCount{};
    std::span<const std::uint32_t> indices;
    VertexLayout layout;
};

template <typename VertexT>
[[nodiscard]] MeshView makeMeshView(
    const std::string_view name,
    const std::span<const VertexT> vertices,
    const std::span<const std::uint32_t> indices,
    const VertexLayout layout
) {
    static_assert(
        std::is_standard_layout_v<VertexT>,
        "makeMeshView requires a standard-layout vertex type"
    );
    static_assert(
        std::is_trivially_copyable_v<VertexT>,
        "makeMeshView requires a trivially copyable vertex type"
    );

    return {
        .name = name,
        .vertices = std::as_bytes(vertices),
        .vertexCount = vertices.size(),
        .indices = indices,
        .layout = layout,
    };
}

} // namespace cadg

#endif // CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_VERTEX_LAYOUT_HXX