#include "MG_audio.h"

MG_Audio* mg_audio = NULL;

static MG_Sound* MG_audio_get_sound(MG_Audio* audio, const char* sfx_name);
static MG_SFX* MG_audio_create_sfx(MG_Audio* audio, MG_Sound* sound, MG_Vec3 position, MG_Vec3* position_ref);
static int MG_audio_load_sfx(MG_Audio* audio, MG_SFX* sfx);
static inline void MG_audio_start_sfx(MG_Audio* audio, MG_SFX* sfx);

int MG_audio_play_sfx(MG_Audio* audio, const char* sfx_name, int loops)
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

	MG_audio_start_sfx(audio, sfx);
	return 0;
}

int MG_audio_play_sfx_3D(MG_Audio* audio, const char* sfx_name, MG_Vec3 position, int loops, int8_t volume);
int MG_audio_play_sfx_3D_ref(MG_Audio* audio, const char* sfx_name, MG_Vec3* position, int loops, int8_t volume);

static MG_Sound* MG_audio_get_sound(MG_Audio* audio, const char* sfx_name)
{
	if (!audio || !sfx_name)
	{
		printf("Error: Cannot get SFX. Audio or SFX name is NULL.\n");
		return NULL;
	}

	MG_ID id = MG_ID_get_id(sfx_name);
	//TODO: implement caching
	for (uint32_t i = 0; i < audio->instance->sound_count; i++)
	{
		if (audio->instance->sound_list[i].id == id)
			return &audio->instance->sound_list[i];
	}

	return NULL;
}

void MG_audio_cache_sfx(MG_Sound* sound)
{

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

	if (!sound->base.asset_file_loaded || !sound->base.loaded)
	{
		if (MG_audio_load_sfx(audio, sfx))
		{
			printf("Error: failed to load sound effect asset: %u.", sound->id);
			return NULL;
		}
	}

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

static inline void MG_audio_start_sfx(MG_Audio* audio, MG_SFX* sfx)
{
	Mix_PlayChannel(-1, sfx->chunk, sfx->loops);
	Mix_Volume(sfx->sdl_channel, sfx->volume);
}

static void MG_audio_free_channel(int channel)
{
	MG_SFX_LL* current = mg_audio->sfx_list;
	while (current)
	{
		MG_SFX* sfx = (MG_SFX*)current->data;
		if (!sfx || sfx->sdl_channel != channel)
		{
			current = current->next;
			continue;
		}

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
		current->data = NULL;
		break;
	}
}