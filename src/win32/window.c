#include "win32/window.h"

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

static const LPCSTR window_prop_name = "InternalWindowPointer";

static LPCSTR window_class_name = NULL;

static LRESULT window_procedure(HWND handle, UINT message, WPARAM w_param, LPARAM l_param) {
	LRESULT result;

	Win32Window* window = GetPropA(handle, window_prop_name);

	if (window == NULL) {
		result = DefWindowProcA(handle, message, w_param, l_param);
	} else {
		switch (message) {
			case WM_CLOSE:
				window->should_close = true;

				result = 0;
				break;

			default:
				result = DefWindowProcA(handle, message, w_param, l_param);
				break;
		}
	}

	return result;
}

bool create_win32_window(Win32Window* window, HINSTANCE instance, const char* title) {
	assert(window != NULL);
	assert(title != NULL);

	const bool created_window_class = window_class_name == NULL;

	if (window_class_name == NULL) {
		const WNDCLASSA window_class = {
			.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW,
			.lpfnWndProc = &window_procedure,
			.cbClsExtra = 0,
			.cbWndExtra = 0,
			.hInstance = instance,
			.hIcon = NULL,
			.hCursor = NULL,
			.hbrBackground = NULL,
			.lpszMenuName = NULL,
			.lpszClassName = "GameWindowClass"
		};

		if (RegisterClassA(&window_class) != 0) {
			window_class_name = window_class.lpszClassName;
		} else {
			fputs("[ERROR] Failed to register win32 window class.\n", stderr);

			return false;
		}
	}

	assert(window_class_name != NULL);

	const HWND handle = CreateWindowA(
		window_class_name,            /* Window Class */
		title,                        /* Title */
		WS_OVERLAPPEDWINDOW,          /* Style */
		CW_USEDEFAULT, CW_USEDEFAULT, /* X, Y */
		CW_USEDEFAULT, CW_USEDEFAULT, /* Width, Height */
		NULL,                         /* Parent */
		NULL,                         /* Menu */
		instance,                     /* Instance*/
		NULL                          /* Additional Parameter*/
	);

	if (handle != NULL) {
		if (SetPropA(handle, window_prop_name, window)) {
			window->handle = handle;
			window->should_close = false;

			return true;
		} else {
			fputs("[ERROR] Failed to set win32 window prop.\n", stderr);
		}

		if (!DestroyWindow(handle)) {
			fputs("[ERROR] Failed to destroy win32 window.\n", stderr);
		}
	} else {
		fputs("[ERROR] Failed to create win32 window.\n", stderr);
	}

	if (created_window_class) {
		if (!UnregisterClassA(window_class_name, instance)) {
			fputs("[ERROR] Failed to unregister win32 window class.\n", stderr);
		} else {
			window_class_name = NULL;
		}
	}

	return false;
}

void destroy_win32_window(Win32Window* window) {
	assert(window != NULL);
	assert(window->handle != NULL);

	if (!DestroyWindow(window->handle)) {
		fputs("[ERROR] Failed to destroy win32 window.\n", stderr);
	}

#ifndef NDEBUG
	window->handle = NULL;
#endif
}

bool win32_window_poll_events(Win32Window* window) {
	assert(window != NULL);
	assert(window->handle != NULL);

	MSG message;
	while (
		!window->should_close &&
		PeekMessageA(&message, window->handle, 0, 0, PM_REMOVE)
	) {
		if (message.message == WM_QUIT) {
			window->should_close = true;
		}

		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	return !window->should_close;
}

