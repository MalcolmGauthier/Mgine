#include "MG_components.h"

MG_ComponentTemplate* MG_component_register(size_t struct_size, const char* name,
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

	comp_template->id = MG_ID_hash_string(name);
	comp_template->on_create = on_create;
	comp_template->on_tick = on_update;
	comp_template->on_destroy = on_destroy;
	comp_template->size = struct_size;

	if (MG_INSTANCE->component_list)
	{
		MG_ComponentTemplate_LL* temp_ll = MG_INSTANCE->component_list;
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

	MG_LL_add(&MG_INSTANCE->component_list, comp_template);
	
	return comp_template;
}

#pragma section(".MGREG$A", read)
#pragma section(".MGREG$Z", read)
__declspec(allocate(".MGREG$A")) MG_DeferredRegFunc __MGreg_start = 0;
__declspec(allocate(".MGREG$Z")) MG_DeferredRegFunc __MGreg_end = 0;
MG_COMPONENT(MG_ComponentTransform, "Transform", NULL, MG_transformcomponent_on_update, NULL)
MG_COMPONENT(MG_ComponentModel, "Model", NULL, NULL, NULL)

void MG_initialize_components()
{
	//extern MG_DeferredRegFunc __MGreg_start, __MGreg_end;
	for (MG_DeferredRegFunc* f = &__MGreg_start + 1; f < &__MGreg_end; f++)
		if (*f) (*f)();
}

MG_ComponentTemplate* MG_component_get_template_ptr(MG_ID id)
{
	MG_ComponentTemplate_LL* temp_ll = MG_INSTANCE->component_list;
	while (temp_ll)
	{
		MG_ComponentTemplate* temp = (MG_ComponentTemplate*)temp_ll->data;
		if (temp->id == id)
			return temp;
		temp_ll = temp_ll->next;
	}
	return NULL;
}

MG_ComponentTemplate* MG_component_get_template_by_name(const char* name)
{
	MG_ID id = MG_ID_hash_string(name);
	return MG_component_get_template_ptr(id);
}


MG_Component* MG_component_copy(MG_ComponentInstanceID src, MG_OBJ dst_parent)
{
	// dst_parent allowed to be null for untracked copies
	MG_Component* src_comp = MG_object_get_component(src);
	if (!src_comp)
	{
		printf("Failed to create component copy: source/parent is NULL\n");
		return NULL;
	}

	// msize is windows-specific, but who tf cares. id rather be vendor locked than trust the template size
	MG_Component* dst_comp = calloc(1, _msize(src_comp));
	if (!dst_comp)
	{
		printf("Failed to allocate memory for component copy\n");
		return NULL;
	}

	memcpy(dst_comp, src_comp, _msize(src_comp));

	if (dst_parent)
	{
		MG_Object* obj = MG_object_ptr(dst_parent);
		MG_LL_add(&obj->components, dst_comp);
	}
	dst_comp->id.owner = dst_parent;
	return dst_comp;
}

// ???????????
//MG_Component* MG_component_copy_untracked(MG_ComponentInstanceID source)
//{
//	return MG_component_create(source, NULL);
//}


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