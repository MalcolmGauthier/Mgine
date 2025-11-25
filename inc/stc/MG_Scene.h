#pragma once

#include "MG_include.h"
#include "stc/MG_Object.h"

typedef struct MG_Texture
{
	MG_Asset base;
	
	GLuint id;
}
MG_Texture;

typedef struct MG_Scene
{
	char* name;
	uint32_t id;

	MG_Object_LL* objects;
	void (*on_load)(void* input_data);

	uint32_t texture_count;
	MG_Texture** textures;

	uint32_t model_count;
	struct MG_Model** models;
}
MG_Scene;