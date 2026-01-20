#pragma once

#include "MG_include.h"

typedef struct MG_Vec2
{
	union
	{
		float x;
		float u;
// unnamed structs/unions are technically non-standard, but this solution is too clean to ignore. too bad! msvc-only code.
#pragma warning(suppress: 4201)
	};
	union
	{
		float y;
		float v;
#pragma warning(suppress: 4201)
	};
} MG_Vec2;
typedef struct MG_Vec3
{
	// Mgine's official order for rotation is PITCH, YAW, ROLL. For use in glm, use the YZX ordering.
	// furthermore, X+ is FORWARD, Y+ is UP, Z+ is RIGHT
	union
	{
		float x;
		float r;
		float pitch;
#pragma warning(suppress: 4201)
	};
	union
	{
		float y;
		float g;
		float yaw;
#pragma warning(suppress: 4201)
	};
	union
	{
		float z;
		float b;
		float roll;
#pragma warning(suppress: 4201)
	};
} MG_Vec3;
typedef struct MG_Vec4
{
	union
	{
		float x;
		float r;
#pragma warning(suppress: 4201)
	};
	union
	{
		float y;
		float g;
#pragma warning(suppress: 4201)
	};
	union
	{
		float z;
		float b;
#pragma warning(suppress: 4201)
	};
	union
	{
		float w;
		float a;
#pragma warning(suppress: 4201)
	};
} MG_Vec4;

typedef struct MG_Transform
{
	MG_Vec3 position;
	MG_Vec3 rotation;
	MG_Vec3 scale;
}
MG_Transform;

typedef struct MG_Matrix
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
