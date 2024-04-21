#include "win32/renderers/opengl33.h"

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "opengl/gl/types.h"
#include "opengl/gl/enums.h"
#include "opengl/gl/functions.h"

#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092

#define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126
#define WGL_CONTEXT_FLAGS_ARB                   0x2094

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB        0x00000001
#define WGL_CONTEXT_DEBUG_BIT_ARB               0x0001

/* TODO: Mark these WINAPI calls. */

#define wglCreateContext opengl32_wglCreateContext
typedef HGLRC (* PFNWGLCREATECONTEXTPROC)(HDC);
static PFNWGLCREATECONTEXTPROC opengl32_wglCreateContext = NULL;

#define wglDeleteContext opengl32_wglDeleteContext
typedef BOOL (* PFNWGLDELETECONTEXTPROC)(HGLRC);
static PFNWGLDELETECONTEXTPROC opengl32_wglDeleteContext = NULL;

#define wglMakeCurrent opengl32_wglMakeCurrent
typedef BOOL (* PFNWGLMAKECURRENTPROC)(HDC, HGLRC);
static PFNWGLMAKECURRENTPROC opengl32_wglMakeCurrent = NULL;

#define wglGetProcAddress opengl32_wglGetProcAddress
typedef PROC (* PFNWGLGETPROCADDRESSPROC)(LPCSTR);
static PFNWGLGETPROCADDRESSPROC opengl32_wglGetProcAddress = NULL;

#define wglGetExtensionsStringEXT opengl32_wglGetExtensionsStringEXT
typedef const char* (* PFNWGLGETEXTENSIONSSTRINGEXTPROC)(void);
static PFNWGLGETEXTENSIONSSTRINGEXTPROC opengl32_wglGetExtensionsStringEXT = NULL;

#define wglCreateContextAttribsARB opengl32_wglCreateContextAttribsARB
typedef HGLRC (* PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int*);
static PFNWGLCREATECONTEXTATTRIBSARBPROC opengl32_wglCreateContextAttribsARB = NULL;

static HMODULE lib_opengl32 = NULL;

