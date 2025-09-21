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
	uint64_t instance_id;

	bool active : 1;
	bool initialized : 1;
}
MG_Instance;
