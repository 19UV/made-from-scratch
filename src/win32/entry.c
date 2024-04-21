#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "win32/window.h"

int main(int argc, const char* argv[]) {
	(void) argc, (void) argv;

	const HINSTANCE instance = GetModuleHandle(NULL);
	if (instance != NULL) {
		Win32Window window;

		if (create_win32_window(&window, instance, "Game")) {
			ShowWindow(window.handle, 1);

			while (!window.should_close) {
				win32_window_poll_events(&window);

				puts("Frame");

				Sleep(100);
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

