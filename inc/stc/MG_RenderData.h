#pragma once

#include "stc/MG_Shader.h"

typedef MG_Generic_LL MG_Mesh_LL;
typedef MG_Generic_LL MG_TransparentDraw_LL;

typedef struct MG_RenderData
{
	MG_GameData old_data;
	MG_GameData latest_data;
	// 0 = old data, 1 = latest data; 0 <= interp_value <= 1
	float interp_value;
	MG_Matrix view_matrix;

	MG_TransparentDraw_LL* transparency_list;
	GLuint accum_tex;
	GLuint reveal_tex;
	GLuint OIT_FBO;
	GLuint OIT_RBO;
	GLuint OIT_VAO_reveal;
	MG_Shader* OIT_shader_accum;
	MG_Shader* OIT_shader_comp;
}
MG_RenderData;