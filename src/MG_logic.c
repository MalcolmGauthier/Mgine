#include "MG_logic.h"

// The main logic loop of the game engine
int MG_logic_loop(void* MG_instance)
{
	if (!MG_instance)
	{
		printf("Logic loop crash: instance is NULL\n");
		return -1;
	}

	MG_GameData* game_data = &((MG_Instance*)MG_instance)->game_data;

	if (game_data->tickrate > 0)
		game_data->delta_time = 1.0f / game_data->tickrate;
	else
		game_data->delta_time = FLT_EPSILON;

	uint64_t last_tick_time = SDL_GetPerformanceCounter();
	uint64_t timer_frequency = SDL_GetPerformanceFrequency();
	uint64_t current_time;

	while (game_data->instance->active)
	{
		MG_input_poll_pressed(&game_data->instance->window_data);

		MG_Object_LL* current = game_data->object_list;
		MG_Object* object = NULL;
		while (current)
		{
			object = (MG_Object*)current->data;

			if (!object)
			{
				current = current->next;
				continue;
			}

			// render thread only gets to interact with objects between every object process
			// this waits until render thread is done copying object data
			while (game_data->instance->lock_owner == MG_GAME_DATA_LOCK_OWNER_RENDER_THREAD);
			game_data->instance->lock_owner = MG_GAME_DATA_LOCK_OWNER_LOGIC_THREAD;

			if (object->components)
			{
				MG_Component_LL* current_comp = object->components;
				MG_Component* component = NULL;
				while (current_comp)
				{
					component = (MG_Component*)current_comp->data;

					if (component && component->type == MG_COMPONENT_TYPE_SCRIPT)
					{
						component->on_update(component, component->data, game_data->delta_time);
					}
					current_comp = current_comp->next;
				}
			}

			if (object->on_tick)
			{
				object->on_tick(object);
			}

			if (object->flags & MG_OBJECT_FLAG_MARKED_FOR_DELETION)
			{
				MG_delete_object(game_data->instance, object->id);
			}

			game_data->instance->lock_owner = MG_GAME_DATA_LOCK_OWNER_NONE;

			current = current->next;
		}

		// reset mouse relative position and scroll
		MG_input_poll_mouse_relative_pos(&game_data->instance->window_data, NULL, NULL);
		MG_input_poll_mouse_scroll(&game_data->instance->window_data, NULL);

		// not necessary, just making sure
		game_data->instance->lock_owner = MG_GAME_DATA_LOCK_OWNER_NONE;

		current_time = SDL_GetPerformanceCounter();

		if (game_data->tickrate > 0)
		{
			uint64_t counter_ticks_per_game_tick = timer_frequency / game_data->tickrate;
			uint64_t time_remaining = counter_ticks_per_game_tick - (current_time - last_tick_time);
			
			// sleep as much as possible to reduce CPU usage
			uint32_t sleep_ms = (uint32_t)((time_remaining * 1000) / timer_frequency);
			if (sleep_ms > 1) SDL_Delay(sleep_ms - 1);
				
			while ((current_time - last_tick_time) < counter_ticks_per_game_tick);
		}

		game_data->delta_time = (float)(current_time - last_tick_time) / timer_frequency;

		last_tick_time = current_time;
		game_data->global_timer++;
	}

	return 0;
}

void MG_logic_free(MG_GameData* game_data)
{
	if (!game_data) return;
	// free all objects
	MG_Object_LL* current = game_data->object_list;
	MG_Object_LL* next = NULL;
	while (current)
	{
		next = current->next;
		MG_object_free_components((MG_Object*)current->data);
		free(current->data);
		free(current);
		current = next;
	}
	game_data->object_list = NULL;
	game_data->object_count = 0;
	game_data->instance->lock_owner = MG_GAME_DATA_LOCK_OWNER_NONE;
}
