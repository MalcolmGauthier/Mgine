#pragma once
#include <SDL_mixer.h>
#include "MG_LL.h"
#include "MG_Transform.h"
#include "MG_Asset.h"
#include "MG_id.h"

// data for a sound file
typedef struct MG_Sound
{
	MG_Asset base;

	MG_ID id;
	int (*update)(struct MG_Audio* audio, struct MG_Sound* self);
}
MG_Sound;

// a loaded (or intended to be loaded) sound effect
typedef struct MG_SFX
{
	MG_Sound* sound;
	SDL_RWops* sdl_mem;
	Mix_Chunk* chunk;
	int sdl_channel;

	int loops;
	int8_t volume;

	MG_Vec3 position;
	MG_Vec3* position_ref;
}
MG_SFX;

typedef MG_Generic_LL MG_SFX_LL;

// all the audio info, part of an instance
typedef struct MG_Audio
{
	struct MG_Instance* instance;

	MG_Sound* music_sound;
	MG_Sound* cued_music_sound;
	Mix_Music* music;
	bool music_playing;
	bool loop_music;
	int8_t music_volume;

	bool stereo;
	uint8_t stereo_intensity;

	MG_Vec3* ears;
	float* ears_yaw;

	MG_SFX_LL* sfx_list;
}
MG_Audio;