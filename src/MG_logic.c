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
			// this also means that a frame render could happen between any object processes
			// [TODO] see if this causes a problem
			while (game_data->instance->lock_owner == MG_GAME_DATA_LOCK_OWNER_RENDER_THREAD);
			game_data->instance->lock_owner = MG_GAME_DATA_LOCK_OWNER_LOGIC_THREAD;

			if (object->components)
			{
				MG_Component_LL* current_comp = object->components;
				MG_Component* component = NULL;
				while (current_comp)
				{
					component = (MG_Component*)current_comp->data;

					if (component && component->base->on_update)
					{
						component->base->on_update(component, game_data->delta_time);
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
				MG_object_delete(game_data->instance, object->id);
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
			// normally windows would not guarentee sleeping for less than 16ms, but this is taken care of in the program's intialization
			if (sleep_ms > 1) SDL_Delay(sleep_ms - 1);
				
			while ((SDL_GetPerformanceCounter() - last_tick_time) < counter_ticks_per_game_tick);
			current_time = SDL_GetPerformanceCounter();
		}

		game_data->delta_time = (float)(current_time - last_tick_time) / timer_frequency;
		game_data->uptime = current_time / (double)timer_frequency;

		last_tick_time = current_time;
		game_data->global_timer++;
	}

	game_data->instance->lock_owner = MG_GAME_DATA_LOCK_OWNER_NONE;
	return 0;
}

void MG_logic_free(MG_GameData* game_data)
{
	if (!game_data)
		return;

	MG_LL_Free(game_data->object_list, MG_object_delete_by_ptr);
	// at the moment this function is only used with copies and not pointers
	//free(game_data);
}
