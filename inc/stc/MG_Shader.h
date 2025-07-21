#pragma once

#include "MG_include.h"

typedef struct MG_Shader
{
	GLuint ID;
	GLuint vertex_shader;
	GLuint fragment_shader;

	const char* vertex_shader_file;
	const char* fragment_shader_file;
}
MG_Shader;