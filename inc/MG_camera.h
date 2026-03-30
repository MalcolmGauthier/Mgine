#pragma once

#include "MG_include.h"
#include "stc/MG_Camera.h"
#include "MG_transform.h"
#include "MG_object.h"

#ifdef __cplusplus
extern "C" {
#endif

MG_API MG_Camera MG_camera_init(float FOV, float screen_width, float screen_height, float near_clip, float far_clip);
MG_API MG_Matrix MG_camera_get_view_matrix(MG_GameData* game_data);

MG_API MG_Camera* MG_camera_get();

MG_API MG_Vec3 MG_camera_get_world_position();
MG_API MG_Vec3 MG_camera_get_world_rotation();

MG_API void MG_camera_set_perspective(float FOV, float screen_width, float screen_height, float near_clip, float far_clip);
MG_API void MG_camera_set_orthographic(float left, float right, float bottom, float top, float near_clip, float far_clip);

#ifdef __cplusplus
}
#endif