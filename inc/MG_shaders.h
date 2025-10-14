#pragma once

#include "MG_include.h"
#include "stc/MG_Shader.h"
#include "stc/MG_Transform.h"
#include "stc/MG_Material.h"

MG_Shader* MG_shader_create(const char* vertex_shader, const char* fragment_shader);
int MG_shader_define(char** ptr_to_shader_file_text, int define_count, ...);
int MG_shader_compile(MG_Shader* shader);
void MG_shader_use(MG_Shader* shader);
int MG_shader_free(MG_Shader* shader);

void MG_shader_set_int(MG_Shader* shader, const char* name, int value);
void MG_shader_set_ivec2(MG_Shader* shader, const char* name, int* value);
void MG_shader_set_ivec3(MG_Shader* shader, const char* name, int* value);
void MG_shader_set_float(MG_Shader* shader, const char* name, float value);
void MG_shader_set_vec2(MG_Shader* shader, const char* name, MG_Vec2 value);
void MG_shader_set_vec3(MG_Shader* shader, const char* name, MG_Vec3 value);
void MG_shader_set_mat4(MG_Shader* shader, const char* name, MG_Matrix* value);
