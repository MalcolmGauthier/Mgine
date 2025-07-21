#pragma once

#include "MG_include.h"
#include "MG_LL.h"

typedef enum
{
	MG_GAME_DATA_LOCK_OWNER_NONE = 0,
	MG_GAME_DATA_LOCK_OWNER_LOGIC_THREAD = 1,
	MG_GAME_DATA_LOCK_OWNER_RENDER_THREAD = 2,
} MG_GameDataLockOwner;

typedef MG_Generic_LL MG_Object_LL;

typedef struct MG_GameData
{
	MG_Instance* instance;

	uint64_t global_timer;
	uint64_t next_object_id;
	unsigned int tickrate;
	// this should be constant unless tickrate is uncapped.
	// however, this still acurately reports tick length even when tickrate is capped, usefull for lag detection.
	float delta_time;

	MG_Object_LL* object_list;
	uint32_t object_count;
}
MG_GameData;
