#pragma once

#include "MG_include.h"
#include "MG_WindowData.h"
#include "MG_GameData.h"
#include "MG_RenderData.h"
#include "MG_Audio.h"

typedef enum
{
	MG_GAME_DATA_LOCK_OWNER_NONE = 0,
	MG_GAME_DATA_LOCK_OWNER_LOGIC_THREAD = 1,
	MG_GAME_DATA_LOCK_OWNER_RENDER_THREAD = 2,
}
MG_GameDataLockOwner;

typedef MG_Generic_LL MG_ComponentTemplate_LL;

typedef struct MG_Instance
{
	SDL_Window* window;
	SDL_GLContext gl_context;
	MG_WindowData window_data;
	MG_GameData game_data;
	MG_Audio audio_data;
	MG_RenderData render_data;

	MG_GameDataLockOwner lock_owner;

	int gl_error_code;
	int instance_exit_code;
	MG_ID instance_id;

	//TODO: GIANT ASS PROBLEM: using arrays means using realloc to add new data which means anything pointing to stuff here becomes invalid.
	// should change all of this to become linked lists. will use more memory, but said memory will not cause dangling pointers.
	uint32_t shader_code_count;
	char** shader_code_list;
	uint32_t shader_count;
	struct MG_Shader* shader_list;
	uint32_t material_count;
	// material list is double pointer due to materials having variable size
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

	MG_ComponentTemplate_LL* component_list;

	bool active : 1;
	bool initialized : 1;
	bool rendering_enabled : 1;
}
MG_Instance;
