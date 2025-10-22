#pragma once

#include "MG_include.h"
#include "MG_LL.h"

typedef enum
{
	MG_SHADER_STATUS_NOT_IMPLEMENTED = 0,
	MG_SHADER_STATUS_OK = 1,
	MG_SHADER_STATUS_ERROR = 2,
}
MG_ShaderStatus;

typedef struct MG_Shader
{
	GLuint ID;

	MG_ShaderStatus status;

	const char* vertex_shader_file;
	const char* fragment_shader_file;
}
MG_Shader;