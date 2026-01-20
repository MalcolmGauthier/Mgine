#pragma once

#include "MG_include.h"
#include "stc/MG_Scene.h"
#include "MG_file.h"

#ifdef __cplusplus
extern "C" {
#endif

#define _MG_default_tex_width 16
#define _MG_default_tex_height 16
MG_API extern uint8_t _MG_default_texture[_MG_default_tex_width * _MG_default_tex_height * 3];

MG_API MG_Texture* MG_texture_init(const char* path);
MG_API MG_Texture* MG_texture_init_MGA(const char* path, int32_t index_in_file);
MG_API int MG_texture_load(MG_Texture* texture);

#ifdef __cplusplus
}
#endif