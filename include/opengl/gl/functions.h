#ifndef INCLUDED_OPENGL_GL_FUNCTIONS_H
#define INCLUDED_OPENGL_GL_FUNCTIONS_H

#include "opengl/gl/types.h"

#ifndef GLEXPORT
#	define GLEXPORT extern
#endif

#define glGetString opengl_glGetString
typedef const GLubyte* (* PFNGLGETSTRINGPROC)(GLenum);
GLEXPORT PFNGLGETSTRINGPROC opengl_glGetString;

#endif /* INCLUDED_OPENGL_GL_FUNCTIONS_H */

