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
		return;
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
		return;
	}

	component->id = template->id;
	component->owner = object;
	component->on_create = template->on_create;
	component->on_update = template->on_update;
	component->on_destroy = template->on_destroy;

	MG_LL_Add(object->components, component);

	if (component->on_create)
		component->on_create(component);

	return component;
}


MG_Component* MG_component_create_copy(MG_Component* src, MG_Object* dst_parent)
{
	if (!src)
	{
		printf("Failed to create component copy: source/parent is NULL\n");
		return NULL;
	}

	MG_Component* component = calloc(1, sizeof(MG_Component));
	if (!component)
	{
		printf("Failed to allocate memory for component copy\n");
		return NULL;
	}
	memcpy_s(component, sizeof(MG_Component), src, sizeof(MG_Component));
	component->data = NULL;
	if (src->data && src->data_size > 0)
	{
		component->data = calloc(1, src->data_size);
		if (!component->data)
		{
			printf("Failed to allocate memory for component data copy\n");
			free(component);
			return NULL;
		}
		memcpy_s(component->data, src->data_size, src->data, src->data_size);
	}

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