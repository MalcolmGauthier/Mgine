#pragma once
#include "stc/MG_component.h"
#include "MG_include.h"
#include "MG_transform.h"

MG_ComponentTemplate* MG_component_register(MG_GameData* game_data, size_t struct_size, const char* name,
	MG_ComponentFuncResult(*on_create)(struct MG_Component* self),
	MG_ComponentFuncResult(*on_update)(struct MG_Component* self, float delta_time),
	void (*on_destroy)(struct MG_Component* self));

void MG_initialize_components(MG_GameData* game_data);


uint32_t MG_component_get_id_by_name(const char* name);

MG_Component* MG_component_create_copy(MG_Component* src, MG_Object* dst_parent);
// only exists to be used as a callback for MG_LL_copy. parent is set to NULL.
MG_Component* MG_component_copy_untracked(MG_Component* source);

void MG_component_free(MG_Component* component);