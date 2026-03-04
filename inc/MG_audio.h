#pragma once

#include "MG_include.h"
#include "stc/MG_Audio.h"
#include "stc/MG_Asset.h"
#include "MG_file.h"

#ifdef __cplusplus
extern "C" {
#endif

MG_API void MG_audio_free_channel(int channel);
MG_API void MG_audio_free(MG_Audio* audio);

#ifdef __cplusplus
}
#endif