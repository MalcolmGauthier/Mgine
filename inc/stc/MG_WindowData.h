#pragma once

typedef struct MG_Instance MG_Instance;

// This is a union that represents the keyboard state.
// Currently supported keys include A-Z, 0-9, numpad 0-9, arrow keys, .,;`/, ret, bksp, tab, space, ctrl, shift, alt.
// Left and right keyboard modifiers are not distinguished.
union Keyboard
{
	uint64_t raw;
	struct Keys
	{
		bool A : 1;
		bool B : 1;
		bool C : 1;
		bool D : 1;
		bool E : 1;
		bool F : 1;
		bool G : 1;
		bool H : 1;
		bool I : 1;
		bool J : 1;
		bool K : 1;
		bool L : 1;
		bool M : 1;
		bool N : 1;
		bool O : 1;
		bool P : 1;
		bool Q : 1;
		bool R : 1;
		bool S : 1;
		bool T : 1;
		bool U : 1;
		bool V : 1;
		bool W : 1;
		bool X : 1;
		bool Y : 1;
		bool Z : 1;

		bool NUM_0 : 1;
		bool NUM_1 : 1;
		bool NUM_2 : 1;
		bool NUM_3 : 1;
		bool NUM_4 : 1;
		bool NUM_5 : 1;
		bool NUM_6 : 1;
		bool NUM_7 : 1;
		bool NUM_8 : 1;
		bool NUM_9 : 1;

		bool NUMPAD_0 : 1;
		bool NUMPAD_1 : 1;
		bool NUMPAD_2 : 1;
		bool NUMPAD_3 : 1;
		bool NUMPAD_4 : 1;
		bool NUMPAD_5 : 1;
		bool NUMPAD_6 : 1;
		bool NUMPAD_7 : 1;
		bool NUMPAD_8 : 1;
		bool NUMPAD_9 : 1;

		bool RETURN : 1;
		bool BACKSPACE : 1;
		bool TAB : 1;
		bool SPACE : 1;
		bool CTRL : 1;
		bool SHIFT : 1;
		bool ALT : 1;

		bool COMMA : 1;
		bool PERIOD : 1;
		bool SEMICOLON : 1;
		bool GRAVE : 1;
		bool SLASH : 1;

		bool ARROW_UP : 1;
		bool ARROW_DOWN : 1;
		bool ARROW_LEFT : 1;
		bool ARROW_RIGHT : 1;

	} keys;
};
struct Mouse
{
	bool LEFT;
	bool RIGHT;
	bool MIDDLE;
};

typedef struct MG_WindowData
{
	MG_Instance* instance;

	int32_t x_pos;
	int32_t y_pos;

	int32_t width;
	int32_t height;

	// pressed = pressed since last poll when not held before
	// held = currently held down
	// prev = held down before the last poll
	union Keyboard keyboard_pressed, keyboard_held, keyboard_prev;

	// mouse_pressed = pressed since last poll when not held before
	// mouse_held = currently held down
	// mouse_prev = held down before the last poll
	struct Mouse mouse_pressed, mouse_held, mouse_prev;

	// only updated when mouse is not captured
	int32_t mouse_x;
	// only updated when mouse is not captured
	int32_t mouse_y;

	int32_t mouse_x_rel;
	int32_t mouse_y_rel;

	float mouse_wheel_rel;

	bool mouse_above_window : 1;
	bool focused : 1;
	bool minimized : 1;
	bool maximized : 1;
	bool windowed_mode : 1;
	bool mouse_hidden : 1;
	bool mouse_grabbed : 1;

	void (*callback_mouse_above_enter)(void);
	void (*callback_mouse_above_exit)(void);

	void (*callback_focus_gained)(void);
	void (*callback_focus_lost)(void);

	void (*callback_moving)(void);

	// only called when the window is resized by the user, not anything else
	void (*callback_manually_resized)(void);
	void (*callback_resized)(void);

	void (*callback_minimized)(void);
	// does NOT get sent if the user restores a minimized window that was previously maximized, despite still being maximized.
	void (*callback_maximized)(void);
	void (*callback_windowed_mode)(void);
}
MG_WindowData;