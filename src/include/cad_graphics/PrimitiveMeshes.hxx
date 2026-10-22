/*
 * Created by Radosław Głasek on 22.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#ifndef CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_PRIMITIVE_MESHES_HXX
#define CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_PRIMITIVE_MESHES_HXX

#include "MeshData.hxx"

#include "cad_math/Vec3.hxx"

namespace cadg {

/// @brief Parameters for a helical spring mesh
///
/// The centerline follows the same parametrization as computer graphics 2
/// course spring shader:
/// <tt>s</tt> in <tt>[0,1]</tt> maps to <tt>y</tt> in <tt>[0,height]</tt>, the
/// helix angle is derived from the unwrapped centerline length, and the wire is
/// swept with a circular section
struct SpringParameters final {
    float height = 1.5f;
    float length = 15.0f;
    float radius = 0.5f;
    float wireRadius = 0.1f;
    int segmentsAlong = 256;
    int segmentsAround = 24;
};

/// @brief Axis-aligned box centered at the origin
MeshData makeBox(const cadm::Vec3& size);

/// @brief Capped cylinder centered at the origin, axis along Y
MeshData makeCylinder(float radius, float height, int sectorCount = 32, int stackCount = 5);

/// @brief Helical spring wire, starting at <tt>y=0</tt> and ending at
/// <tt>y=height</tt>
MeshData makeSpring(const SpringParameters& parameters = {});

} // namespace cadg


#endif // CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_PRIMITIVE_MESHES_HXX