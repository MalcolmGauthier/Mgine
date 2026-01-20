#include "MG_components.h"

MG_ComponentTemplate* MG_component_register(MG_Instance* instance, size_t struct_size, const char* name,
	MG_ComponentFuncResult(*on_create)(struct MG_Component* self),
	MG_ComponentFuncResult(*on_update)(struct MG_Component* self, float delta_time),
	void (*on_destroy)(struct MG_Component* self)
)
{
	MG_ComponentTemplate* comp_template = calloc(1, sizeof(MG_ComponentTemplate));
	if (!comp_template)
	{
		printf("ERROR: out of memory, unable to register new component");
		return NULL;
	}

	comp_template->id = MG_ID_get_id(name);
	comp_template->on_create = on_create;
	comp_template->on_tick = on_update;
	comp_template->on_destroy = on_destroy;
	comp_template->size = struct_size;

	if (instance->component_list)
	{
		MG_ComponentTemplate_LL* temp_ll = instance->component_list;
		while (temp_ll)
		{
			if (((MG_ComponentTemplate*)temp_ll->data)->id == comp_template->id)
			{
				printf("ERROR: component with name '%s' already registered\n", name);
				free(comp_template);
				return NULL;
			}
			temp_ll = temp_ll->next;
		}
	}

	MG_LL_add(&instance->component_list, comp_template);
	
	return comp_template;
}

#pragma section(".MGREG$A", read)
#pragma section(".MGREG$Z", read)
__declspec(allocate(".MGREG$A")) MG_DeferredRegFunc __MGreg_start = 0;
__declspec(allocate(".MGREG$Z")) MG_DeferredRegFunc __MGreg_end = 0;
MG_COMPONENT(MG_ComponentTransform, "Transform", NULL, MG_transformcomponent_on_update, NULL)
MG_COMPONENT(MG_ComponentModel, "Model", NULL, NULL, NULL)

void MG_initialize_components(MG_Instance* instance)
{
	//extern MG_DeferredRegFunc __MGreg_start, __MGreg_end;
	for (MG_DeferredRegFunc* f = &__MGreg_start + 1; f < &__MGreg_end; ++f)
		if (*f) (*f)(instance);
}

MG_ComponentTemplate* MG_component_get_template_by_id(MG_Instance* instance, MG_ID id)
{
	MG_ComponentTemplate_LL* temp_ll = instance->component_list;
	while (temp_ll)
	{
		MG_ComponentTemplate* temp = (MG_ComponentTemplate*)temp_ll->data;
		if (temp->id == id)
			return temp;
		temp_ll = temp_ll->next;
	}
	return NULL;
}

MG_ComponentTemplate* MG_component_get_template_by_name(MG_Instance* instance, const char* name)
{
	MG_ID id = MG_ID_get_id(name);
	return MG_component_get_template_by_id(instance, id);
}

MG_Component* MG_component_create(MG_Object* object, MG_ComponentTemplate* comp_template, void* data)
{
	MG_Component* component = calloc(1, sizeof(comp_template->size));
	if (!component)
	{
		printf("ERROR: out of memory, unable to allocate new component");
		return NULL;
	}

	component->base = comp_template;
	component->owner = object;

	// copy everything except first 2 pointers
	size_t offset = offsetof(MG_Component, flags);
	size_t size = sizeof(comp_template->size) - offset;
	if (size > 0 && data)
		memcpy_s((char*)component + offset, size, (char*)data + offset, size);

	MG_LL_add(&object->components, component);

	if (component->base->on_create)
		component->base->on_create(component);

	return component;
}

MG_Component* MG_component_create_copy(MG_Component* src, MG_Object* dst_parent)
{
	// dst_parent allowed to be null for untracked copies
	if (!src)
	{
		printf("Failed to create component copy: source/parent is NULL\n");
		return NULL;
	}

	MG_Component* component = calloc(1, _msize(src));
	if (!component)
	{
		printf("Failed to allocate memory for component copy\n");
		return NULL;
	}

	// msize is windows-specific, but who cares at this point. better than trusting the template size
	memcpy(component, src, _msize(src));

	if (dst_parent)
	{
		MG_LL_add(&dst_parent->components, component);
	}
	component->owner = dst_parent;
	return component;
}

MG_Component* MG_component_copy_untracked(MG_Component* source)
{
	return MG_component_create_copy(source, NULL);
}


void MG_component_free(MG_Component* component)
{
	if (!component) return;

	if (component->base->on_destroy)
	{
		component->base->on_destroy(component);
	}

	// also frees extra data used by component
	free(component);
}