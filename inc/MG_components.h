#pragma once
#include "stc/MG_component.h"
#include "MG_include.h"
#include "MG_transform.h"
#include "MG_object.h"
#include "MG_id.h"

#ifdef __cplusplus
extern "C" {
#endif

MG_API MG_ComponentTemplate* MG_component_register(size_t struct_size, const char* name,
	MG_ComponentFuncResult(*on_create)(struct MG_Component* self),
	MG_ComponentFuncResult(*on_update)(struct MG_Component* self, float delta_time),
	void (*on_destroy)(struct MG_Component* self));

MG_API void MG_initialize_components();


MG_API MG_Component* MG_component_get(MG_ComponentInstanceID component);


MG_API MG_ComponentTemplate* MG_component_get_template_ptr(MG_ID id);
MG_API MG_ID MG_component_get_template(const char* name);

// creates component in dst_parent of the same type as src's type, but also copies over all the data currently in src.
MG_API MG_ComponentInstanceID MG_component_copy(MG_ComponentInstanceID src, MG_ID dst_parent);

MG_API void MG_component_free(MG_Component* component);

#ifdef __cplusplus
}
#endif