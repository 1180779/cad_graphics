/*
 * Created by Radosław Głasek on 22.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#include <cad_graphics/PrimitiveMeshes.hxx>

#include <algorithm>
#include <array>
#include <cmath>
#include <numbers>
#include <vector>

namespace cadg {
namespace {

using Index = std::uint32_t;

void addTriangle(std::vector<Index>& indices, const Index a, const Index b, const Index c) {
    indices.push_back(a);
    indices.push_back(b);
    indices.push_back(c);
}

} // namespace

MeshData makeBox(const cadm::Vec3& size) {
    MeshData mesh;
    mesh.name = "box";

    const float hx = std::max(size.x, 0.0f) * 0.5f;
    const float hy = std::max(size.y, 0.0f) * 0.5f;
    const float hz = std::max(size.z, 0.0f) * 0.5f;

    const auto addFace = [&](const std::array<cadm::Vec3, 4>& positions,
        const cadm::Vec3& normal,
        const std::array<cadm::Vec2, 4>& uvs) {
        const auto base = static_cast<Index>(mesh.vertices.size());
        for (std::size_t i = 0; i < positions.size(); ++i) {
            mesh.vertices.push_back(Vertex{
                .position = positions[i],
                .normal = normal,
                .texCoord = uvs[i]
            });
        }
        addTriangle(mesh.indices, base + 0, base + 1, base + 2);
        addTriangle(mesh.indices, base + 2, base + 3, base + 0);
    };

    // back face (Z = -hz)
    addFace(
        {{
            {-hx, -hy, -hz},
            {-hx, hy, -hz},
            {hx, hy, -hz},
            {hx, -hy, -hz},
        }},
        {0.0f, 0.0f, -1.0f},
        {
            {
                {0.0f, 0.0f},
                {0.0f, 1.0f},
                {1.0f, 1.0f},
                {1.0f, 0.0f}
            }}
        );

    // Front face (Z = +hz)
    addFace(
        {{
            {-hx, -hy, hz},
            {hx, -hy, hz},
            {hx, hy, hz},
            {-hx, hy, hz},
        }},
        {0.0f, 0.0f, 1.0f},
        {
            {
                {0.0f, 0.0f},
                {1.0f, 0.0f},
                {1.0f, 1.0f},
                {0.0f, 1.0f}
            }}
        );

    // Left face (X = -hx)
    addFace(
        {{
            {-hx, -hy, -hz},
            {-hx, -hy, hz},
            {-hx, hy, hz},
            {-hx, hy, -hz},
        }},
        {-1.0f, 0.0f, 0.0f},
        {
            {
                {0.0f, 0.0f},
                {1.0f, 0.0f},
                {1.0f, 1.0f},
                {0.0f, 1.0f}
            }}
        );

    // Right face (X = +hx)
    addFace(
        {{
            {hx, -hy, -hz},
            {hx, hy, -hz},
            {hx, hy, hz},
            {hx, -hy, hz},
        }},
        {1.0f, 0.0f, 0.0f},
        {
            {
                {0.0f, 0.0f},
                {0.0f, 1.0f},
                {1.0f, 1.0f},
                {1.0f, 0.0f}
            }}
        );

    // Bottom face (Y = -hy)
    addFace(
        {{
            {-hx, -hy, -hz},
            {hx, -hy, -hz},
            {hx, -hy, hz},
            {-hx, -hy, hz},
        }},
        {0.0f, -1.0f, 0.0f},
        {
            {
                {0.0f, 0.0f},
                {1.0f, 0.0f},
                {1.0f, 1.0f},
                {0.0f, 1.0f}
            }}
        );

    // Top face (Y = +hy)
    addFace(
        {{
            {-hx, hy, -hz},
            {-hx, hy, hz},
            {hx, hy, hz},
            {hx, hy, -hz},
        }},
        {0.0f, 1.0f, 0.0f},
        {
            {
                {0.0f, 1.0f},
                {0.0f, 0.0f},
                {1.0f, 0.0f},
                {1.0f, 1.0f}
            }}
        );

    return mesh;
}

MeshData makeCylinder(const float radius, const float height, int sectorCount, int stackCount) {
    MeshData mesh;
    mesh.name = "cylinder";

    sectorCount = std::max(sectorCount, 3);
    stackCount = std::max(stackCount, 1);

    const float safeRadius = std::max(radius, 0.0f);
    const float safeHeight = std::max(height, 0.0f);
    const float halfHeight = safeHeight * 0.5f;
    const float angleStep = 2.0f * std::numbers::pi_v<float> / static_cast<float>(sectorCount);

    // side surface
    for (int i = 0; i <= stackCount; ++i) {
        const float y = -halfHeight + safeHeight * static_cast<float>(i) / static_cast<float>(stackCount);
        const float v = 1.0f - static_cast<float>(i) / static_cast<float>(stackCount);
        for (int j = 0; j <= sectorCount; ++j) {
            const float theta = angleStep * static_cast<float>(j);
            const float cosine = std::cos(theta);
            const float sine = std::sin(theta);
            mesh.vertices.push_back(Vertex{
                .position = {safeRadius * cosine, y, safeRadius * sine},
                .normal = {cosine, 0.0f, sine},
                .texCoord = {static_cast<float>(j) / static_cast<float>(sectorCount), v},
            });
        }
    }

    for (int i = 0; i < stackCount; ++i) {
        const auto k1 = static_cast<Index>(i * (sectorCount + 1));
        const auto k2 = k1 + sectorCount + 1;
        for (int j = 0; j < sectorCount; ++j) {
            const auto current = static_cast<Index>(j);
            const auto next = static_cast<Index>(j + 1);
            addTriangle(mesh.indices, k1 + current, k2 + current, k1 + next);
            addTriangle(mesh.indices, k2 + current, k2 + next, k1 + next);
        }
    }

    // caps. The ring vertices are duplicated so the caps get flat normals
    for (int cap = 0; cap < 2; ++cap) {
        const float y = cap == 0 ? -halfHeight : halfHeight;
        const float normalY = cap == 0 ? -1.0f : 1.0f;

        const auto center = static_cast<Index>(mesh.vertices.size());
        mesh.vertices.push_back(Vertex{
            .position = {0.0f, y, 0.0f},
            .normal = {0.0f, normalY, 0.0f},
            .texCoord = {0.5f, 0.5f}
        });

        const auto ringStart = static_cast<Index>(mesh.vertices.size());
        for (int j = 0; j < sectorCount; ++j) {
            const float theta = angleStep * static_cast<float>(j);
            const float cosine = std::cos(theta);
            const float sine = std::sin(theta);
            mesh.vertices.push_back(Vertex{
                .position = {safeRadius * cosine, y, safeRadius * sine},
                .normal = {0.0f, normalY, 0.0f},
                .texCoord = {-cosine * 0.5f + 0.5f, -sine * 0.5f + 0.5f},
            });
        }

        for (int j = 0; j < sectorCount; ++j) {
            const auto current = static_cast<Index>(j);
            const auto next = static_cast<Index>((j + 1) % sectorCount);
            if (cap == 0) {
                addTriangle(mesh.indices, center, ringStart + current, ringStart + next);
            } else {
                addTriangle(mesh.indices, center, ringStart + next, ringStart + current);
            }
        }
    }

    return mesh;
}

MeshData makeSphere(const float radius, int sectorCount, int stackCount) {
    MeshData mesh;
    mesh.name = "sphere";

    sectorCount = std::max(sectorCount, 3);
    stackCount = std::max(stackCount, 2);

    const float safeRadius = std::max(radius, 0.0f);
    const float sectorStep = 2.0f * std::numbers::pi_v<float> / static_cast<float>(sectorCount);
    const float stackStep = std::numbers::pi_v<float> / static_cast<float>(stackCount);

    for (int i = 0; i <= stackCount; ++i) {
        const float phi = stackStep * static_cast<float>(i);
        const float y = std::cos(phi);
        const float ringRadius = std::sin(phi);

        for (int j = 0; j <= sectorCount; ++j) {
            const float theta = sectorStep * static_cast<float>(j);
            const cadm::Vec3 normal{
                ringRadius * std::cos(theta),
                y,
                ringRadius * std::sin(theta),
            };

            mesh.vertices.push_back(Vertex{
                .position = normal * safeRadius,
                .normal = normal,
                .texCoord = {
                    static_cast<float>(j) / static_cast<float>(sectorCount),
                    1.0f - static_cast<float>(i) / static_cast<float>(stackCount),
                },
            });
        }
    }

    for (int i = 0; i < stackCount; ++i) {
        const auto k1 = static_cast<Index>(i * (sectorCount + 1));
        const auto k2 = k1 + sectorCount + 1;
        for (int j = 0; j < sectorCount; ++j) {
            const auto current = static_cast<Index>(j);
            const auto next = static_cast<Index>(j + 1);
            addTriangle(mesh.indices, k1 + current, k1 + next, k2 + current);
            addTriangle(mesh.indices, k1 + next, k2 + next, k2 + current);
        }
    }

    return mesh;
}

MeshData makeSpring(const SpringParameters& parameters) {
    MeshData mesh;
    mesh.name = "spring";

    const float height = std::max(parameters.height, 1e-4f);
    const float length = std::max(parameters.length, height + 1e-4f);
    const float radius = std::max(parameters.radius, 1e-4f);
    const float wireRadius = std::max(parameters.wireRadius, 1e-4f);
    const int along = std::max(parameters.segmentsAlong, 2);
    const int around = std::max(parameters.segmentsAround, 3);

    // Same parametrization as gk2's springVS.hlsl:
    //   a = sqrt(length^2 - height^2) / radius
    //   center(s) = (radius cos(s a), s height, radius sin(s a))
    const float angleRate = std::sqrt(length * length - height * height) / radius;
    const float aroundStep = 2.0f * std::numbers::pi_v<float> / static_cast<float>(around);

    mesh.vertices.reserve(static_cast<std::size_t>(along + 1) * (around + 1));
    mesh.indices.reserve(static_cast<std::size_t>(along) * around * 6);

    for (int i = 0; i <= along; ++i) {
        const float s = static_cast<float>(i) / static_cast<float>(along);
        const float theta = s * angleRate;
        const float cosine = std::cos(theta);
        const float sine = std::sin(theta);

        const cadm::Vec3 center{radius * cosine, s * height, radius * sine};
        const cadm::Vec3 tangent = cadm::Vec3{
            -radius * angleRate * sine,
            height,
            radius * angleRate * cosine,
        }.normalized();
        const cadm::Vec3 curveNormal{-cosine, 0.0f, -sine};
        const cadm::Vec3 curveBinormal = curveNormal.cross(tangent).normalized();

        for (int j = 0; j <= around; ++j) {
            const float phi = aroundStep * static_cast<float>(j);
            const cadm::Vec3 normal =
                curveNormal * std::cos(phi) + curveBinormal * std::sin(phi);

            mesh.vertices.push_back(Vertex{
                .position = center + normal * wireRadius,
                .normal = normal,
                .texCoord = {s * 6.8f, static_cast<float>(j) / static_cast<float>(around) * 0.2f},
            });
        }
    }

    for (int i = 0; i < along; ++i) {
        const auto k1 = static_cast<Index>(i * (around + 1));
        const auto k2 = k1 + around + 1;
        for (int j = 0; j < around; ++j) {
            const auto current = static_cast<Index>(j);
            const auto next = static_cast<Index>(j + 1);
            addTriangle(mesh.indices, k1 + current, k2 + current, k1 + next);
            addTriangle(mesh.indices, k2 + current, k2 + next, k1 + next);
        }
    }

    return mesh;
}

} // namespace cadg