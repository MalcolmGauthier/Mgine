#pragma once

#include "MG_include.h"
#include "stc/MG_Scene.h"

#define _MG_default_tex_width 16
#define _MG_default_tex_height 16
extern uint8_t _MG_default_texture[_MG_default_tex_width * _MG_default_tex_height * 3];

MG_Texture* MG_texture_init(const char* path, uint32_t index_in_file);
int MG_texture_load(MG_Texture* texture);
