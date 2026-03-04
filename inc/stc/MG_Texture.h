#pragma once

#include "MG_include.h"

typedef struct MG_Texture
{
	MG_Asset base;

	GLuint id;
	int width;
	int height;
	byte channels;
}
MG_Texture;