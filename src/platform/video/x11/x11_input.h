#ifndef PLATFORM_VIDEO_X11_X11_INPUT_H_
#define PLATFORM_VIDEO_X11_X11_INPUT_H_

#include <X11/Xlib.h>

#include "../../../input/keycode.h"

Keycode x11_key_event_to_keycode(XKeyEvent *key_event);

#endif // PLATFORM_VIDEO_X11_X11_INPUT_H_
