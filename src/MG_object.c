#include "MG_object.h"

uint64_t MG_object_create(MG_Instance* instance, MG_Object* parent, uint32_t flags, void (*on_load)(MG_Object*), void (*on_tick)(MG_Object*))
{
	MG_Object* object = calloc(1, sizeof(MG_Object));
	MG_Object_LL* new_node = malloc(sizeof(MG_Object_LL));
	if (!object || !new_node)
	{
		printf("Failed to allocate memory for object\n");
		if (object) free(object);
		if (new_node) free(new_node);
		return -1;
	}

	object->instance = instance;
	object->parent = parent;
	object->flags = flags;
	object->id = instance->game_data.next_object_id;
	instance->game_data.next_object_id++;
	object->on_load = on_load;
	object->on_tick = on_tick;

	new_node->data = object;
	new_node->next = NULL;
	instance->game_data.object_count++;
	if (!instance->game_data.object_list)
	{
		instance->game_data.object_list = new_node;
	}
	else
	{
		MG_Object_LL* current = instance->game_data.object_list;
		while (current->next) current = current->next;
		current->next = new_node;
	}

	if (object->on_load)
	{
		object->on_load(object);
	}

	return object->id;
}

uint64_t MG_object_create_by_copy(MG_Object* object)
{
	if (!object)
	{
		printf("Failed to copy object: object is NULL\n");
		return -1;
	}

	// [TODO] copy linked lists
	return MG_create_object(object->instance, object->parent, object->flags, object->on_load, object->on_tick);
}

uint64_t MG_object_create_with_parent(MG_Object* parent_object, uint32_t flags, void (*on_load)(MG_Object*), void (*on_tick)(MG_Object*))
{
	if (!parent_object)
	{
		printf("Failed to create object: parent is NULL\n");
		return -1;
	}

	return MG_create_object(parent_object->instance, parent_object, flags, on_load, on_tick);
}

MG_Object* MG_object_create_untracked_copy(MG_Object* source)
{
	if (!source)
	{
		printf("Failed to create untracked copy: source is NULL\n");
		return NULL;
	}

	MG_Object* object = calloc(1, sizeof(MG_Object));
	if (!object)
	{
		printf("Failed to allocate memory for untracked object copy\n");
		return NULL;
	}
	memcpy_s(object, sizeof(MG_Object), source, sizeof(MG_Object));
	object->children = NULL;
	object->components = NULL;

	if (source->children)
		object->children = MG_LL_Copy(source->children, MG_object_create_untracked_copy);
	if (source->components)
		object->components = MG_LL_Copy(source->components, MG_component_copy_untracked);

	return object;
}


MG_Object* MG_object_get_by_id(MG_Instance* instance, uint64_t id)
{
	if (!instance)
	{
		printf("Failed to get object: instance is NULL\n");
		return NULL;
	}

	MG_Object_LL* current = instance->game_data.object_list;
	while (current)
	{
		if (((MG_Object*)current->data)->id == id)
		{
			return current->data;
		}
		current = current->next;
	}

	printf("Failed to get object with ID %u: not found\n", (uint32_t)id);
	return NULL;
}

MG_Object_LL* MG_object_get_all(MG_Instance* instance)
{
	if (!instance)
	{
		printf("Failed to get all objects: instance is NULL\n");
		return NULL;
	}

	return instance->game_data.object_list;
}

MG_Object_LL* MG_object_get_all_top_level(MG_Instance* instance)
{
	if (!instance)
	{
		printf("Failed to get top-level objects: instance is NULL\n");
		return NULL;
	}

	MG_Object_LL* top_level_objects = NULL;
	MG_Object_LL* current = instance->game_data.object_list;
	while (current)
	{
		if (!((MG_Object*)current->data)->parent)
		{
			MG_LL_Add(&top_level_objects, current->data);
		}
		current = current->next;
	}

	return top_level_objects;
}


int MG_object_add_child(MG_Object* parent, MG_Object* child)
{
	if (!parent || !child)
	{
		printf("Failed to add child: parent or child is NULL\n");
		return -1;
	}

	// check to see if adding child would cause infinite loop
	MG_Object* current = parent;
	while (current->parent)
	{
		if (current->parent == child)
		{
			printf("Failed to add child: child is parent of new parent\n");
			return -2;
		}

		current = current->parent;
	}


	if (child->parent)
	{
		MG_object_remove_child(child->parent, child->id);
	}

	child->parent = parent;
	MG_LL_Add(&parent->children, child);
	return 0;
}

int MG_object_remove_child(MG_Object* parent, uint64_t child_id)
{
	if (!parent)
	{
		printf("Failed to remove child: parent is NULL\n");
		return -1;
	}

	if (MG_LL_Remove(parent->children, MG_object_get_by_id(parent->instance, child_id)))
		return 0;

	printf("Failed to remove child with ID %u: not found\n", (uint32_t)child_id);
	return -2;
}


void MG_object_free_components(MG_Object* object)
{
	if (!object)
	{
		printf("Failed to free components: object is NULL\n");
		return;
	}

	MG_LL_Free(object->components, MG_component_free);
	object->components = NULL;
}


int MG_object_delete(MG_Instance* instance, uint64_t id)
{
	if (!instance)
	{
		printf("Failed to delete object: instance is NULL\n");
		return -1;
	}

	MG_Object_LL* current = instance->game_data.object_list;
	MG_Object_LL* previous = NULL;
	MG_Object* object = NULL;
	while (current)
	{
		object = (MG_Object*)current->data;

		if (object->id == id)
		{
			if (previous)
			{
				previous->next = current->next;
			}
			else
			{
				instance->game_data.object_list = current->next;
			}

			instance->game_data.object_count--;

			// delete children recursively
			while (object->children && object->children->data)
			{
				MG_Object* child = (MG_Object*)object->children->data;
				object->children = object->children->next;
				MG_delete_object(instance, child->id);
			}

			// call on_destroy for all components
			while (object->components && object->components->data)
			{
				MG_Component* component = (MG_Component*)object->components->data;
				if (component->on_destroy)
				{
					component->on_destroy(component);
				}
				object->components = object->components->next;
			}

			MG_LL_Free(object->children, MG_object_delete_by_ptr);
			MG_LL_Free(object->components, MG_component_free);
			free(object);
			free(current);
			return 0;
		}

		previous = current;
		current = current->next;
	}

	return 1;
}

void MG_object_delete_by_ptr(MG_Object* object)
{
	if (!object)
	{
		printf("Failed to delete object: instance or object is NULL\n");
		return;
	}

	MG_object_delete(object->instance, object->id);
}

int MG_object_delete_non_recursive(MG_Instance* instance, uint64_t id)
{
	if (!instance)
	{
		printf("Failed to delete object: instance is NULL\n");
		return -1;
	}

	MG_Object* object = MG_get_object_by_id(instance, id);
	if (!object)
	{
		printf("Failed to delete object with ID %llu: not found\n", id);
		return -2;
	}

	// set the parent of all the children to the parent of the object being deleted
	// create temp object to avoid memory leak of LL nodes
	MG_Object_LL* current = object->children;
	while (current && current->data)
	{
		MG_Object* child = (MG_Object*)current->data;
		current = current->next;
		child->parent = object->parent;
	}

	// delete childless object
	return MG_delete_object(instance, id);
}