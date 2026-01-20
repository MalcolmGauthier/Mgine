#pragma once
#include "stc/MG_component.h"
#include "MG_include.h"
#include "MG_transform.h"
#include "MG_object.h"
#include "MG_id.h"

#ifdef __cplusplus
extern "C" {
#endif

MG_API MG_ComponentTemplate* MG_component_register(MG_Instance* instance, size_t struct_size, const char* name,
	MG_ComponentFuncResult(*on_create)(struct MG_Component* self),
	MG_ComponentFuncResult(*on_update)(struct MG_Component* self, float delta_time),
	void (*on_destroy)(struct MG_Component* self));

MG_API void MG_initialize_components(MG_Instance* game_data);


MG_API MG_ComponentTemplate* MG_component_get_template_by_id(MG_Instance* instance, MG_ID id);
MG_API MG_ComponentTemplate* MG_component_get_template_by_name(MG_Instance* instance, const char* name);

MG_API MG_Component* MG_component_create(MG_Object* object, MG_ComponentTemplate* comp_template, void* data);

MG_API MG_Component* MG_component_create_copy(MG_Component* src, MG_Object* dst_parent);
// creates a copy of a component instance with no attached object. used internally for object copying. 
// use not recommended. either manually attach to object or free after use.
MG_API MG_Component* MG_component_copy_untracked(MG_Component* source);

MG_API void MG_component_free(MG_Component* component);

#ifdef __cplusplus
}
#endif