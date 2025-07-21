#include "MG_logic.h"

// The main logic loop of the game engine. This processes game logic at a fixed tick rate.
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

	while (game_data->instance->active)
	{
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

			if (object->on_tick)
			{
				object->on_tick(object);
			}

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

			if (object->flags & MG_OBJECT_FLAG_MARKED_FOR_DELETION)
			{
				MG_delete_object(game_data->instance, object->id);
			}

			current = current->next;
		}

		if (game_data->tickrate > 0)
		{
			uint64_t counter_ticks_per_game_tick = timer_frequency / game_data->tickrate;
			uint64_t time_remaining = counter_ticks_per_game_tick - (SDL_GetPerformanceCounter() - last_tick_time);
			
			// sleep as much as possible to reduce CPU usage
			uint32_t sleep_ms = (uint32_t)((time_remaining * 1000) / timer_frequency);
			if (sleep_ms > 1) SDL_Delay(sleep_ms - 1);
				
			while ((SDL_GetPerformanceCounter() - last_tick_time) < counter_ticks_per_game_tick);
		}

		last_tick_time = SDL_GetPerformanceCounter();
		game_data->global_timer++;

		if (game_data->tickrate <= 0)
		{
			game_data->delta_time = (float)(SDL_GetPerformanceCounter() - last_tick_time) / timer_frequency;
		}
	}

	return 0;
}
