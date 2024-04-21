#ifndef INCLUDED_WIN32_WINDOW_H
#define INCLUDED_WIN32_WINDOW_H

#include <stdbool.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

typedef struct {
	HWND handle;

	bool should_close;
} Win32Window;

bool create_win32_window(Win32Window* window, HINSTANCE instance, const char* title);
void destroy_win32_window(Win32Window* window);

bool win32_window_poll_events(Win32Window* window);

#endif /* INCLUDED_WIN32_WINDOW_H */

