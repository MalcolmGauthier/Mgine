#pragma once

#include "MG_include.h"
#include "stc/MG_Object.h"

typedef struct MG_Texture
{
	GLuint id;
	bool loaded;

	const char* path;
	uint32_t index_in_file;
}
MG_Texture;

typedef struct MG_Model
{
	GLuint id;
	bool loaded;

	const char* path;
	uint32_t index_in_file;
}
MG_Model;

typedef struct MG_Scene
{
	const char* name;
	uint32_t id;

	MG_Object* objects;
	void (*on_load)(void* input_data);

	uint32_t texture_count;
	MG_Texture* textures;

	uint32_t model_count;
	MG_Model* models;
}
MG_Scene;