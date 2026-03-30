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

	SDL_Thread* event_thread;
	SDL_Thread* logic_thread;
	SDL_Thread* render_thread;
	MG_GameDataLockOwner lock_owner;

	int gl_error_code;
	int instance_exit_code;
	uint64_t instance_id;

	// these are lists instead of linked lists, because realistically outside of the initial loading phase, these lists will not be modified much.
	// using arrays means using realloc to add new data which means storing an index to the list or a pointer to the list is a no-go.
	// all of these are arrays of pointers to each asset.
	uint32_t shader_count;
	struct MG_Shader** shader_list;
	// note: materials have variable size
	uint32_t material_count;
	struct MG_Material** material_list;
	uint32_t prefab_count;
	struct MG_Object** prefab_list;
	uint32_t scene_count;
	struct MG_Scene** scene_list;

	// these three take up more meory, so unlike the previous assets,
	// they're not always loaded, and thus need to be loaded from a file before use.
	uint32_t model_count;
	struct MG_Model** model_list;
	uint32_t texture_count;
	struct MG_Texture** texture_list;
	uint32_t sound_count;
	struct MG_Sound** sound_list;

	MG_ComponentTemplate_LL* component_list;

	bool active : 1;
	bool initialized : 1;
	bool rendering_enabled : 1;
}
MG_Instance;
