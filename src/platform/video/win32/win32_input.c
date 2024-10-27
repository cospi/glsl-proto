#include "win32_input.h"

Keycode win32_key_to_keycode(WPARAM key)
{
	switch (key) {
	case 0x42:
		return KEYCODE_B;
	case 0x43:
		return KEYCODE_C;
	case 0x52:
		return KEYCODE_R;
	default:
		return KEYCODE_UNKNOWN;
	}
}
