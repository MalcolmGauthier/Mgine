#pragma once

#include "MG_include.h"
#include "stc/MG_Audio.h"
#include "stc/MG_Asset.h"
#include "MG_file.h"

#ifdef __cplusplus
extern "C" {
#endif

MG_API int MG_audio_play_sfx(const char* sfx_name);

MG_API int MG_audio_play_sfx_3D(const char* sfx_name, MG_Vec3 position, int8_t volume);
MG_API int MG_audio_play_sfx_3D_ref(const char* sfx_name, MG_Vec3* position, int8_t volume);


MG_API void MG_audio_free_channel(int channel);

MG_API void MG_audio_free();

MG_API void MG_sound_free(MG_Sound* sound);

#ifdef __cplusplus
}
#endif