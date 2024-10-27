#ifndef PLATFORM_VIDEO_WIN32_WIN32_INPUT_H_
#define PLATFORM_VIDEO_WIN32_WIN32_INPUT_H_

#include <Windows.h>

#include "../../../input/keycode.h"

Keycode win32_key_to_keycode(WPARAM key);

#endif // PLATFORM_VIDEO_WIN32_WIN32_INPUT_H_
