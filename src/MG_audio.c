#include "MG_audio.h"

static MG_Sound* MG_audio_get_sound(const char* sfx_name);
static MG_SFX* MG_audio_create_sfx(MG_SOUND sound, MG_Vec3 position, MG_Vec3* position_ref);
static int MG_audio_load_sfx(MG_SOUND sfx);
static inline void MG_audio_start_sfx(MG_SOUND sfx);

int MG_audio_play_sfx(const char* sfx_name)
{
	if (!sfx_name)
	{
		printf("Error: Cannot play sound. Audio or SFX name is NULL.\n");
		return -1;
	}

	MG_Sound* sound = MG_audio_get_sound(sfx_name);
	if (!sound)
	{
		printf("Error: Cannot play sound. SFX '%s' not found.\n", sfx_name);
		return -1;
	}
	
	//TODO: rename to create/add when cache added
	MG_SFX* sfx = MG_audio_create_sfx(sound, (MG_Vec3){0}, MG_INSTANCE->audio_data.ears);
	if (!sfx)
	{
		printf("Error: Cannot play sound. Failed to create SFX for '%s'.\n", sfx_name);
		return -2;
	}

	MG_audio_start_sfx(sfx);
	return 0;
}

static MG_Sound* MG_audio_get_sound(const char* sfx_name)
{
	if (!sfx_name)
	{
		printf("Error: Cannot get SFX. SFX name is NULL.\n");
		return NULL;
	}

	MG_ID id = MG_ID_hash_string(sfx_name);
	//TODO: implement caching
	for (uint32_t i = 0; i < MG_INSTANCE->sound_count; i++)
	{
		if (MG_INSTANCE->sound_list[i]->id == id)
			return MG_INSTANCE->sound_list[i];
	}

	return NULL;
}

//TODO
void MG_audio_cache_sfx(MG_Sound* sound)
{
	UNUSED(sound)
}


static MG_SFX* MG_audio_create_sfx(MG_SOUND sound, MG_Vec3 position, MG_Vec3* position_ref)
{
	if (!sound)
	{
		printf("Error: Cannot create SFX. Sound is NULL.\n");
		return NULL;
	}

	MG_SFX* sfx = calloc(1, sizeof(MG_SFX));
	if (!sfx)
	{
		printf("Error: Failed to allocate memory for SFX.\n");
		return NULL;
	}

	sfx->sound = sound;
	sfx->position = position;
	sfx->position_ref = position_ref;

	MG_asset_add(&MG_INSTANCE->sound_list, &MG_INSTANCE->sound_count, sound);

	if (!sound->base.asset_file_loaded || !sound->base.loaded)
	{
		if (MG_audio_load_sfx(sfx))
		{
			printf("Error: failed to load sound effect asset: %u.", sound->id);
			return NULL;
		}
	}

	return sfx;
}

static int MG_audio_load_sfx(MG_SOUND sfx)
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

static inline void MG_audio_start_sfx(MG_SOUND sfx)
{
	Mix_PlayChannel(-1, sfx->chunk, sfx->loops);
	Mix_Volume(sfx->sdl_channel, sfx->volume);
}


static void MG_audio_free_sfx(MG_SFX* sfx)
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

		MG_audio_free_sfx(sfx);
		current->data = NULL;
		break;
	}
}

void MG_audio_free()
{
	Mix_PauseAudio(1);
	MG_LL_free(&MG_INSTANCE->audio_data.sfx_list, MG_audio_free_sfx);
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