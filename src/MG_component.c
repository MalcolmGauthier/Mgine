#include "MG_component.h"

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

	if (component->data)
	{
		free(component->data);
	}
	free(component);
}