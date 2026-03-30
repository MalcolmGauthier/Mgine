#pragma once

#include "MG_include.h"
#include "stc/MG_Instance.h"

#ifdef __cplusplus
extern "C" {
#endif

// Main loop for the window and I/O events.
MG_API int MG_window_loop();


// Call this to update the pressed keys and mouse button flags.
// This is automatically called at the start of every tick, but can be called manually to update the keyboard and mouse button data
MG_API void MG_input_poll_pressed();

// Puts into out_x_rel and out_y_rel the relative mouse position since the last call to this function.
// This also resets the relative mouse position to 0, so set the out_rel variables to NULL to simply reset the relative mouse position tracker.
MG_API void MG_input_poll_mouse_relative_pos(int32_t* out_x_rel, int32_t* out_y_rel);

// Puts into out_rel_scroll the relative scroll amount since the last call to this function.
// This also resets the relative mouse position to 0, so set the out_rel_scroll variable to NULL to simply reset the relative mouse position tracker.
MG_API void MG_input_poll_mouse_scroll(float* out_rel_scroll);


bool MG_input_key_down(MG_Key key_code);

bool MG_input_key_held(MG_Key key_code);

bool MG_input_mouse_button_down(MG_MouseButton button);

bool MG_input_mouse_button_held(MG_MouseButton button);

#ifdef __cplusplus
}
#endif