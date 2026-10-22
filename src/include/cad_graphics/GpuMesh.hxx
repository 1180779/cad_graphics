/*
 * Created by Radosław Głasek on 22.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#ifndef CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_GPU_MESH_HXX
#define CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_GPU_MESH_HXX

#include <cstddef>
#include <expected>
#include <string>

#include "Gl.hxx"
#include "MeshData.hxx"

namespace cadg {

/// @brief Owns the VAO/VBO/EBO for one indexed triangle mesh
///
/// The destructor never issues GL calls. it only asserts that the handles were
/// released
class GpuMesh final {
public:
    GpuMesh() = default;
    ~GpuMesh();

    GpuMesh(const GpuMesh&) = delete;
    GpuMesh& operator=(const GpuMesh&) = delete;

    GpuMesh(GpuMesh&& other) noexcept;
    GpuMesh& operator=(GpuMesh&& other) noexcept;

    static std::expected<GpuMesh, std::string> upload(Gl& gl, const MeshData& mesh);

    void draw(Gl& gl) const;

    void destroy(Gl& gl);

    [[nodiscard]] bool valid() const {
        return m_vao != 0;
    }

    [[nodiscard]] GLsizei indexCount() const {
        return m_indexCount;
    }

    [[nodiscard]] GLuint vao() const {
        return m_vao;
    }

private:
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_ebo = 0;
    GLsizei m_indexCount = 0;
};

} // namespace cadg

#endif // CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_GPU_MESH_HXX