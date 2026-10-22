/*
 * Created by Radosław Głasek on 22.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#include <cad_graphics/Camera.hxx>

#include <algorithm>
#include <cmath>

namespace cadg {

cadm::Vec3 Camera::position() const {
    return m_target + m_orbitRot * cadm::Vec3{cadm::cadf{0.0}, cadm::cadf{0.0}, m_radius};
}

cadm::Vec3 Camera::right() const {
    return m_orbitRot.col(0);
}

cadm::Vec3 Camera::up() const {
    return m_orbitRot.col(1);
}

cadm::Vec3 Camera::back() const {
    return m_orbitRot.col(2);
}

cadm::Mat4 Camera::view() const {
    return cadm::Mat4::lookAtRh(position(), m_target, up());
}

cadm::Mat4 Camera::projection() const {
    const cadm::cadf aspect = m_aspectRatio > 0.0 ? m_aspectRatio : 1.0;

    if (m_isOrtho) {
        const cadm::cadf height = std::max(m_orthoHeight, cadm::cadf{1e-3});
        const cadm::cadf width = height * aspect;
        return cadm::Mat4::ortho(
            -width * cadm::cadf{0.5},
            width * cadm::cadf{0.5},
            -height * cadm::cadf{0.5},
            height * cadm::cadf{0.5},
            m_nearPlane,
            m_farPlane
            );
    }

    return cadm::Mat4::perspective(aspect, m_fov, m_nearPlane, m_farPlane);
}

bool Camera::isOrthographic() const {
    return m_isOrtho;
}

void Camera::setOrthographic(const bool orthographic) {
    if (orthographic == m_isOrtho) {
        return;
    }

    if (orthographic) {
        const cadm::cadf fittedHeight =
            2.0f * m_radius * std::tan(m_fov * cadm::cadf{0.5});
        m_orthoHeight = std::max(fittedHeight, cadm::cadf{1e-3});
    }
    m_isOrtho = orthographic;
}

void Camera::toggleProjection() {
    setOrthographic(!m_isOrtho);
}

cadm::cadf Camera::orthoHeight() const {
    return m_orthoHeight;
}

void Camera::setOrthoHeight(const cadm::cadf height) {
    m_orthoHeight = std::clamp(height, cadm::cadf{0.05}, cadm::cadf{500.0});
}

void Camera::orbit(const cadm::cadf yawDelta, const cadm::cadf pitchDelta) {
    cadm::Vec3 orbitBack = back();
    cadm::cadf pitch = std::asin(std::clamp(orbitBack.y, cadm::cadf{-1.0}, cadm::cadf{1.0}));
    cadm::cadf yaw = std::atan2(orbitBack.x, orbitBack.z);

    yaw += yawDelta;
    pitch = std::clamp(pitch + pitchDelta, cadm::cadf{-1.45}, cadm::cadf{1.45});

    orbitBack = {
        std::cos(pitch) * std::sin(yaw),
        std::sin(pitch),
        std::cos(pitch) * std::cos(yaw),
    };

    constexpr cadm::Vec3 worldUp{cadm::cadf{0.0}, cadm::cadf{1.0}, cadm::cadf{0.0}};
    const cadm::Vec3 orbitRight = worldUp.cross(orbitBack).normalized();
    const cadm::Vec3 orbitUp = orbitBack.cross(orbitRight);
    m_orbitRot = cadm::Mat3{orbitRight, orbitUp, orbitBack};
}

void Camera::pan(const cadm::cadf dxPixels, const cadm::cadf dyPixels, const int viewportHeight) {
    if (viewportHeight <= 0) {
        return;
    }

    const cadm::cadf worldHeight = m_isOrtho
                                       ? m_orthoHeight
                                       : cadm::cadf{2.0} * m_radius * std::tan(m_fov * cadm::cadf{0.5});
    const cadm::cadf scale = worldHeight / static_cast<cadm::cadf>(viewportHeight);
    m_target += right() * (-dxPixels * scale) + up() * (dyPixels * scale);
}

void Camera::zoom(const cadm::cadf steps) {
    const cadm::cadf factor = std::exp(-steps * cadm::cadf{0.15});
    if (m_isOrtho) {
        m_orthoHeight = std::clamp(m_orthoHeight * factor, cadm::cadf{0.05}, cadm::cadf{500.0});
    } else {
        m_radius = std::clamp(m_radius * factor, cadm::cadf{0.25}, cadm::cadf{100.0});
    }
}

} // namespace cadg