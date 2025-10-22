#pragma once

#include "MG_include.h"
#include "stc/MG_Camera.h"

MG_Camera MG_camera_init(float FOV, float screen_width, float screen_height, float near_clip, float far_clip);
MG_Matrix MG_camera_get_view_matrix(MG_Camera* camera);

void MG_camera_set_perspective(MG_Camera* camera, float FOV, float screen_width, float screen_height, float near_clip, float far_clip);
void MG_camera_set_orthographic(MG_Camera* camera, float left, float right, float bottom, float top, float near_clip, float far_clip);