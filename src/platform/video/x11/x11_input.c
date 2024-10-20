#include "x11_input.h"

#include <X11/keysym.h>

#include <assert.h>

Keycode x11_key_event_to_keycode(XKeyEvent *key_event)
{
	assert(key_event != NULL);

	KeySym key_sym = XLookupKeysym(key_event, 0);
	switch (key_sym) {
	case XK_b:
		return KEYCODE_B;
	case XK_c:
		return KEYCODE_C;
	case XK_r:
		return KEYCODE_R;
	default:
		return KEYCODE_UNKNOWN;
	}
}
