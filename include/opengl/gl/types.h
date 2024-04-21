#ifndef INCLUDED_OPENGL_GL_TYPES_H
#define INCLUDED_OPENGL_GL_TYPES_H

#include <stdint.h>

/* REFERENCE: https://www.khronos.org/opengl/wiki/OpenGL_Type */

typedef uint8_t GLboolean;

typedef int8_t   GLbyte;
typedef uint8_t  GLubyte;
typedef char     GLchar; /* Not on reference table. */

typedef int16_t  GLshort;
typedef uint16_t GLsuhort;

typedef int32_t  GLint;
typedef uint32_t GLuint;
/* TODO: GLfixed */

typedef int64_t  GLint64;
typedef uint64_t GLuint64;

/* TODO: GLintptr */
/* TODO: GLsizeiptr */
/* TODO: GLsync */
typedef uint32_t GLbitfield;
/* TODO: GLhalf */

typedef uint32_t GLsizei;
typedef uint32_t GLenum;

typedef float    GLfloat;
typedef GLfloat  GLclampf;
typedef double   GLdouble;
typedef GLdouble GLclampd;

#endif /* INCLUDED_OPENGL_GL_TYPES_H */

