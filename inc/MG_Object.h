#pragma once

#include "MG_include.h"
#include "stc/MG_object.h"
#include "MG_component.h"

uint64_t MG_object_create(MG_Instance* instance, MG_Object* parent, uint32_t flags, void (*on_load)(MG_Object*), void (*on_tick)(MG_Object*));
uint64_t MG_object_create_by_copy(MG_Object* object);
uint64_t MG_object_create_with_parent(MG_Object* parent_object, uint32_t flags, void (*on_load)(MG_Object*), void (*on_tick)(MG_Object*));
// create a copy of the object that is not tracked by the engine.
// this returns a new object with the same data, but does not add it to the object list, and thus the object is not managed by the engine.
MG_Object* MG_object_create_untracked_copy(MG_Object* source);

MG_Object* MG_object_get_by_id(MG_Instance* instance, uint64_t id);
MG_Object_LL* MG_object_get_all(MG_Instance* instance);
// returns a linked list of all top-level objects (objects without a parent)
MG_Object_LL* MG_object_get_all_top_level(MG_Instance* instance);
MG_Object_LL* MG_object_get_all_with_flags(MG_Instance* instance, uint32_t flags);
MG_Object_LL* MG_object_get_all_with_component(MG_Instance* instance, uint32_t type);
MG_Object_LL* MG_object_get_all_with_component_flags(MG_Instance* instance, uint32_t flags);

int MG_object_add_child(MG_Object* parent, MG_Object* child);
int MG_object_remove_child(MG_Object* parent, uint64_t child_id);

int MG_object_add_component(MG_Object* object, MG_Component* component);
int MG_object_remove_component(MG_Object* object, uint32_t type);
MG_Component* MG_object_get_component(MG_Object* object, uint32_t type);
MG_Component_LL* MG_object_get_all_components(MG_Object* object);

// frees the memory used by the component linked list. this does not call the component's on_destroy functions.
void MG_object_free_components(MG_Object* object);

int MG_object_delete(MG_Instance* instance, uint64_t id);
// returns void for compatibility with LL functions
void MG_object_delete_by_ptr(MG_Object* object);
int MG_object_delete_non_recursive(MG_Instance* instance, uint64_t id);
int MG_object_delete_all_with_flags(MG_Instance* instance, uint32_t flags);
int MG_object_delete_all_with_component(MG_Instance* instance, uint32_t type);
int MG_object_delete_all_with_component_flags(MG_Instance* instance, uint32_t flags);