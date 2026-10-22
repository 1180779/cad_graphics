/*
 * Created by Radosław Głasek on 22.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#ifndef CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_BOUNDS_HXX
#define CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_BOUNDS_HXX

#include "cad_math/Mat4.hxx"
#include "cad_math/Vec3.hxx"

namespace cadg {

/// @brief Axis-aligned bounding box in model space
struct Bounds final {
    cadm::Vec3 min{};
    cadm::Vec3 max{};
    bool valid = false;

    void expand(const cadm::Vec3& point);
    void expand(const Bounds& other);

    [[nodiscard]] cadm::Vec3 center() const;
    [[nodiscard]] cadm::Vec3 size() const;

    /// @brief Transform the eight corners and return the AABB of the result
    [[nodiscard]] Bounds transformed(const cadm::Mat4& transform) const;
};

} // namespace cadg


#endif // CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_BOUNDS_HXX