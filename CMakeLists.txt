cmake_minimum_required(VERSION 3.10)

project(Game
	LANGUAGES C
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(FILES ${FILES}
	src/win32/window.c

	src/win32/entry.c
)

add_executable(game ${FILES})
target_include_directories(game PRIVATE include/)

