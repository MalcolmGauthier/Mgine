#pragma once

#include "MG_include.h"

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
		float pitch;
	};
	union
	{
		float y;
		float g;
		float yaw;
	};
	union
	{
		float z;
		float b;
		float roll;
	};
} MG_Vec3;
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
	union
	{
		float w;
		float a;
	};
} MG_Vec4;

typedef struct MG_Transform
{
	MG_Vec3 position;
	MG_Vec3 rotation;
	MG_Vec3 scale;
}
MG_Transform;

typedef struct
{
	float m00, m01, m02, m03;
	float m10, m11, m12, m13;
	float m20, m21, m22, m23;
	float m30, m31, m32, m33;
	// casting this struct directly to a mat4 is technically UB.
	// however, patching that out means losing assignment copying.
}
MG_Matrix;
#define MG_MATRIX_IDENTITY 1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1
