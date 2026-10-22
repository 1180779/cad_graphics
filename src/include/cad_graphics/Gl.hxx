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

} // namespace cadg

#endif // CAD_GRAPHICS_HEADER_SRC_INCLUDE_CAD_GRAPHICS_GL_HXX