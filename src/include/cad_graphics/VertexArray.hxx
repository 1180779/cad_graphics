/*
 * Created by Radosław Głasek on 29.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#ifndef CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_VERTEX_ARRAY_HXX
#define CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_VERTEX_ARRAY_HXX

#include <expected>
#include <string>

#include "Gl.hxx"
#include "VertexLayout.hxx"

namespace cadg {

class VertexArray final {
public:
    VertexArray() = default;
    ~VertexArray();

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;
    VertexArray(VertexArray&& other) noexcept;
    VertexArray& operator=(VertexArray&& other) noexcept;

    /// @brief Configure vao to reference existing buffers 
    /// @note Does not preserve current VAO and array buffer bindings
    static std::expected<VertexArray, std::string> create(
        Gl& gl, GLuint vertexBuffer, const VertexLayout& layout, GLuint indexBuffer = 0
    );
    static std::expected<VertexArray, std::string> create(
        Gl* gl, GLuint vertexBuffer, const VertexLayout& layout, GLuint indexBuffer = 0
    );

    void bind(Gl& gl) const;
    void bind(Gl* gl) const;
    static void unbind(Gl& gl);
    static void unbind(Gl* gl);

    void destroy(Gl& gl);
    void destroy(Gl* gl);

    [[nodiscard]] bool valid() const { return m_vao != 0; }
    [[nodiscard]] GLuint id() const { return m_vao; }

private:
    GLuint m_vao = 0;
};

} // namespace cadg

#endif // CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_VERTEX_ARRAY_HXX