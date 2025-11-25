#include "MG_audio.h"

MG_Audio* mg_audio = NULL;

int MG_audio_play_sfx(MG_Audio* audio, const char* sfx_name, int loops, int8_t volume)
{
	if (!audio || !sfx_name)
	{
		printf("Error: Cannot play sound. Audio or SFX name is NULL.\n");
		return -1;
	}

	MG_Sound* sound = MG_audio_get_sound(audio, sfx_name);
	if (!sound)
	{
		printf("Error: Cannot play sound. SFX '%s' not found.\n", sfx_name);
		return -1;
	}
	
	//TODO: rename to create/add when cache added
	MG_SFX* sfx = MG_audio_create_sfx(audio, sound, (MG_Vec3){0}, audio->ears);
	if (!sfx)
	{
		printf("Error: Cannot play sound. Failed to create SFX for '%s'.\n", sfx_name);
		return -2;
	}
}

int MG_audio_play_sfx_3D(MG_Audio* audio, const char* sfx_name, MG_Vec3 position, int loops, int8_t volume);
int MG_audio_play_sfx_3D_ref(MG_Audio* audio, const char* sfx_name, MG_Vec3* position, int loops, int8_t volume);

MG_Sound* MG_audio_get_sound(MG_Audio* audio, const char* sfx_name)
{
	if (!audio || !sfx_name)
	{
		printf("Error: Cannot get SFX. Audio or SFX name is NULL.\n");
		return NULL;
	}

	MG_ID id = MG_ID_get_id(sfx_name);
	//TODO: implement caching
	for (int i = 0; i < audio->instance->sound_count; i++)
	{
		if (audio->instance->sound_list[i].id == id)
			return &audio->instance->sound_list[i];
	}

	return NULL;
}

static MG_SFX* MG_audio_create_sfx(MG_Audio* audio, MG_Sound* sound, MG_Vec3 position, MG_Vec3* position_ref)
{
	if (!audio || !sound)
	{
		printf("Error: Cannot create SFX. Audio or SFX is NULL.\n");
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

	MG_LL_add(&audio->sfx_list, sfx);

	return sfx;
}

static int MG_audio_load_sfx(MG_Audio* audio, MG_SFX* sfx)
{
	if (!audio || !sfx)
	{
		printf("Error: Cannot get SFX. Audio or SFX is NULL.\n");
		return -1;
	}

	if (sfx->sound->base.loaded)
		return 0;

	if (!sfx->sound->base.asset_file_data || !sfx->sound->base.asset_file_loaded)
	{
		sfx->sound->base.asset_file_data = MG_load_asset(NULL, &sfx->sound->base);
		if (!sfx->sound->base.asset_file_data)
		{
			printf("Error: Failed to load SFX asset data for SFX ID %u\n", sfx->sound->id);
			return -2;
		}
	}

	sfx->sdl_mem = SDL_RWFromMem(sfx->sound->base.asset_file_data, sfx->sound->base.asset_file_size);
	if (!sfx->sdl_mem)
	{
		printf("Error: Failed to convert sound file to SDL RWop. SDL_Error: %s", SDL_GetError());
		return NULL;
	}

	// do NOT free RWops. needed for playback.
	sfx->chunk = Mix_LoadWAV_RW(sfx->sdl_mem, 0);
	if (!sfx->chunk)
	{
		printf("Error: Failed to convert SDL RWop to WAV.");
		return NULL;
	}

	sfx->sound->base.loaded = true;
	return 0;
}

void MG_audio_free_channel(int channel)
{
	//Mix_Chunk* chunk = Mix_GetChunk(channel);
	
	//Mix_UnregisterAllEffects(channel);
}