#pragma once

#include "MG_include.h"
#include "MG_LL.h"
#include "stc/MG_Camera.h"

typedef MG_Generic_LL MG_Object_LL;

typedef struct MG_GameData
{
	MG_Instance* instance;

	uint64_t global_timer;
	uint64_t next_object_id;
	uint32_t next_component_id;
	unsigned int tickrate;
	// this should be constant unless tickrate is uncapped.
	// however, this still acurately reports tick length even when tickrate is capped. usefull for lag detection.
	float delta_time;
	double uptime;

	// list of top level object. all object obtained by filling out tree.
	MG_Object_LL* object_list;
	// complete count of objects, including children.
	uint32_t object_count;

	MG_Camera camera;
}
MG_GameData;
