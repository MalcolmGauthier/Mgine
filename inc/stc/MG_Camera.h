#pragma once

#include "stc/MG_Transform.h"

typedef struct MG_Camera
{
	MG_Vec3 position;
	// when focus is null, rotation is used. when not null, only rotation roll is used.
	MG_Vec3* focus;
	MG_Vec3 rotation;

	float fov;
	float screen_width;
	float screen_height;
	float near_clip;
	float far_clip;

	MG_Matrix projection_matrix;
}
MG_Camera;