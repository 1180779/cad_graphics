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

} // namespace cadg