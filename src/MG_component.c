#include "MG_component.h"

MG_ComponentTemplate* MG_component_register(MG_GameData* game_data, size_t struct_size,
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
	comp_template->id = game_data->next_component_id++;

	if (comp_template->id < 0)
	{
		printf("WARNING: component ID overflow. IDs will be reused.\n");
		game_data->next_component_id = 1;
		comp_template->id = 0;
	}

	return comp_template;
}

MG_Component* MG_component_create(MG_Object* object, MG_ComponentTemplate* template)
{
	MG_Component* component = calloc(1, sizeof(template->size));
	if (!component)
	{
		printf("ERROR: out of memory, unable to allocate new component");
		return NULL;
	}

	component->base = template;
	component->owner = object;

	MG_LL_Add(object->components, component);

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
		MG_LL_Add(dst_parent->components, component);
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