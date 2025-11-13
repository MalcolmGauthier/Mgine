#pragma once

#include <stdio.h>
#include "MG_include.h"
#include "stc/MG_Instance.h"
#include "stc/MG_Scene.h"
#include "MG_shaders.h"
#include "MG_object.h"

byte* MG_load_asset(FILE* file, const char* path, uint32_t index);
int MG_load_game(MG_Instance* instance);