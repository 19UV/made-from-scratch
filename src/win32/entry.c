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
				printf("Window: %p\n", renderer.window_handle);
				printf("Device Context: %p\n", renderer.device_context);
				printf("OpenGL Context: %p\n", renderer.opengl_context);
				printf("Version: %s\n", glGetString(GL_VERSION));

				ShowWindow(window.handle, 1);

				while (!window.should_close) {
					win32_window_poll_events(&window);

					puts("Frame");

					Sleep(100);
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

