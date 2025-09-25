#pragma once

#include "MG_include.h"
#include "MG_Shader.h"
#include "MG_Transform.h"

MG_Shader* MG_shader_create(const char* vertex_shader, const char* fragment_shader);
int MG_shader_compile(MG_Shader* shader);