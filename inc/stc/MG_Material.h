#pragma once

#include "MG_include.h"
#include "stc/MG_Scene.h"

typedef struct MG_MaterialShaderVariable
{
	const char* name;
	GLenum type;
	uint32_t size;
	uint32_t offset_in_material;
}
MG_MaterialShaderVariable;

typedef MG_ID MG_MATERIAL;

typedef struct MG_Material
{
	float diffuse_rgba[4];

	struct MG_Texture* diffuse_texture;

	bool contains_transparency;

	MG_MATERIAL id;
	struct MG_Shader* shader;
	uint32_t shader_variable_count;
	MG_MaterialShaderVariable* shader_variables;

	void (*free_func)(struct MG_Material* self);
}
MG_Material;
