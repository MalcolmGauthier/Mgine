#include "MG_logic.h"

int MG_logic_loop(void* MG_instance)
{
	if (!MG_instance)
	{
		printf("Logic loop crash: instance is NULL\n");
		return -1;
	}

	MG_GameData* game_data = &((MG_Instance*)MG_instance)->game_data;
	game_data->delta_time = 1.0f / game_data->tickrate;

	uint64_t last_tick_time = SDL_GetPerformanceCounter();
	uint64_t timer_frequency = SDL_GetPerformanceFrequency();

	while (game_data->instance->active)
	{
		struct MG_Object_LL* current = game_data->object_list;
		while (current)
		{
			if (current->object && current->object->on_tick)
			{
				current->object->on_tick(current->object);
			}
			current = current->next;
		}

		while ((SDL_GetPerformanceCounter() - last_tick_time) < timer_frequency / game_data->tickrate);
		last_tick_time = SDL_GetPerformanceCounter();
		game_data->global_timer++;
	}

	return 0;
}

int MG_create_object(MG_Instance* instance, MG_Component root_component, uint32_t flags, void (*on_load)(MG_Object*), void (*on_tick)(MG_Object*))
{
	MG_Object* object = calloc(1, sizeof(MG_Object));
	struct MG_Object_LL* new_node = malloc(sizeof(struct MG_Object_LL));
	if (!object || !new_node)
	{
		printf("Failed to allocate memory for object\n");
		if (object) free(object);
		if (new_node) free(new_node);
		return -1;
	}

	object->instance = instance;
	object->self = root_component;
	object->flags = flags;
	object->id = instance->game_data.next_object_id;
	instance->game_data.next_object_id++;
	object->on_load = on_load;
	object->on_tick = on_tick;

	new_node->object = object;
	new_node->next = NULL;
	instance->game_data.object_count++;
	if (!instance->game_data.object_list)
	{
		instance->game_data.object_list = new_node;
	}
	else
	{
		struct MG_Object_LL* current = instance->game_data.object_list;
		while (current->next) current = current->next;
		current->next = new_node;
	}

	if (object->on_load)
	{
		object->on_load(object);
	}

	return object->id;
}

int MG_create_object_by_copy(MG_Object* object)
{
	if (!object)
	{
		printf("Failed to copy object: object is NULL\n");
		return -1;
	}

	return MG_create_object(object->instance, object->self, object->flags, object->on_load, object->on_tick);
}

MG_Object* MG_get_object_by_id(MG_Instance* instance, uint32_t id)
{
	if (!instance)
	{
		printf("Failed to get object: instance is NULL\n");
		return NULL;
	}

	struct MG_Object_LL* current = instance->game_data.object_list;
	while (current)
	{
		if (current->object->id == id)
		{
			return current->object;
		}
		current = current->next;
	}

	printf("Failed to get object with ID %u: not found\n", id);
	return NULL;
}

int MG_delete_object(MG_Instance* instance, uint32_t id)
{
	if (!instance)
	{
		printf("Failed to delete object: instance is NULL\n");
		return -1;
	}

	struct MG_Object_LL* current = instance->game_data.object_list;
	struct MG_Object_LL* previous = NULL;
	while (current)
	{
		if (current->object->id == id)
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
			free(current->object);
			free(current);
			return 0;
		}
		previous = current;
		current = current->next;
	}

	return 1;
}