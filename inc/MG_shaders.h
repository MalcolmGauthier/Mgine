#pragma once

#include "MG_include.h"
#include "MG_Shader.h"

MG_Shader* MG_create_shader(const char* vertex_shader, const char* fragment_shader);
int MG_compile_shader(MG_Shader* shader);