/*
 * Created by Radosław Głasek on 22.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#include "cad_graphics/GpuMaterial.hxx"

#include <cassert>
#include <utility>

namespace cadg {

GpuMaterial::~GpuMaterial() {
    assert(!m_diffuse.valid() && "GpuMaterial destroyed with a live texture; call destroy() first");
}

std::expected<GpuMaterial, std::string> GpuMaterial::upload(Gl& gl, const Material& material) {
    GpuMaterial result;
    result.m_material = material;

    if (material.diffuseMap) {
        auto texture = GpuTexture::upload(gl, *material.diffuseMap);
        if (!texture) {
            return std::unexpected("failed to upload diffuse texture: " + texture.error());
        }
        result.m_diffuse = std::move(*texture);
    }

    return result;
}

void GpuMaterial::bind(Gl& gl, const ShaderProgram& shader) const {
    shader.setVec3(gl, "uDiffuseColor", m_material.diffuseColor);
    shader.setVec3(gl, "uSpecularColor", m_material.specularColor);
    shader.setFloat(gl, "uShininess", m_material.shininess);
    shader.setFloat(gl, "uOpacity", m_material.opacity);
    shader.setInt(gl, "uHasDiffuseMap", m_diffuse.valid() ? 1 : 0);
    shader.setInt(gl, "uDiffuseMap", 0);

    if (m_diffuse.valid()) {
        m_diffuse.bind(gl, 0);
    }
    gl.glActiveTexture(GL_TEXTURE0);
}

void GpuMaterial::destroy(Gl& gl) {
    m_diffuse.destroy(gl);
}

} // namespace cadg