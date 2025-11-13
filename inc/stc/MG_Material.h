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

typedef MG_Generic_LL MG_MaterialShaderVariable_LL;

typedef struct MG_Material
{
	float diffuse_rgba[4];

	struct MG_Texture* diffuse_texture;

	bool contains_transparency;

	struct MG_Shader* shader;
	uint32_t shader_variable_count;
	MG_MaterialShaderVariable* shader_variables;
}
MG_Material;
