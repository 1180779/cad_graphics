/*
 * Created by Radosław Głasek on 22.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#ifndef CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_GPU_MATERIAL_HXX
#define CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_GPU_MATERIAL_HXX

#include "Gl.hxx"
#include "GpuTexture.hxx"
#include "Material.hxx"
#include "ShaderProgram.hxx"

#include <expected>

namespace cadg {

/// @brief GPU-side Phong material
///
/// Standard uniform contract: <br/> 
///   <tt>uDiffuseColor</tt> (<tt>vec3</tt>), <br/> 
///   <tt>uSpecularColor</tt> (<tt>vec3</tt>), <br/>  
///   <tt>uShininess</tt> (<tt>float</tt>), <br/> 
///   <tt>uOpacity</tt> (<tt>float</tt>),  <br/> 
///   <tt>uHasDiffuseMap</tt> (<tt>int</tt>),  <br/> 
///   <tt>uDiffuseMap</tt> (<tt>sampler2D</tt>, unit 0)
///
/// @pre <tt>bind()</tt> assumes the target ShaderProgram is already bound
class GpuMaterial final {
public:
    GpuMaterial() = default;
    ~GpuMaterial();

    GpuMaterial(const GpuMaterial&) = delete;
    GpuMaterial& operator=(const GpuMaterial&) = delete;

    GpuMaterial(GpuMaterial&& other) noexcept = default;
    GpuMaterial& operator=(GpuMaterial&& other) noexcept = default;

    static std::expected<GpuMaterial, std::string> upload(Gl& gl, const Material& material);

    void bind(Gl& gl, const ShaderProgram& shader) const;

    void destroy(Gl& gl);

    [[nodiscard]] const Material& data() const {
        return m_material;
    }

    [[nodiscard]] bool doubleSided() const {
        return m_material.doubleSided;
    }

private:
    Material m_material;
    GpuTexture m_diffuse{};
};

} // namespace cadg


#endif // CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_GPU_MATERIAL_HXX