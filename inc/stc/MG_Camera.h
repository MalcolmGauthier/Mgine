#pragma once

#include "stc/MG_Transform.h"
#include "MG_id.h"

typedef struct MG_Camera
{
	// the anchor is the ID of an object whose position is the anchor of the camera's position.
	// a 0 or invalid id will default to having the focus be at the origin of the map.
	MG_ID anchor;
	// the position is an offset to the anchor's position. so if the anchor is null/invalid, the offset becomes the full on position.
	MG_Vec3 position;
	// the focus is the ID of an object whose position is the location the camera will look at.
	// a 0 or invalid id will default to an angle of (0, 0, 0).
	MG_ID focus;
	// the rotation is an offset to the focus' rotation. so if the focus is null/invalid, the offset becomes the full on rotation.
	// Roll is never modified when looking at a focus. The only way to modify the camera's roll is through this offset.
	MG_Vec3 rotation;

	float fov;
	float screen_width;
	float screen_height;
	float near_clip;
	float far_clip;

	MG_Matrix projection_matrix;
}
MG_Camera;