static bool load_opengl32(void) {
	if (lib_opengl32 != NULL) {
		fputs("[WARN] Attempting to load opengl32 when already loaded.\n", stderr);

		return true;
	}

	lib_opengl32 = LoadLibraryA("opengl32.dll");

	if (lib_opengl32 != NULL) {
#define LOAD_WGL_FUNCTION(name, type) \
	opengl32_##name = (type) GetProcAddress(lib_opengl32, #name); \
	if (opengl32_##name == NULL) { \
		fputs("[ERROR] Failed to load wgl function '" #name "'.\n", stderr); \
		goto load_wgl_function_failed; \
	}

		LOAD_WGL_FUNCTION(wglCreateContext, PFNWGLCREATECONTEXTPROC);
		LOAD_WGL_FUNCTION(wglDeleteContext, PFNWGLDELETECONTEXTPROC);
		LOAD_WGL_FUNCTION(wglMakeCurrent, PFNWGLMAKECURRENTPROC);
		LOAD_WGL_FUNCTION(wglGetProcAddress, PFNWGLGETPROCADDRESSPROC);

#undef LOAD_WGL_FUNCTION
		return true;

load_wgl_function_failed:
		if (!FreeLibrary(lib_opengl32)) {
			fputs("[ERROR] Failed to free opengl32.\n", stderr);
		}
	} else {
		fputs("[ERROR] Failed to load opengl32.\n", stderr);
	}


	return false;
}

static void load_wgl_extensions(void) {
	assert(lib_opengl32 != NULL);
	assert(wglGetProcAddress != NULL);

	wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC) wglGetProcAddress("wglGetExtensionsStringEXT");
	if (wglGetExtensionsStringEXT != NULL) {
		const char* extensions = wglGetExtensionsStringEXT();

		if (extensions != NULL) {
#define LOAD_WGL_EXTENSION(name, type, extension) \
	opengl32_##name = (type) wglGetProcAddress(#name); \
	if (opengl32_##name == NULL) { \
		fputs("[WARN] Failed to load wgl function '" #name "' from '" #extension "'.\n", stderr); \
	}

#if 0
			fprintf(stderr, "[INFO] WGL Extensions: %s\n", extensions);
#endif

			if (
				strstr(extensions, "WGL_ARB_create_context") &&
				strstr(extensions, "WGL_ARB_create_context_profile")
			) {
				LOAD_WGL_EXTENSION(
					wglCreateContextAttribsARB,
					PFNWGLCREATECONTEXTATTRIBSARBPROC,
					WGL_ARB_create_context
				);
			}

#undef LOAD_WGL_EXTENSION
		} else {
			fputs("[ERROR] Invalid wgl extensions string.\n", stderr);
		}
	} else {
		fputs("[ERROR] EXT_extensions_string not supported.\n", stderr);
	}
}

static bool check_opengl_version(const char* string, unsigned int major, unsigned int minor) {
	unsigned int context_major, context_minor;
	sscanf(string, "%u.%u", &context_major, &context_minor);

	return (
		context_major > major ||
		(context_major == major && context_minor >= minor)
	);
}

static bool load_opengl_functions(void) {
	/* NOTE: Because glGetString is weird, and from opengl32.dll */
	opengl_glGetString = (PFNGLGETSTRINGPROC) GetProcAddress(lib_opengl32, "glGetString");
	if (opengl_glGetString == NULL) {
		fputs("[ERROR] Failed to load opengl function 'glGetString'.\n", stderr);
		return false;
	}

	return true;
}

bool create_win32_opengl33_renderer(Win32OpenGL33Renderer* renderer, Win32Window* window) {
	const bool loaded_opengl32 = lib_opengl32 == NULL;

	if (lib_opengl32 != NULL || load_opengl32()) {
		const HDC device_context = GetDC(window->handle);

		if (device_context != NULL) {
			const PIXELFORMATDESCRIPTOR pixel_format_descriptor = {
				.nSize = sizeof(pixel_format_descriptor),
				.nVersion = 1,

				.dwFlags = PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL,

				.iPixelType = PFD_TYPE_RGBA,

				.cColorBits = 24,
				.cAlphaBits = 8,
				.cAccumBits = 0,
				.cDepthBits = 32,
				.cStencilBits = 0,

				.cAuxBuffers = 0,

				.iLayerType = PFD_MAIN_PLANE
			};

			const int pixel_format = ChoosePixelFormat(device_context, &pixel_format_descriptor);
			if (pixel_format > 0 && SetPixelFormat(device_context, pixel_format, &pixel_format_descriptor)) {
				HGLRC dummy_opengl_context = wglCreateContext(device_context);

				if (dummy_opengl_context != NULL) {
					if (wglMakeCurrent(device_context, dummy_opengl_context)) {
						load_wgl_extensions();

						HGLRC opengl_context = NULL;
						if (opengl32_wglCreateContextAttribsARB != NULL) {
							/* NOTE: We can create a core OpenGL 3.3 context. */

							const int attribute_list[] = {
								WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
								WGL_CONTEXT_MINOR_VERSION_ARB, 3,
								WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
								WGL_CONTEXT_FLAGS_ARB,
#ifndef NDEBUG
									WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#else
									0,
#endif
								0
							};

							opengl_context = wglCreateContextAttribsARB(device_context, NULL, attribute_list);

							/* TODO: If creation failed, print error code. */
						}

						if (opengl_context == NULL) {
							/* NOTE: We need to fallback on just using the dummy context. */
							/*       But first, we need to check the version. */

							opengl_glGetString = (PFNGLGETSTRINGPROC) GetProcAddress(lib_opengl32, "glGetString");
							if (opengl_glGetString != NULL) {
								const char* version = glGetString(GL_VERSION);

								if (version != NULL) {
									if (check_opengl_version(version, 3, 3)) {
										opengl_context = dummy_opengl_context;
									} else {
										fputs("[ERROR] Unsupported OpenGL version - minimum: 3.3.\n", stderr);
									}
								} else {
									fputs("[ERROR] Invalid OpenGL version string.\n", stderr);
								}
							} else {
								fputs("[ERROR] Failed to get OpenGL function 'glGetString'.\n", stderr);
							}
						} else {
							if (wglMakeCurrent(device_context, NULL)) {
								if (!wglDeleteContext(dummy_opengl_context)) {
									fputs("[ERROR] Failed to delete dummy opengl context.\n", stderr);
								}
							} else {
								fputs("[ERROR] Failed to unbind dummy opengl context.\n", stderr);
							}
						}

						if (opengl_context != NULL) {
							if (wglMakeCurrent(device_context, opengl_context)) {
								if (load_opengl_functions()) {
#ifndef NDEBUG
									fprintf(stderr, "[INFO] Driver Vendor:   %s\n", glGetString(GL_VENDOR));
									fprintf(stderr, "[INFO] Physical Device: %s\n", glGetString(GL_RENDERER));
									fprintf(stderr, "[INFO] OpenGL Version:  %s\n", glGetString(GL_VERSION));
#endif
									renderer->window_handle = window->handle;
									renderer->device_context = device_context;
									renderer->opengl_context = opengl_context;

									return true;
								} else {
									fputs("[ERROR] Failed to load opengl functions.\n", stderr);
								}
							} else {
								fputs("[ERROR] Failed to bind opengl context.\n", stderr);
							}

							if (wglMakeCurrent(device_context, NULL)) {
								if (!wglDeleteContext(opengl_context)) {
									fputs("[ERROR] Failed to delete opengl context.\n", stderr);
								}
							} else {
								fputs("[ERROR] Failed to unbind opengl context.\n", stderr);
							}
						}
					} else {
						fputs("[ERROR] Failed to bind opengl context.\n", stderr);
					}

					if (!wglDeleteContext(dummy_opengl_context)) {
						fputs("[ERROR] Failed to cleanup dummy opengl context.\n", stderr);
					}
				} else {
					fputs("[ERROR] Failed to create dummy opengl context.\n", stderr);
				}
			} else {
				fputs("[ERROR] Failed to set window pixel format.\n", stderr);
			}

			if (ReleaseDC(window->handle, device_context) == 0) {
				fputs("[ERROR] Failed to release device context.\n", stderr);
			}
		} else {
			fputs("[ERROR] Failed to get window device context.\n", stderr);
		}

		if (loaded_opengl32) {
			if (!FreeLibrary(lib_opengl32)) {
				fputs("[ERROR] Failed to free library opengl32.\n", stderr);
			} else {
				lib_opengl32 = NULL;
			}
		}
	} else {
		fputs("[ERROR] Failed to load wgl functions.\n", stderr);
	}

	return false;
}

void destroy_win32_opengl33_renderer(Win32OpenGL33Renderer* renderer) {
	assert(renderer != NULL);
	assert(renderer->window_handle != NULL);
	assert(renderer->device_context != NULL);
	assert(renderer->opengl_context != NULL);

	if (wglMakeCurrent(renderer->device_context, NULL)) {
		if (wglDeleteContext(renderer->opengl_context)) {
			if (ReleaseDC(renderer->window_handle, renderer->device_context)) {
			} else {
				fputs("[ERROR] Failed to release device context.\n", stderr);
			}
		} else {
			fputs("[ERROR] Failed to delete opengl context.\n", stderr);
		}
	} else {
		fputs("[ERROR] Failed to unbind opengl context.\n", stderr);
	}
}

