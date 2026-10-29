/*
 * Created by Radosław Głasek on 28.10.2026
 * radoslaw.glasek@theworldlooksback.dev
 */

#ifndef CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_CHECKS_HXX
#define CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_CHECKS_HXX

#include "Gl.hxx"

#include <QtDebug>

namespace cadg {

inline const char* GLErrorToString(const GLenum error) {
    switch (error) {
    case GL_INVALID_ENUM:
        return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "GL_INVALID_FRAMEBUFFER_OPERATION";
    case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY";
    default:
        return "UNKNOWN_GL_ERROR";
    }
}

inline bool logGlErrorsIfAny(const char *file, const int line) {
    bool hasError = false;
    const auto gl = getGl();
    if (!gl) {
        return false;
    }

    for (GLenum error = gl->glGetError(); error != GL_NO_ERROR; error = gl->glGetError()) {
        hasError = true;
        qWarning() << "OpenGL error" << GLErrorToString(error) << "(" << static_cast<unsigned int>(error) << ") at"
            << file << "," << line;
    }

    return hasError;
}

#define UNIQUE_PTR_RELEASE_CHECK(expr) \
    do { \
        if(!expr) { \
            qWarning() << "Unique ptr release failed" << __FILE__ << ", " << __LINE__; \
        } \
    } while(0)

#define GET_GL_ERRORS() \
    do { \
        ::cadg::logGlErrorsIfAny(__FILE__, __LINE__); \
    } while(0)

#define SHADER_SET_UNIFORM_CHECK(expr) \
    do { \
        if(!expr) { \
            qWarning() << "Shader set uniform failed" << __FILE__ << ", " << __LINE__; \
            GET_GL_ERRORS(); \
        } \
    } while (0)

#define SHADER_ATTACHING_CHECK(expr) \
    do { \
        if (!expr) { \
            qWarning() << "Shader compilation failed: " << __FILE__ << ", " << __LINE__; \
            GET_GL_ERRORS(); \
        } \
    } while (0)

#define SHADER_COMPILATION_CHECK(expr) \
    do { \
        if (!expr) { \
            qWarning() << "Shader compilation failed: " << __FILE__ << ", " << __LINE__; \
            GET_GL_ERRORS(); \
        } \
    } while (0)

#define SHADER_COMPILATION_CHECK(expr) \
    do { \
        if (!expr) { \
            qWarning() << "Shader compilation failed: " << __FILE__ << ", " << __LINE__; \
            GET_GL_ERRORS(); \
        } \
    } while (0)

#define EXPECTED_COMPONENT_MISSING() \
    do { \
            qWarning() << "Expected component missing: " << __FILE__ << ", " << __LINE__; \
    } while (0)

} // namespace cadg

#endif // CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_CHECKS_HXX
