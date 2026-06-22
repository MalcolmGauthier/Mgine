#include "MG_audio.h"

static MG_SFX* MG_audio_sfx_create(MG_SOUND sound, MG_Vec3 position, MG_Vec3* position_ref);
static int MG_audio_sfx_load(MG_SFX* sfx);
static inline void MG_audio_sfx_start(MG_SFX* sfx);

int MG_audio_sfx_play(MG_SOUND audio_asset)
{
	if (!audio_asset)
	{
		printf("Error: Cannot play sound. audio asset is invalid.\n");
		return -1;
	}
	
	//TODO: rename to create/add when cache added
	MG_SFX* sfx = MG_audio_sfx_create(audio_asset, (MG_Vec3){0}, MG_INSTANCE->audio_data.ears);
	if (!sfx)
	{
		printf("Error: Cannot play sound. Failed to create SFX for sound with ID '%u'.\n", audio_asset);
		return -2;
	}

	MG_audio_sfx_start(sfx);
	return 0;
}

MG_SOUND MG_audio_get_from_name(const char* sfx_name)
{
	if (!sfx_name)
	{
		printf("Error: Cannot get SFX. Inputted name is NULL.\n");
		return 0;
	}

	MG_Sound* ret = MG_hashmap_get(MG_INSTANCE->sound_list.assets, MG_id_hash_string(sfx_name));
	if (!ret)
	{
		printf("Error: Cannot get SFX. Sound asset with name '%s' not found.\n", sfx_name);
		return 0;
	}

	return ret->id;
}

MG_Sound* MG_audio_get_ptr(MG_SOUND sound)
{
	return MG_hashmap_get(MG_INSTANCE->sound_list.assets, sound);
}

//TODO
void MG_audio_sfx_cache(MG_Sound* sound)
{
	UNUSED(sound)
}


static MG_SFX* MG_audio_sfx_create(MG_SOUND sound, MG_Vec3 position, MG_Vec3* position_ref)
{
	if (!sound)
	{
		printf("Error: Cannot create SFX. Sound is invalid.\n");
		return NULL;
	}

	MG_SFX* sfx = calloc(1, sizeof(MG_SFX));
	if (!sfx)
	{
		printf("Error: Failed to allocate memory for SFX.\n");
		return NULL;
	}

	MG_Sound* sound_ptr = MG_audio_get_ptr(sound);
	if (!sound_ptr)
	{
		printf("Error: Cannot create SFX. Sound asset with ID %u not found.\n", sound);
		free(sfx);
		return NULL;
	}

	sfx->sound = sound_ptr;
	sfx->position = position;
	sfx->position_ref = position_ref;

	MG_LL_add(&MG_INSTANCE->audio_data.sfx_list, sfx);

	if (!sound_ptr->base.asset_file_loaded || !sound_ptr->base.loaded)
	{
		if (MG_audio_sfx_load(sfx))
		{
			printf("Error: failed to load sound effect asset: %u.", sound_ptr->id);
			MG_LL_remove(&MG_INSTANCE->audio_data.sfx_list, sfx);
			free(sfx);
			return NULL;
		}
	}

	return sfx;
}

static int MG_audio_sfx_load(MG_SFX* sfx)
{
	if (!sfx)
	{
		printf("Error: Cannot get SFX. SFX is NULL.\n");
		return -1;
	}

	if (sfx->sound->base.loaded)
		return 0;

	if (!sfx->sound->base.asset_file_data || !sfx->sound->base.asset_file_loaded)
	{
		sfx->sound->base.asset_file_data = MG_asset_load(NULL, &sfx->sound->base);
		if (!sfx->sound->base.asset_file_data)
		{
			printf("Error: Failed to load SFX asset data for SFX ID %u\n", sfx->sound->id);
			return -2;
		}
	}

	sfx->sdl_mem = SDL_RWFromMem(sfx->sound->base.asset_file_data, (int)sfx->sound->base.asset_file_size);
	if (!sfx->sdl_mem)
	{
		printf("Error: Failed to convert sound file to SDL RWop. SDL_Error: %s", SDL_GetError());
		return -3;
	}

	// free RWops. not needed for playback.
	sfx->chunk = Mix_LoadWAV_RW(sfx->sdl_mem, 1);
	sfx->sdl_mem = NULL;
	if (!sfx->chunk)
	{
		printf("Error: Failed to convert SDL RWop to WAV.");
		return -4;
	}

	MG_asset_free(&sfx->sound->base);
	sfx->sound->base.loaded = true;
	return 0;
}

static inline void MG_audio_sfx_start(MG_SFX* sfx)
{
	Mix_PlayChannel(-1, sfx->chunk, sfx->loops);
	Mix_Volume(sfx->sdl_channel, sfx->sound->volume);
}


static void MG_audio_sfx_free(MG_SFX* sfx)
{
	if (!sfx)
		return;

	if (sfx->chunk)
	{
		Mix_FreeChunk(sfx->chunk);
		sfx->chunk = NULL;
	}

	if (sfx->sdl_mem)
	{
		SDL_FreeRW(sfx->sdl_mem);
		sfx->sdl_mem = NULL;
	}

	free(sfx);
}

void MG_audio_free_channel(int channel)
{
	MG_SFX_LL* current = MG_INSTANCE->audio_data.sfx_list;
	while (current)
	{
		MG_SFX* sfx = (MG_SFX*)current->data;
		if (!sfx || sfx->sdl_channel != channel)
		{
			current = current->next;
			continue;
		}

		MG_audio_sfx_free(sfx);
		current->data = NULL;
		break;
	}
}

void MG_audio_free()
{
	Mix_PauseAudio(1);
	MG_LL_free(&MG_INSTANCE->audio_data.sfx_list, MG_audio_sfx_free);
	Mix_FreeMusic(MG_INSTANCE->audio_data.music);

	Mix_CloseAudio();
	Mix_Quit();
}

void MG_sound_free(MG_Sound* sound)
{
	if (!sound)
		return;

	MG_asset_free(&sound->base);
	free(sound);
	sound = NULL;
}