#pragma once
#include "stc/MG_component.h"
#include "MG_include.h"

uint32_t MG_component_register_id();

MG_Component* MG_component_create_copy(MG_Component* src, MG_Object* dst_parent);
MG_Component* MG_component_copy_untracked(MG_Component* source);

void MG_component_free(MG_Component* component);