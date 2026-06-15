#pragma once

#include "MG_include.h"
#include "MG_WindowData.h"
#include "MG_GameData.h"
#include "MG_RenderData.h"
#include "MG_Audio.h"
#include "MG_asset.h"

typedef enum
{
	MG_GAME_DATA_LOCK_OWNER_NONE = 0,
	MG_GAME_DATA_LOCK_OWNER_LOGIC_THREAD = 1,
	MG_GAME_DATA_LOCK_OWNER_RENDER_THREAD = 2,
}
MG_GameDataLockOwner;

typedef MG_LinkedList MG_ComponentTemplate_LL;

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

	// these are hashmaps because due to the ID system, we need as much speed in pointer retreival as we can get.
	// assets can be extracted with the functions in MG_asset.c

	struct MG_AssetList shader_list;
	struct MG_AssetList material_list;
	struct MG_AssetList prefab_list;
	struct MG_AssetList scene_list;

	// these three take up more memory, so unlike the previous assets,
	// they're not always loaded, and thus need to be loaded from a file before use.

	struct MG_AssetList model_list;
	struct MG_AssetList texture_list;
	struct MG_AssetList sound_list;

	struct MG_AssetList component_template_list;
	// whilst not assets, strings need to be treated as such to be properly serialized in component struct values.
	struct MG_AssetList string_list;

	bool active : 1;
	bool initialized : 1;
	bool rendering_enabled : 1;
}
MG_Instance;
