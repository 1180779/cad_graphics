/*
 * Created by Radosław Głasek on 22.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#include "cad_graphics/Model.hxx"

#include "cad_math/Vec4.hxx"

namespace cadg {

Bounds Model::bounds() const {
    Bounds result;
    for (const auto& part : parts) {
        if (!part.mesh) {
            continue;
        }
        for (const auto& vertex : part.mesh->vertices) {
            const cadm::Vec4 position = part.nodeTransform * cadm::Vec4{vertex.position, 1.0f};
            result.expand(position.xyz());
        }
    }
    return result;
}

} // namespace cadg