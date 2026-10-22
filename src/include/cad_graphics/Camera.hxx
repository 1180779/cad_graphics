/*
 * Created by Radosław Głasek on 22.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#ifndef CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_CAMERA_HXX
#define CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_CAMERA_HXX

#include <numbers>

#include "cad_math/Mat3.hxx"
#include "cad_math/Mat4.hxx"
#include "cad_math/Vec3.hxx"

namespace cadg {

/// @brief Orbit camera around a target
///
/// <tt>position = target + orbitRot * (0, 0, radius)</tt>
/// 
/// The third column of <tt>orbitRot</tt> points from the target toward the
/// camera
/// 
/// Supports both perspective and orthographic projections
struct Camera {
    cadm::cadf m_radius{5.0};
    cadm::Mat3 m_orbitRot = [] {
        const cadm::Vec3 back = cadm::Vec3{1, 1, 1}.normalized();
        const cadm::Vec3 right = cadm::Vec3{0, 1, 0}.cross(back).normalized();
        return cadm::Mat3{right, back.cross(right), back};
    }();

    cadm::Vec3 m_target{};
    cadm::cadf m_nearPlane{0.1};
    cadm::cadf m_farPlane{200.0};
    cadm::cadf m_fov{std::numbers::pi_v<cadm::cadf> / 4.0};

    /// @brief Vertical world-space height visible in orthographic mode
    cadm::cadf m_orthoHeight{4.0f};
    bool m_isOrtho = false;

    cadm::cadf m_zoomFactor = 1.0;
    cadm::cadf m_aspectRatio{1.0};

    [[nodiscard]] cadm::Vec3 position() const;
    [[nodiscard]] cadm::Vec3 right() const;
    [[nodiscard]] cadm::Vec3 up() const;
    [[nodiscard]] cadm::Vec3 back() const;

    [[nodiscard]] cadm::Mat4 view() const;
    [[nodiscard]] cadm::Mat4 projection() const;

    [[nodiscard]] bool isOrthographic() const;

    /// @brief Switch projection. Enabling <tt>ortho</tt> derives
    /// <tt>orthoHeight</tt> from the current <tt>radius</tt> and <tt>FOV</tt>
    void setOrthographic(bool orthographic);

    void toggleProjection();

    [[nodiscard]] cadm::cadf orthoHeight() const;

    void setOrthoHeight(cadm::cadf height);

    /// @brief Orbit around the target with world-Y yaw and local-X pitch
    void orbit(cadm::cadf yawDelta, cadm::cadf pitchDelta);

    /// @brief Pan the target in the camera plane
    void pan(cadm::cadf dxPixels, cadm::cadf dyPixels, int viewportHeight);

    /// @brief Multiplicative zoom; positive <tt>steps</tt> move closer. Changes
    /// <tt>orthoHeight</tt> in orthographic mode and <tt>radius</tt> in
    /// perspective mode.
    void zoom(cadm::cadf steps);
};

} // namespace cadg

#endif // CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_CAMERA_HXX