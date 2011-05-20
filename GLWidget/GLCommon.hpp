#ifndef QGLWIDGET_COMMON_HPP
#define QGLWIDGET_COMMON_HPP

#ifdef WIN32
#include <windows.h>								// Header File For Windows
#include <gl\gl.h>									// Header File For The OpenGL32 Library
#include <gl\glu.h>									// Header File For The GLu32 Library
#include <gl\glaux.h>								// Header File For The GLaux Library
#else
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else 
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#endif

#include "common.hpp"

namespace isis {
namespace viewer {
namespace GL {

bool checkAndReportGLError( std::string context, LogLevel level = error );

}}}

#endif