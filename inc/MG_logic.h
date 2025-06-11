#pragma once

#include "MG_include.h"
#include "MG_window.h"
#include "MG_Instance.h"

int MG_logic_loop(void* instance);

int MG_create_object(MG_Instance* instance, MG_Component root_component, uint32_t flags, void (*on_load)(MG_Object*), void (*on_tick)(MG_Object*));
int MG_create_object_by_copy(MG_Object* object);
MG_Object* MG_get_object_by_id(MG_Instance* instance, uint32_t id);
int MG_delete_object(MG_Instance* instance, uint32_t id);
