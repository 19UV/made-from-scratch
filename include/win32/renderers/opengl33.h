#ifndef INCLUDED_WIN32_RENDERERS_OPENGL33_H
#define INCLUDED_WIN32_RENDERERS_OPENGL33_H

#include <stdbool.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "win32/window.h"

typedef struct {
	HWND window_handle;
	HDC device_context;
	HGLRC opengl_context;
} Win32OpenGL33Renderer;

bool create_win32_opengl33_renderer(Win32OpenGL33Renderer* renderer, Win32Window* window);
void destroy_win32_opengl33_renderer(Win32OpenGL33Renderer* renderer);

#endif /* INCLUDED_WIN32_RENDERERS_OPENGL33_H */

