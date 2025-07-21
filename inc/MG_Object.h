#pragma once

#include "MG_include.h"
#include "stc/MG_object.h"

uint64_t MG_create_object(MG_Instance* instance, MG_Object* parent, uint32_t flags, void (*on_load)(MG_Object*), void (*on_tick)(MG_Object*));
uint64_t MG_create_object_by_copy(MG_Object* object);
uint64_t MG_create_object_with_parent(MG_Object* parent_object, uint32_t flags, void (*on_load)(MG_Object*), void (*on_tick)(MG_Object*));

MG_Object* MG_get_object_by_id(MG_Instance* instance, uint64_t id);
MG_Object_LL* MG_get_all_objects(MG_Instance* instance);
MG_Object_LL* MG_get_all_object_orphans(MG_Instance* instance);
MG_Object_LL* MG_get_all_objects_with_flags(MG_Instance* instance, uint32_t flags);
MG_Object_LL* MG_get_all_objects_with_component(MG_Instance* instance, MG_ComponentTypes type);
MG_Object_LL* MG_get_all_objects_with_component_flags(MG_Instance* instance, uint32_t flags);

int MG_delete_object(MG_Instance* instance, uint64_t id);
int MG_delete_object_by_ptr(MG_Instance* instance, MG_Object* object);
int MG_delete_object_non_recursive(MG_Instance* instance, uint64_t id);
int MG_delete_all_objects_with_flags(MG_Instance* instance, uint32_t flags);
int MG_delete_all_objects_with_component(MG_Instance* instance, MG_ComponentTypes type);
int MG_delete_all_objects_with_component_flags(MG_Instance* instance, uint32_t flags);