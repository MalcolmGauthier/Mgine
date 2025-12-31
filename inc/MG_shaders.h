#pragma once

#include "MG_include.h"
#include "stc/MG_Shader.h"
#include "stc/MG_Transform.h"
#include "stc/MG_Material.h"

#ifdef __cplusplus
extern "C" {
#endif

MG_API MG_Shader MG_shader_create(char* vertex_shader, char* fragment_shader);
MG_API int MG_shader_define(char** ptr_to_shader_file_text, int define_count, ...);
MG_API int MG_shader_compile(MG_Shader* shader);
MG_API void MG_shader_use(MG_Shader* shader);
MG_API int MG_shader_free(MG_Shader* shader);

MG_API int MG_material_register_variable(MG_Material* material, const char* name, GLenum type, uint32_t offset_in_material);

MG_API void MG_shader_set_int(MG_Shader* shader, const char* name, int value);
MG_API void MG_shader_set_ivec2(MG_Shader* shader, const char* name, int* value);
MG_API void MG_shader_set_ivec3(MG_Shader* shader, const char* name, int* value);
MG_API void MG_shader_set_float(MG_Shader* shader, const char* name, float value);
MG_API void MG_shader_set_vec2(MG_Shader* shader, const char* name, MG_Vec2 value);
MG_API void MG_shader_set_vec3(MG_Shader* shader, const char* name, MG_Vec3 value);
MG_API void MG_shader_set_mat4(MG_Shader* shader, const char* name, MG_Matrix* value);

#ifdef __cplusplus
}
#endif