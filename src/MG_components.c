#include "MG_components.h"

MG_ComponentTemplate* MG_component_register(MG_GameData* game_data, size_t struct_size, const char* name,
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

	comp_template->id = MG_component_get_id_by_name(name);
	comp_template->size = struct_size;

	return comp_template;
}

#pragma section(".MGREG$A", read)
#pragma section(".MGREG$Z", read)
__declspec(allocate(".MGREG$A")) MG_DeferredRegFunc __MGreg_start = 0;
__declspec(allocate(".MGREG$Z")) MG_DeferredRegFunc __MGreg_end = 0;
MG_COMPONENT(MG_ComponentTransform, "Transform", NULL, MG_transformcomponent_on_update, NULL)
MG_COMPONENT(MG_ComponentModel, "Model", NULL, NULL, NULL)

void MG_initialize_components(MG_GameData* game_data)
{
	//extern MG_DeferredRegFunc __MGreg_start, __MGreg_end;
	for (MG_DeferredRegFunc* f = &__MGreg_start + 1; f < &__MGreg_end; ++f)
		if (*f) (*f)(game_data);
}


uint32_t MG_component_get_id_by_name(const char* name)
{
	if (!name)
	{
		printf("ERROR: MG_component_get_id: name is NULL\n");
		return 0;
	}
	
	// FNV-1a hash
	uint32_t hash = 2166136261u;
	while (*name)
	{
		hash ^= (uint8_t)(toupper(*name));
		hash *= 16777619u;
		name++;
	}
	return hash;
}


MG_Component* MG_component_create(MG_Object* object, MG_ComponentTemplate* template, void* data)
{
	MG_Component* component = calloc(1, sizeof(template->size));
	if (!component)
	{
		printf("ERROR: out of memory, unable to allocate new component");
		return NULL;
	}

	component->base = template;
	component->owner = object;

	// copy everything except first 2 pointers
	size_t offset = offsetof(MG_Component, flags);
	size_t size = sizeof(template->size) - offset;
	memcpy_s((char*)component + offset, size, (char*)data + offset, size);

	MG_LL_add(object->components, component);

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
		MG_LL_add(dst_parent->components, component);
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

	free(component);
}