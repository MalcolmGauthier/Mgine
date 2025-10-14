#pragma once

#include "MG_include.h"
#include "stc/MG_Camera.h"

MG_Camera MG_camera_init(float FOV, float screen_width, float screen_height, float near_clip, float far_clip);
MG_Matrix MG_camera_get_view_matrix(MG_Camera* camera);