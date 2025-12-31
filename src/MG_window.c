#include "MG_window.h"

int MG_window_event_parse(MG_WindowData* window_data, SDL_WindowEvent window_event);
int MG_key_event_parse(MG_WindowData* window_data, SDL_Scancode key_code, bool toggle);
void MG_mouse_button_event_parse(MG_WindowData* window_data, SDL_MouseButtonEvent button_event, bool toggle);

// Main loop for the window and I/O events.
int MG_window_loop(void* MG_instance)
{
	// default exit info. If these values are not altered, the loop was ended by another thread.
	int exit_code = 0;
	const char* exit_type = "REQUESTED BY ANOTHER THREAD";
	if (!MG_instance)
	{
		exit_code = -1;
		printf("Event loop ended. Exit type: -1 \"INSTANCE NULL\"\n");
		return exit_code;
	}

	MG_Instance* instance = (MG_Instance*)MG_instance;
	SDL_Event event;

	while (instance->active)
	{
		// delay is used to prevent the event loop from hogging the CPU.
		SDL_Delay(1);
		while (SDL_PollEvent(&event))
		{
			//printf("event type: %d\n", event.type);
			switch (event.type)
			{
			case SDL_QUIT:
				instance->active = false;
				exit_code = 1;
				exit_type = "QUIT REQUESTED BY OS";
				break;

			case SDL_WINDOWEVENT:
				if (MG_window_event_parse(&instance->window_data, event.window))
				{
					instance->active = false;
					exit_code = 2;
					exit_type = "WINDOW CLOSED";
				}
				break;

			case SDL_KEYDOWN:
				MG_key_event_parse(&instance->window_data, event.key.keysym.scancode, true);
				break;
			case SDL_KEYUP:
				// check for window closing through ESC. is only done on key up
				// [TODO] this is for debugging, and should be replaced later with a proper exit menu.
				if (MG_key_event_parse(&instance->window_data, event.key.keysym.scancode, false))
				{
					instance->active = false;
					exit_code = 3;
					exit_type = "ESCAPE KEY";
				}
				break;

			case SDL_MOUSEMOTION:
				// mouse movement is cumulative until polled by another thread.
				instance->window_data.mouse_x_rel += event.motion.xrel;
				instance->window_data.mouse_y_rel += event.motion.yrel;
				// if the mouse is grabbed, warp it to the center of the window.
				if (instance->window_data.mouse_grabbed)
				{
					SDL_WarpMouseInWindow(instance->window, instance->window_data.width / 2, instance->window_data.height / 2);
				}
				else
				{
					instance->window_data.mouse_x = event.motion.x;
					instance->window_data.mouse_y = event.motion.y;
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				MG_mouse_button_event_parse(&instance->window_data, event.button, true);
				break;
			case SDL_MOUSEBUTTONUP:
				MG_mouse_button_event_parse(&instance->window_data, event.button, false);
				break;

			case SDL_MOUSEWHEEL:
				instance->window_data.mouse_wheel_rel += event.wheel.preciseY;
				break;

				// [TODO] add controller support
			case SDL_JOYAXISMOTION:
			case SDL_JOYBALLMOTION:
			case SDL_JOYHATMOTION:
			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP:
			case SDL_JOYDEVICEADDED:
			case SDL_JOYDEVICEREMOVED:
			case SDL_JOYBATTERYUPDATED:

			case SDL_CONTROLLERAXISMOTION:
			case SDL_CONTROLLERBUTTONDOWN:
			case SDL_CONTROLLERBUTTONUP:
			case SDL_CONTROLLERDEVICEADDED:
			case SDL_CONTROLLERDEVICEREMOVED:
			case SDL_CONTROLLERDEVICEREMAPPED:
			case SDL_CONTROLLERTOUCHPADDOWN:
			case SDL_CONTROLLERTOUCHPADMOTION:
			case SDL_CONTROLLERTOUCHPADUP:

				// intentionally unused
			case SDL_TEXTEDITING:
			case SDL_TEXTINPUT:
			case SDL_SYSWMEVENT:
			case SDL_DISPLAYEVENT:
				break;
			}
		}
	}

	printf("Event loop ended. Exit type: %d \"%s\"\n", exit_code, exit_type);
	return exit_code;
}

// Parses the SDL_WindowEvent and updates the MG_WindowData accordingly.
// Calls the respective callbacks if they are set.
static int MG_window_event_parse(MG_WindowData* window_data, SDL_WindowEvent window_event)
{
	if (!window_data) return -1;

	switch (window_event.event)
	{
	case SDL_WINDOWEVENT_CLOSE:
		return 1;

	case SDL_WINDOWEVENT_ENTER:
		window_data->mouse_above_window = true;
		if (window_data->callback_mouse_above_enter)
			window_data->callback_mouse_above_enter(window_data->instance);
		break;

	case SDL_WINDOWEVENT_LEAVE:
		window_data->mouse_above_window = false;
		if (window_data->callback_mouse_above_exit)
			window_data->callback_mouse_above_exit(window_data->instance);
		break;

	case SDL_WINDOWEVENT_FOCUS_GAINED:
		window_data->focused = true;
		if (window_data->callback_focus_gained)
			window_data->callback_focus_gained(window_data->instance);
		break;

	case SDL_WINDOWEVENT_FOCUS_LOST:
		window_data->focused = false;
		if (window_data->callback_focus_lost)
			window_data->callback_focus_lost(window_data->instance);
		break;

	case SDL_WINDOWEVENT_MOVED:
		window_data->x_pos = window_event.data1;
		window_data->y_pos = window_event.data2;
		if (window_data->callback_moving)
			window_data->callback_moving(window_data->instance);
		break;

	case SDL_WINDOWEVENT_RESIZED:
		window_data->width = window_event.data1;
		window_data->height = window_event.data2;
		if (window_data->callback_manually_resized)
			window_data->callback_manually_resized(window_data->instance);
		break;

	case SDL_WINDOWEVENT_SIZE_CHANGED:
		window_data->width = window_event.data1;
		window_data->height = window_event.data2;
		if (window_data->callback_resized)
			window_data->callback_resized(window_data->instance);
		break;

	case SDL_WINDOWEVENT_MINIMIZED:
		window_data->minimized = true;
		window_data->maximized = false;
		window_data->windowed_mode = false;
		if (window_data->callback_minimized)
			window_data->callback_minimized(window_data->instance);
		break;

	case SDL_WINDOWEVENT_MAXIMIZED:
		window_data->minimized = false;
		window_data->maximized = true;
		window_data->windowed_mode = false;
		if (window_data->callback_maximized)
			window_data->callback_maximized(window_data->instance);
		break;

	case SDL_WINDOWEVENT_RESTORED:
		window_data->minimized = false;
		window_data->maximized = false;
		window_data->windowed_mode = true;
		if (window_data->callback_windowed_mode)
			window_data->callback_windowed_mode(window_data->instance);
		break;

	case SDL_WINDOWEVENT_TAKE_FOCUS:
		SDL_RaiseWindow(window_data->instance->window);
		break;

		// ignored for now
	case SDL_WINDOWEVENT_SHOWN:
	case SDL_WINDOWEVENT_HIDDEN:
	case SDL_WINDOWEVENT_EXPOSED:
	case SDL_WINDOWEVENT_HIT_TEST:
	case SDL_WINDOWEVENT_ICCPROF_CHANGED:
	case SDL_WINDOWEVENT_DISPLAY_CHANGED:
		break;
	}

	return 0;
}

// Parses the SDL_Keycode and updates the MG_WindowData accordingly.
static int MG_key_event_parse(MG_WindowData* window_data, SDL_Scancode key_code, bool toggle)
{
	if (!window_data)
		return 0;

	if (key_code == SDL_SCANCODE_ESCAPE)
		return 1;

	switch (key_code)
	{
	case SDL_SCANCODE_RETURN:
		window_data->keyboard_held.keys.RETURN = toggle;
		break;
	case SDL_SCANCODE_BACKSPACE:
		window_data->keyboard_held.keys.BACKSPACE = toggle;
		break;
	case SDL_SCANCODE_TAB:
		window_data->keyboard_held.keys.TAB = toggle;
		break;
	case SDL_SCANCODE_SPACE:
		window_data->keyboard_held.keys.SPACE = toggle;
		break;
	case SDL_SCANCODE_LCTRL:
	case SDL_SCANCODE_RCTRL:
		window_data->keyboard_held.keys.CTRL = toggle;
		break;
	case SDL_SCANCODE_LSHIFT:
	case SDL_SCANCODE_RSHIFT:
		window_data->keyboard_held.keys.SHIFT = toggle;
		break;
	case SDL_SCANCODE_LALT:
	case SDL_SCANCODE_RALT:
		window_data->keyboard_held.keys.ALT = toggle;
		break;
	case SDL_SCANCODE_COMMA:
		window_data->keyboard_held.keys.COMMA = toggle;
		break;
	case SDL_SCANCODE_PERIOD:
		window_data->keyboard_held.keys.PERIOD = toggle;
		break;
	case SDL_SCANCODE_SEMICOLON:
		window_data->keyboard_held.keys.SEMICOLON = toggle;
		break;
	case SDL_SCANCODE_GRAVE:
		window_data->keyboard_held.keys.GRAVE = toggle;
		break;
	case SDL_SCANCODE_SLASH:
		window_data->keyboard_held.keys.SLASH = toggle;
		break;
	case SDL_SCANCODE_UP:
		window_data->keyboard_held.keys.ARROW_UP = toggle;
		break;
	case SDL_SCANCODE_DOWN:
		window_data->keyboard_held.keys.ARROW_DOWN = toggle;
		break;
	case SDL_SCANCODE_LEFT:
		window_data->keyboard_held.keys.ARROW_LEFT = toggle;
		break;
	case SDL_SCANCODE_RIGHT:
		window_data->keyboard_held.keys.ARROW_RIGHT = toggle;
		break;
	case SDL_SCANCODE_KP_0:
		window_data->keyboard_held.keys.NUMPAD_0 = toggle;
		break;
	case SDL_SCANCODE_KP_1:
		window_data->keyboard_held.keys.NUMPAD_1 = toggle;
		break;
	case SDL_SCANCODE_KP_2:
		window_data->keyboard_held.keys.NUMPAD_2 = toggle;
		break;
	case SDL_SCANCODE_KP_3:
		window_data->keyboard_held.keys.NUMPAD_3 = toggle;
		break;
	case SDL_SCANCODE_KP_4:
		window_data->keyboard_held.keys.NUMPAD_4 = toggle;
		break;
	case SDL_SCANCODE_KP_5:
		window_data->keyboard_held.keys.NUMPAD_5 = toggle;
		break;
	case SDL_SCANCODE_KP_6:
		window_data->keyboard_held.keys.NUMPAD_6 = toggle;
		break;
	case SDL_SCANCODE_KP_7:
		window_data->keyboard_held.keys.NUMPAD_7 = toggle;
		break;
	case SDL_SCANCODE_KP_8:
		window_data->keyboard_held.keys.NUMPAD_8 = toggle;
		break;
	case SDL_SCANCODE_KP_9:
		window_data->keyboard_held.keys.NUMPAD_9 = toggle;
		break;
	case SDL_SCANCODE_A:
		window_data->keyboard_held.keys.A = toggle;
		break;
	case SDL_SCANCODE_B:
		window_data->keyboard_held.keys.B = toggle;
		break;
	case SDL_SCANCODE_C:
		window_data->keyboard_held.keys.C = toggle;
		break;
	case SDL_SCANCODE_D:
		window_data->keyboard_held.keys.D = toggle;
		break;
	case SDL_SCANCODE_E:
		window_data->keyboard_held.keys.E = toggle;
		break;
	case SDL_SCANCODE_F:
		window_data->keyboard_held.keys.F = toggle;
		break;
	case SDL_SCANCODE_G:
		window_data->keyboard_held.keys.G = toggle;
		break;
	case SDL_SCANCODE_H:
		window_data->keyboard_held.keys.H = toggle;
		break;
	case SDL_SCANCODE_I:
		window_data->keyboard_held.keys.I = toggle;
		break;
	case SDL_SCANCODE_J:
		window_data->keyboard_held.keys.J = toggle;
		break;
	case SDL_SCANCODE_K:
		window_data->keyboard_held.keys.K = toggle;
		break;
	case SDL_SCANCODE_L:
		window_data->keyboard_held.keys.L = toggle;
		break;
	case SDL_SCANCODE_M:
		window_data->keyboard_held.keys.M = toggle;
		break;
	case SDL_SCANCODE_N:
		window_data->keyboard_held.keys.N = toggle;
		break;
	case SDL_SCANCODE_O:
		window_data->keyboard_held.keys.O = toggle;
		break;
	case SDL_SCANCODE_P:
		window_data->keyboard_held.keys.P = toggle;
		break;
	case SDL_SCANCODE_Q:
		window_data->keyboard_held.keys.Q = toggle;
		break;
	case SDL_SCANCODE_R:
		window_data->keyboard_held.keys.R = toggle;
		break;
	case SDL_SCANCODE_S:
		window_data->keyboard_held.keys.S = toggle;
		break;
	case SDL_SCANCODE_T:
		window_data->keyboard_held.keys.T = toggle;
		break;
	case SDL_SCANCODE_U:
		window_data->keyboard_held.keys.U = toggle;
		break;
	case SDL_SCANCODE_V:
		window_data->keyboard_held.keys.V = toggle;
		break;
	case SDL_SCANCODE_W:
		window_data->keyboard_held.keys.W = toggle;
		break;
	case SDL_SCANCODE_X:
		window_data->keyboard_held.keys.X = toggle;
		break;
	case SDL_SCANCODE_Y:
		window_data->keyboard_held.keys.Y = toggle;
		break;
	case SDL_SCANCODE_Z:
		window_data->keyboard_held.keys.Z = toggle;
		break;
	case SDL_SCANCODE_0:
		window_data->keyboard_held.keys.NUM_0 = toggle;
		break;
	case SDL_SCANCODE_1:
		window_data->keyboard_held.keys.NUM_1 = toggle;
		break;
	case SDL_SCANCODE_2:
		window_data->keyboard_held.keys.NUM_2 = toggle;
		break;
	case SDL_SCANCODE_3:
		window_data->keyboard_held.keys.NUM_3 = toggle;
		break;
	case SDL_SCANCODE_4:
		window_data->keyboard_held.keys.NUM_4 = toggle;
		break;
	case SDL_SCANCODE_5:
		window_data->keyboard_held.keys.NUM_5 = toggle;
		break;
	case SDL_SCANCODE_6:
		window_data->keyboard_held.keys.NUM_6 = toggle;
		break;
	case SDL_SCANCODE_7:
		window_data->keyboard_held.keys.NUM_7 = toggle;
		break;
	case SDL_SCANCODE_8:
		window_data->keyboard_held.keys.NUM_8 = toggle;
		break;
	case SDL_SCANCODE_9:
		window_data->keyboard_held.keys.NUM_9 = toggle;
		break;
	}

	return 0;
}

// Parses the SDL_MouseButtonEvent and updates the MG_WindowData accordingly.
static void MG_mouse_button_event_parse(MG_WindowData* window_data, SDL_MouseButtonEvent button_event, bool toggle)
{
	if (!window_data) return;

	switch (button_event.button)
	{
	case SDL_BUTTON_LEFT:
		window_data->mouse_held.LEFT = toggle;
		break;
	case SDL_BUTTON_RIGHT:
		window_data->mouse_held.RIGHT = toggle;
		break;
	case SDL_BUTTON_MIDDLE:
		window_data->mouse_held.MIDDLE = toggle;
		break;
	}
}

// Call this to update the pressed keys and mouse button flags.
// Not calling this will cause keyboard_pressed to contain old data. Held keys/buttons will still work fine however.
void MG_input_poll_pressed(MG_WindowData* window_data)
{
	if (!window_data) return;

	for (int i = 0; i < sizeof(window_data->keyboard_held.raw) * 8; i++)
	{
		if ((window_data->keyboard_prev.raw & (1ULL << i)) && (window_data->keyboard_held.raw & (1ULL << i)))
			window_data->keyboard_pressed.raw &= ~(1ULL << i);
		else if (!(window_data->keyboard_prev.raw & (1ULL << i)) && (window_data->keyboard_held.raw & (1ULL << i)))
			window_data->keyboard_pressed.raw |= (1ULL << i);
		else
			window_data->keyboard_pressed.raw &= ~(1ULL << i);
	}

	// this is ugly. but it works.
#define NUM_MOUSE_BUTTONS 3
	bool* mouse_held_buttons[NUM_MOUSE_BUTTONS] = { &window_data->mouse_held.LEFT, &window_data->mouse_held.MIDDLE, &window_data->mouse_held.RIGHT };
	bool* mouse_pressed_buttons[NUM_MOUSE_BUTTONS] = { &window_data->mouse_pressed.LEFT, &window_data->mouse_pressed.MIDDLE, &window_data->mouse_pressed.RIGHT };
	bool* mouse_prev_buttons[NUM_MOUSE_BUTTONS] = { &window_data->mouse_prev.LEFT, &window_data->mouse_prev.MIDDLE, &window_data->mouse_prev.RIGHT };
	for (int i = 0; i < NUM_MOUSE_BUTTONS; i++)
	{
		if (mouse_prev_buttons[i] && mouse_held_buttons[i])
			*mouse_pressed_buttons[i] = false;
		else if (!mouse_prev_buttons[i] && mouse_held_buttons[i])
			*mouse_pressed_buttons[i] = true;
		else
			*mouse_pressed_buttons[i] = false;
	}
#undef NUM_MOUSE_BUTTONS

	window_data->keyboard_prev = window_data->keyboard_held;
	window_data->mouse_prev = window_data->mouse_held;
}

// Puts into out_x_rel and out_y_rel the relative mouse position since the last call to this function.
// This also resets the relative mouse position to 0, so set the out_rel variables to NULL to simply reset the relative mouse position tracker.
void MG_input_poll_mouse_relative_pos(MG_WindowData* window_data, int32_t* out_x_rel, int32_t* out_y_rel)
{
	if (!window_data) return;
	if (out_x_rel) *out_x_rel = window_data->mouse_x_rel;
	if (out_y_rel) *out_y_rel = window_data->mouse_y_rel;

	window_data->mouse_x_rel = 0;
	window_data->mouse_y_rel = 0;
}

// Puts into out_rel_scroll the relative scroll amount since the last call to this function.
// This also resets the relative mouse position to 0, so set the out_rel_scroll variable to NULL to simply reset the relative mouse position tracker.
void MG_input_poll_mouse_scroll(MG_WindowData* window_data, float* out_rel_scroll)
{
	if (!window_data) return;
	if (out_rel_scroll) *out_rel_scroll = window_data->mouse_wheel_rel;

	window_data->mouse_wheel_rel = 0;
}

bool MG_input_key_down(MG_WindowData* window_data, MG_Key key_code)
{
	// might break in the future or on various platforms lol
	return window_data->keyboard_pressed.raw & (1ULL << key_code);
}

bool MG_input_key_held(MG_WindowData* window_data, MG_Key key_code)
{
	return window_data->keyboard_held.raw & (1ULL << key_code);
}

bool MG_input_mouse_button_down(MG_WindowData* window_data, MG_MouseButton button)
{
	switch (button)
	{
	case MG_MOUSE_BUTTON_LEFT:
		return window_data->mouse_pressed.LEFT;
	case MG_MOUSE_BUTTON_RIGHT:
		return window_data->mouse_pressed.RIGHT;
	case MG_MOUSE_BUTTON_MIDDLE:
		return window_data->mouse_pressed.MIDDLE;
	default:
		return false;
	}
}

bool MG_input_mouse_button_held(MG_WindowData* window_data, MG_MouseButton button)
{
	switch (button)
	{
	case MG_MOUSE_BUTTON_LEFT:
		return window_data->mouse_held.LEFT;
	case MG_MOUSE_BUTTON_RIGHT:
		return window_data->mouse_held.RIGHT;
	case MG_MOUSE_BUTTON_MIDDLE:
		return window_data->mouse_held.MIDDLE;
	default:
		return false;
	}
}