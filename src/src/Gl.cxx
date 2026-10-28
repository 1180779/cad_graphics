/*
 * Created by Radosław Głasek on 22.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#include "cad_graphics/Gl.hxx"

#include <QDebug>
#include <QOpenGLContext>
#include <QOpenGLVersionFunctionsFactory>

namespace cadg {

void glSetDefaults() {
    QSurfaceFormat format;
    format.setVersion(4, 5);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    QSurfaceFormat::setDefaultFormat(format);
}

Gl* getGl() {
    auto* context = QOpenGLContext::currentContext();
    if (!context) {
        qWarning() << "getGl() called without a current QOpenGLContext!";
        return nullptr;
    }

    auto* gl45 = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_4_5_Core>(context);
    if (!gl45) {
        qWarning() << "OpenGL 4.5 Core not supported";
        return nullptr;
    }

    if (!gl45->initializeOpenGLFunctions()) {
        qWarning() << "Failed to initialize OpenGL functions";
        return nullptr;
    }

    return gl45;
}

void GlStateSnapshot::save(QOpenGLFunctions_4_5_Core* gl) {
    oldScissor = gl->glIsEnabled(GL_SCISSOR_TEST);
    oldBlend = gl->glIsEnabled(GL_BLEND);
    oldDepth = gl->glIsEnabled(GL_DEPTH_TEST);
    oldCull = gl->glIsEnabled(GL_CULL_FACE);

    gl->glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &oldDrawFbo);
    gl->glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &oldReadFbo);
    gl->glGetIntegerv(GL_VIEWPORT, oldViewport);
    gl->glGetIntegerv(GL_SCISSOR_BOX, oldScissorBox);
    gl->glGetIntegerv(GL_CURRENT_PROGRAM, &oldProgram);
    gl->glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &oldVao);
    gl->glGetIntegerv(GL_BLEND_EQUATION_RGB, &oldBlendEquation);
    gl->glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &oldBlendEquationAlpha);
    gl->glGetIntegerv(GL_BLEND_SRC_RGB, &oldBlendSrc);
    gl->glGetIntegerv(GL_BLEND_DST_RGB, &oldBlendDst);
    gl->glGetIntegerv(GL_BLEND_SRC_ALPHA, &oldBlendSrcAlpha);
    gl->glGetIntegerv(GL_BLEND_DST_ALPHA, &oldBlendDstAlpha);
}

void GlStateSnapshot::restore(QOpenGLFunctions_4_5_Core* gl) const {
    gl->glUseProgram(static_cast<GLuint>(oldProgram));
    gl->glBindVertexArray(static_cast<GLuint>(oldVao));
    gl->glBlendEquationSeparate(
        static_cast<GLenum>(oldBlendEquation),
        static_cast<GLenum>(oldBlendEquationAlpha)
    );
    gl->glBlendFuncSeparate(
        static_cast<GLenum>(oldBlendSrc),
        static_cast<GLenum>(oldBlendDst),
        static_cast<GLenum>(oldBlendSrcAlpha),
        static_cast<GLenum>(oldBlendDstAlpha)
    );
    restoreCap(gl, GL_BLEND, oldBlend);
    restoreCap(gl, GL_DEPTH_TEST, oldDepth);
    restoreCap(gl, GL_SCISSOR_TEST, oldScissor);
    gl->glScissor(oldScissorBox[0], oldScissorBox[1], oldScissorBox[2], oldScissorBox[3]);
    restoreCap(gl, GL_SCISSOR_TEST, oldScissor);
    gl->glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);
    gl->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, static_cast<GLuint>(oldDrawFbo));
    gl->glBindFramebuffer(GL_READ_FRAMEBUFFER, static_cast<GLuint>(oldReadFbo));
}

GlStateSnapshot GlStateSnapshot::create(QOpenGLFunctions_4_5_Core* gl) {
    GlStateSnapshot snapshot;
    snapshot.save(gl);
    return snapshot;
}

void GlStateSnapshot::restoreCap(QOpenGLFunctions_4_5_Core* gl, GLenum cap, GLboolean enable) {
    if (enable != GL_FALSE) {
        gl->glEnable(cap);
    } else {
        gl->glDisable(cap);
    }
}

} // namespace cadg