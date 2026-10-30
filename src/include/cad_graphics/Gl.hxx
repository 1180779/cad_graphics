/*
 * Created by Radosław Głasek on 22.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#ifndef CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_GL_HXX
#define CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_GL_HXX

#include <QOpenGLFunctions_4_5_Core>

namespace cadg {

using Gl = QOpenGLFunctions_4_5_Core;

/// @brief Install the 4.5 Core default QSurfaceFormat
void glSetDefaults();

/// @brief Return the 4.5 Core functions for the current Qt context
/// @returns nullptr when there is no current context or 4.5 is unavailable
Gl* getGl();

struct GlStateSnapshot {
    GLint oldDrawFbo = 0;
    GLint oldReadFbo = 0;
    GLint oldViewport[4]{};
    GLint oldScissorBox[4]{};
    GLint oldProgram = 0;
    GLint oldVao = 0;
    GLint oldBlendEquation = GL_FUNC_ADD;
    GLint oldBlendEquationAlpha = GL_FUNC_ADD;
    GLint oldBlendSrc = GL_ONE;
    GLint oldBlendDst = GL_ZERO;
    GLint oldBlendSrcAlpha = GL_ONE;
    GLint oldBlendDstAlpha = GL_ZERO;
    GLboolean oldScissor = false;
    GLboolean oldBlend = false;
    GLboolean oldDepth = false;
    GLboolean oldStencil = false;
    GLboolean oldCull = false;
    GLboolean oldDepthMask = GL_TRUE;
    GLint oldDepthFunc = GL_LESS;
    GLfloat oldLineWidth = 1.0f;

    void save(QOpenGLFunctions_4_5_Core* gl);
    void restore(QOpenGLFunctions_4_5_Core* gl) const;
    static GlStateSnapshot create(QOpenGLFunctions_4_5_Core* gl);

private:
    static void restoreCap(QOpenGLFunctions_4_5_Core* gl, GLenum cap, GLboolean enable);
};


} // namespace cadg

#endif // CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_GL_HXX
