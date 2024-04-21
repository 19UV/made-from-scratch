#ifndef INCLUDED_OPENGL_GL_FUNCTIONS_H
#define INCLUDED_OPENGL_GL_FUNCTIONS_H

#include "opengl/gl/types.h"

#ifndef GLEXPORT
#	define GLEXPORT extern
#endif

#define glGetString opengl_glGetString
typedef const GLubyte* (* PFNGLGETSTRINGPROC)(GLenum);
GLEXPORT PFNGLGETSTRINGPROC opengl_glGetString;

#define glGetError opengl_glGetError
typedef GLenum (* PFNGLGETERRORPROC)(void);
GLEXPORT PFNGLGETERRORPROC opengl_glGetError;

/* -------------------------------------------------- */

#define glClearColor opengl_glClearColor
typedef void (* PFNGLCLEARCOLORPROC)(GLfloat, GLfloat, GLfloat, GLfloat);
GLEXPORT PFNGLCLEARCOLORPROC opengl_glClearColor;

#define glClear opengl_glClear
typedef void (* PFNGLCLEARPROC)(GLbitfield);
GLEXPORT PFNGLCLEARPROC opengl_glClear;

#endif /* INCLUDED_OPENGL_GL_FUNCTIONS_H */

