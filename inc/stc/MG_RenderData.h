#pragma once

#include "MG_Instance.h"

typedef struct MG_RenderData
{
	MG_Instance* instance;

	MG_GameData old_data;
	MG_GameData latest_data;

	struct MG_Mesh_LL* transparency_list;
	GLuint accum_tex;
	GLuint reveal_tex;
	GLuint OIT_FBO;
}
MG_RenderData;