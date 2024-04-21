#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "win32/window.h"
#include "win32/renderers/opengl33.h"

#include "opengl/gl/enums.h"
#include "opengl/gl/functions.h"

int main(int argc, const char* argv[]) {
	(void) argc, (void) argv;

	const HINSTANCE instance = GetModuleHandle(NULL);
	if (instance != NULL) {
		Win32Window window;

		if (create_win32_window(&window, instance, "Game")) {
			Win32OpenGL33Renderer renderer;

			if (create_win32_opengl33_renderer(&renderer, &window)) {
				ShowWindow(window.handle, 1);

				GLfloat color = 0.0f;
				GLfloat delta = 0.05f;

				while (!window.should_close) {
					glClearColor(color, color * (1.0f - color), 1.0f - color, 1.0f);
					glClear(GL_COLOR_BUFFER_BIT);

					win32_window_poll_events(&window);

					color += delta;
					if (color < 0.0f || color > 1.0f) {
						delta = -delta;

						color += delta;
					}

					if (!SwapBuffers(renderer.device_context)) {
						fputs("[ERROR] Failed to swap buffers.\n", stderr);

						window.should_close = true;
					}

#ifndef NDEBUG
					puts("Frame");
					Sleep(100);
#endif
				}

				destroy_win32_opengl33_renderer(&renderer);
			} else {
				fputs("[ERROR] Failed to create OpenGL 3.3 renderer.\n", stderr);
			}

			destroy_win32_window(&window);
		} else {
			fputs("[ERROR] Failed to create window.\n", stderr);
		}
	} else {
		fputs("[ERROR] Failed to get instance.\n", stderr);
	}

	return 0;
}

