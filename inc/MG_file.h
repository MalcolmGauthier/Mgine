#pragma once

#include <stdio.h>
#include "MG_include.h"
#include "stc/MG_Instance.h"
#include "stc/MG_Scene.h"
#include "MG_shaders.h"
#include "MG_object.h"
#include "stc/MG_Audio.h"

// pass NULL to file to open new file descriptor
byte* MG_asset_load(FILE* file, MG_Asset* asset);

// free asset data, and just the data.
void MG_asset_free(MG_Asset* asset);

// loads the game data from file into the instance
int MG_load_game(MG_Instance* instance);