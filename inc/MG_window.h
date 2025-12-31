#pragma once

#include "MG_include.h"
#include "stc/MG_Instance.h"

#ifdef __cplusplus
extern "C" {
#endif

MG_API int MG_window_loop(void* instance);

MG_API void MG_input_poll_pressed(MG_WindowData* window_data);
MG_API void MG_input_poll_mouse_relative_pos(MG_WindowData* window_data, int32_t* out_x_rel, int32_t* out_y_rel);
MG_API void MG_input_poll_mouse_scroll(MG_WindowData* window_data, float* out_rel_scroll);

bool MG_input_key_down(MG_WindowData* window_data, MG_Key key_code);
bool MG_input_key_held(MG_WindowData* window_data, MG_Key key_code);
bool MG_input_mouse_button_down(MG_WindowData* window_data, MG_MouseButton button);
bool MG_input_mouse_button_held(MG_WindowData* window_data, MG_MouseButton button);

#ifdef __cplusplus
}
#endif