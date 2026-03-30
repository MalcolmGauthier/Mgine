#pragma once

#include "MG_include.h"
#include "stc/MG_Object.h"
#include "stc/MG_Texture.h"

typedef MG_ID MG_SCENE;

typedef struct MG_Scene
{
	MG_SCENE id;
	MG_NAME name;

	MG_Object_LL* objects;

	void (*on_load)(struct MG_Scene* self, void* input_data);
	void (*on_reset)(struct MG_Scene* self);

	uint32_t texture_count;
	MG_Texture** textures;

	uint32_t model_count;
	struct MG_Model** models;
}
MG_Scene;