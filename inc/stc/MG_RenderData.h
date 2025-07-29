#pragma once

#include "MG_Instance.h"

typedef struct MG_RenderData
{
	MG_Instance* instance;

	MG_GameData old_data;
	MG_GameData latest_data;
	// 0 = old data, 1 = latest data; 0 <= interp_value <= 1
	float interp_value;

	MG_Mesh_LL* transparency_list;
	GLuint accum_tex;
	GLuint reveal_tex;
	GLuint OIT_FBO;
}
MG_RenderData;