#pragma once

#include "MG_include.h"
#include "MG_WindowData.h"
#include "MG_GameData.h"
#include "MG_Object.h"
#include "MG_RenderData.h"

typedef struct MG_Instance
{
	SDL_Window* window;
	SDL_GLContext gl_context;
	MG_WindowData window_data;
	MG_GameData game_data;
	MG_RenderData render_data;
	MG_GameDataLockOwner lock_owner;

	int gl_error_code;
	int instance_exit_code;
	int64_t instance_id;

	uint32_t shader_code_count;
	char** shader_code_list;
	uint32_t shader_count;
	struct MG_Shader* shader_list;
	uint32_t material_count;
	struct MG_Material** material_list;
	uint32_t prefab_count;
	struct MG_Object* prefab_list;
	uint32_t scene_count;
	struct MG_Scene* scene_list;

	uint32_t model_count;
	struct MG_Model* model_list;
	uint32_t texture_count;
	struct MG_Texture* texture_list;
	uint32_t sound_count;
	struct MG_Sound* sound_list;

	struct MG_ComponentTemplate_LL* component_list;

	bool active : 1;
	bool initialized : 1;
	bool rendering_enabled : 1;
}
MG_Instance;
