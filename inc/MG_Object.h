#pragma once

#include "MG_include.h"
#include "stc/MG_object.h"

uint64_t MG_object_create(MG_Instance* instance, MG_Object* parent, uint32_t flags, void (*on_load)(MG_Object*), void (*on_tick)(MG_Object*));
uint64_t MG_object_create_by_copy(MG_Object* object);
uint64_t MG_object_create_with_parent(MG_Object* parent_object, uint32_t flags, void (*on_load)(MG_Object*), void (*on_tick)(MG_Object*));
// create a copy of the object that is not tracked by the engine.
// this returns a new object with the same data, but does not add it to the object list, and thus the object is not managed by the engine.
MG_Object* MG_object_create_untracked_copy(MG_Object* source);

MG_Object* MG_object_get_by_id(MG_Instance* instance, uint64_t id);
MG_Object_LL* MG_object_get_all(MG_Instance* instance);
MG_Object_LL* MG_object_get_all_orphans(MG_Instance* instance);
MG_Object_LL* MG_object_get_all_with_flags(MG_Instance* instance, uint32_t flags);
MG_Object_LL* MG_object_get_all_with_component(MG_Instance* instance, MG_ComponentTypes type);
MG_Object_LL* MG_object_get_all_with_component_flags(MG_Instance* instance, uint32_t flags);

// frees the memory used by the component linked list. this does not call the component's on_destroy functions.
void MG_object_free_components(MG_Object* object);

int MG_object_delete(MG_Instance* instance, uint64_t id);
int MG_object_delete_by_ptr(MG_Instance* instance, MG_Object* object);
int MG_object_delete_non_recursive(MG_Instance* instance, uint64_t id);
int MG_object_delete_all_with_flags(MG_Instance* instance, uint32_t flags);
int MG_object_delete_all_with_component(MG_Instance* instance, MG_ComponentTypes type);
int MG_object_delete_all_with_component_flags(MG_Instance* instance, uint32_t flags);