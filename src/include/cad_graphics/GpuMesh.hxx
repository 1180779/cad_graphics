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
#include "VertexLayout.hxx"

namespace cadg {

/// @brief Owns the VAO/VBO/EBO for one indexed mesh
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
    static std::expected<GpuMesh, std::string> upload(Gl& gl, const MeshView& mesh);

    /// @brief Draw the mesh
    /// @tparam Primitive primitive mode used by glDrawElements
    template <GLenum Primitive = GL_TRIANGLES>
    void draw(Gl& gl) const {
        if (m_vao == 0 || m_indexCount == 0) {
            return;
        }
        gl.glBindVertexArray(m_vao);
        gl.glDrawElements(Primitive, m_indexCount, GL_UNSIGNED_INT, nullptr);
        gl.glBindVertexArray(0);
    }

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