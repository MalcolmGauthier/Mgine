#pragma once

#include "MG_include.h"

typedef struct MG_GameData
{
	MG_Instance* instance;

	uint64_t global_timer;
	uint64_t next_object_id;
	unsigned int tickrate;
	// this should be constant unless tickrate is uncapped.
	// however, this still acurately reports tick length even when tickrate is capped, usefull for lag detection.
	float delta_time;

	struct MG_Object_LL* object_list;
	uint32_t object_count;
}
MG_GameData;
