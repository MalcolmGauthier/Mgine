#pragma once

#include "MG_include.h"
#include "stc/MG_object.h"
#include "MG_components.h"
#include "MG_transform.h"

#ifdef __cplusplus
extern "C" {
#endif

// Creates an empty object managed by the specified Mgine instance.
// Empty objects do not do anything. They are invisible, have no location, and run no code. Components must be added to give them functionality or appearence.
// The parent paramter lets you attach this object to a parent. 
// Objects can use the information from their parents to influence their actions, such as moving with them with the transform component. 
// Setting parent parameter to 0 will make this object a top-level object.
// The name will let you retreive the object by name in the future. Statically allocate the name, it's not freed later. Unique names are required to avoid confusion on retreival.
// The flags let you specify extra object properties. These flags are found in the MG_ObjectFlags enum in stc/MG_object.h.
MG_API MG_OBJ MG_object_create(MG_OBJ parent, const char* name, MG_ObjectFlags flags);
MG_API MG_OBJ MG_object_create_by_copy(MG_OBJ object);
// create a copy of the object that is not tracked by the engine.
// this returns a new object with the same data, but does not add it to the object list, and thus the object is not managed by the engine.
// This is a safer alternative to getting an object's data as a struct, but the user is responsible for freeing its memory with MG_object_delete_by_ptr.
MG_API MG_Object* MG_object_create_untracked_copy(MG_OBJ source);
// same as before, but manages the copy. this is mainly for MG_LL_copy usage.
MG_API MG_OBJ MG_object_create_tracked_copy(MG_OBJ source);

MG_API MG_OBJ MG_object_get(const char* name);
// Not recommended for use by user. Use at your own risk, modifications performed without API calls are unsupported and might lead to errors now or later with an engine update.
MG_API MG_Object* MG_object_ptr(MG_OBJ id);
// returns a linked list of all top-level objects (objects without a parent)
MG_API MG_OBJ_ARRAY MG_object_get_all_top_level();
MG_API MG_OBJ_ARRAY MG_object_get_all_with_flags(MG_ObjectFlags flags);
MG_API MG_OBJ_ARRAY MG_object_get_all_with_component(MG_ID type);
MG_API MG_OBJ_ARRAY MG_object_get_all_with_component_flags(MG_ObjectFlags flags);

MG_API int MG_object_add_child(MG_OBJ parent, MG_OBJ child);
MG_API int MG_object_remove_child(MG_OBJ parent, MG_OBJ child_id);

MG_API int MG_object_add_component(MG_OBJ object, const char* component_name);
MG_API int MG_object_remove_component(MG_OBJ object, MG_ID type);

MG_API MG_Component* MG_object_get_component(MG_ComponentInstanceID component_id);
MG_API MG_Component* MG_object_get_component_by_name(MG_OBJ object, const char* name);
MG_API MG_Component_LL* MG_object_get_all_components(MG_OBJ object);

MG_API MG_Vec3 MG_object_get_world_position(MG_OBJ object);
MG_API MG_Vec3 MG_object_get_world_rotation(MG_OBJ object);
MG_API MG_Vec3 MG_object_get_world_scale(MG_OBJ object);

// frees the memory used by the component linked list. this DOES call the component's on_destroy functions...
//TODO: do i need this?
MG_API void MG_object_free_components(MG_Object* object);

MG_API void MG_object_delete(MG_OBJ object);
// returns void for compatibility with LL functions
MG_API int MG_object_delete_non_recursive(MG_OBJ object);
MG_API int MG_object_delete_all_with_flags(MG_ObjectFlags flags);
MG_API int MG_object_delete_all_with_component(MG_ID type);
MG_API int MG_object_delete_all_with_component_flags(MG_ObjectFlags flags);

#ifdef __cplusplus
}
#endif