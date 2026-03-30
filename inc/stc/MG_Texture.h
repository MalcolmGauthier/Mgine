#pragma once

#include "MG_include.h"

typedef MG_ID MG_TEXTURE;

typedef struct MG_Texture
{
	MG_Asset base;

	MG_TEXTURE id;
	GLuint GL_id;
	int width;
	int height;
	byte channels;
}
MG_Texture;