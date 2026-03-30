#pragma once

#include "MG_include.h"
#include "MG_id.h"

typedef enum
{
	MG_SHADER_STATUS_NOT_IMPLEMENTED = 0,
	MG_SHADER_STATUS_OK = 1,
	MG_SHADER_STATUS_ERROR = 2,
}
MG_ShaderStatus;

typedef MG_ID MG_SHADER;

typedef struct MG_Shader
{
	GLuint GL_id;
	MG_SHADER id;

	MG_ShaderStatus status;

	char* vertex_shader_code;
	char* fragment_shader_code;
}
MG_Shader;