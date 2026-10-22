/*
 * Created by Radosław Głasek on 22.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#include "cad_graphics/Bounds.hxx"

#include <algorithm>

#include "cad_math/Vec3.hxx"

namespace cadg {

void Bounds::expand(const cadm::Vec3& point) {
    if (!valid) {
        min = point;
        max = point;
        valid = true;
        return;
    }
    
    min = min.elementWiseMin(point);
    max = max.elementWiseMax(point);
}

void Bounds::expand(const Bounds& other) {
    if (!other.valid) {
        return;
    }
    expand(other.min);
    expand(other.max);
}

cadm::Vec3 Bounds::center() const {
    if (!valid) {
        return {};
    }
    return (min + max) * cadm::cadf{0.5};
}

cadm::Vec3 Bounds::size() const {
    if (!valid) {
        return {};
    }
    return max - min;
}

Bounds Bounds::transformed(const cadm::Mat4& transform) const {
    if (!valid) {
        return {};
    }

    Bounds result;
    for (int i = 0; i < 8; ++i) {
        const cadm::Vec3 corner{
            (i & 1) != 0 ? max.x : min.x,
            (i & 2) != 0 ? max.y : min.y,
            (i & 4) != 0 ? max.z : min.z,
        };
        const cadm::Vec4 transformed = transform * cadm::Vec4{corner, 1.0f};
        result.expand(transformed.xyz());
    }
    return result;
}

} // namespace cadg