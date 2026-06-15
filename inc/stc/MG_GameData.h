#pragma once

#include "MG_include.h"
#include "stc/MG_Camera.h"
#include "MG_LL.h"
#include "MG_hash.h"

typedef MG_LinkedList MG_Object_LL;

// This is the entire gamestate. 
typedef struct MG_GameData
{
	uint64_t global_timer;
	// this should be constant unless tickrate is uncapped.
	// however, this still acurately reports tick length even when tickrate is capped. useful for lag detection.
	float delta_time;
	double uptime;

	// list of ALL currently existing objects. not just orphan objects.
	struct MG_AssetList* object_list;

	MG_Camera camera;
}
MG_GameData;
