#pragma once

#include "MG_include.h"

typedef struct MG_MaterialShaderVariable
{
	const char* name;
	GLenum type;
	uint32_t offset_in_material;
}
MG_MaterialShaderVariable;

typedef MG_Generic_LL MG_MaterialShaderVariable_LL;

typedef struct MG_Material
{
	float diffuse_rgb[3];
	float specular_rgb[3];
	float shininess;

	char* diffuse_texture_path;
	GLuint diffuse_texture;

	bool contains_transparency;

	struct MG_Shader* shader;
	MG_MaterialShaderVariable_LL* shader_variable_translations;
}
MG_Material;
