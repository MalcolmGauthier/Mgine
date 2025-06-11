#pragma once

#include "MG_include.h"
#include "MG_Instance.h"

int MG_window_loop(void* instance);

void MG_input_update_pressed(MG_WindowData* window_data);
void MG_input_poll_mouse_relative_pos(MG_WindowData* window_data, int32_t* out_x_rel, int32_t* out_y_rel);
void MG_input_poll_mouse_scroll(MG_WindowData* window_data, float* out_rel_scroll);