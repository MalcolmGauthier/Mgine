#pragma once

#include "MG_include.h"

typedef struct MG_Transform
{
	float X;
	float Y;
	float Z;

	float pitch;
	float yaw;
	float roll;

	float scale_x;
	float scale_y;
	float scale_z;
} MG_Transform;

typedef struct
{
	union
	{
		float x;
		float u;
	};
	union
	{
		float y;
		float v;
	};
} MG_Vec2;
typedef struct
{
	union
	{
		float x;
		float r;
	};
	union
	{
		float y;
		float g;
	};
	union
	{
		float z;
		float b;
	};
} MG_Vec3;

typedef struct
{
	mat4 matrix;
} MG_Matrix;
