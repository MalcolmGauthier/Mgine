#pragma once

#include "MG_include.h"
#include "stc/MG_object.h"
#include "MG_components.h"
#include "MG_transform.h"

#ifdef __cplusplus
extern "C" {
#endif

MG_API MG_ID MG_object_create(MG_Instance* instance, MG_Object* parent, uint32_t flags);
MG_API MG_ID MG_object_create_by_copy(MG_Object* object);
MG_API MG_ID MG_object_create_with_parent(MG_Object* parent_object, uint32_t flags);
// create a copy of the object that is not tracked by the engine.
// this returns a new object with the same data, but does not add it to the object list, and thus the object is not managed by the engine.
MG_API MG_Object* MG_object_create_untracked_copy(MG_Object* source);
// same as before, but manages the copy. this is mainly for MG_LL_copy usage.
MG_API MG_Object* MG_object_create_tracked_copy(MG_Object* source);

MG_API MG_Object* MG_object_get_by_id(MG_Instance* instance, MG_ID id);
MG_API MG_Object_LL* MG_object_get_all(MG_Instance* instance);
// returns a linked list of all top-level objects (objects without a parent)
MG_API MG_Object_LL* MG_object_get_all_top_level(MG_Instance* instance);
MG_API MG_Object_LL* MG_object_get_all_with_flags(MG_Instance* instance, uint32_t flags);
MG_API MG_Object_LL* MG_object_get_all_with_component(MG_Instance* instance, MG_ID type);
MG_API MG_Object_LL* MG_object_get_all_with_component_flags(MG_Instance* instance, uint32_t flags);

MG_API int MG_object_add_child(MG_Object* parent, MG_Object* child);
MG_API int MG_object_remove_child(MG_Object* parent, MG_ID child_id);

MG_API int MG_object_add_component(MG_Object* object, MG_Component* component);
MG_API int MG_object_remove_component(MG_Object* object, MG_ID type);

MG_API MG_Component* MG_object_get_component_by_name(MG_Object* object, const char* name);
MG_API MG_Component* MG_object_get_component_by_id(MG_Object* object, MG_ID id);
MG_API MG_Component_LL* MG_object_get_all_components(MG_Object* object);

MG_API MG_Vec3 MG_object_get_world_position(MG_Object* object);
MG_API MG_Matrix MG_object_get_world_transform_matrix(MG_Object* object);

// frees the memory used by the component linked list. this does not call the component's on_destroy functions.
MG_API void MG_object_free_components(MG_Object* object);

MG_API int MG_object_delete(MG_Instance* instance, MG_ID id);
// returns void for compatibility with LL functions
MG_API void MG_object_delete_by_ptr(MG_Object* object);
MG_API int MG_object_delete_non_recursive(MG_Instance* instance, MG_ID id);
MG_API int MG_object_delete_all_with_flags(MG_Instance* instance, uint32_t flags);
MG_API int MG_object_delete_all_with_component(MG_Instance* instance, uint32_t type);
MG_API int MG_object_delete_all_with_component_flags(MG_Instance* instance, uint32_t flags);

#ifdef __cplusplus
}
#endif