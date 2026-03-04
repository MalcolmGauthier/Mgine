#pragma once

#include <stdio.h>
#include "MG_include.h"
#include "stc/MG_Instance.h"
#include "stc/MG_Scene.h"
#include "MG_shaders.h"
#include "MG_object.h"
#include "stc/MG_Audio.h"

#ifdef __cplusplus
extern "C" {
#endif

// takes in pointer to asset list in MG_Instance struct, pointer to the count of that asset list, and pointer to asset to add
MG_API int MG_asset_add(void** asset_list_ref, uint32_t* asset_count_ref, void* asset);

// pass NULL as file to open new file descriptor
MG_API byte* MG_asset_load(FILE* file, MG_Asset* asset);

// free asset data. does NOT free the asset file name, it will assume you used a constant string. does not free itself.
MG_API void MG_asset_free(MG_Asset* asset);

// loads the game data from file into the instance
MG_API int MG_load_game(MG_Instance* instance);

#ifdef __cplusplus
}
#endif