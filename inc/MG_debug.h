#pragma once

#include "stc/MG_Instance.h"
#include "MG_include.h"
#include "MG_transform.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CENTRE SDL_MAX_SINT32
#define NO_SCROLL SDL_MAX_SINT32
#define BLANC 0xFFFFFF
#define ROUGE 0xFF0000
#define VERT 0x00FF00
#define BLEU 0x0000FF
#define NOIR 0x000000
#define OPAQ 255
#define LARGEUR_DEFAUT 5.0f
#define HAUTEUR_DEFAUT 10.0f
#define ESPACE_DEFAUT 3.0f

MG_API void MG_debug_panic_text(MG_Instance* instance, char* text, MG_Vec2 position, float size, uint32_t color_rgba);

#ifdef __cplusplus
}
#endif