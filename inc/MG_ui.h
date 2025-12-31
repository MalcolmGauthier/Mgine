#pragma once

#include "MG_include.h"
#include "stc/MG_Scene.h"
#include "MG_textures.h"
#include "MG_transform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	MG_FONT_STYLE_NORMAL = TTF_STYLE_NORMAL,
	MG_FONT_STYLE_BOLD = TTF_STYLE_BOLD,
	MG_FONT_STYLE_ITALIC = TTF_STYLE_ITALIC,
	MG_FONT_STYLE_UNDERLINE = TTF_STYLE_UNDERLINE,
	MG_FONT_STYLE_STRIKETHROUGH = TTF_STYLE_STRIKETHROUGH
}
MG_FontStyle;

MG_API MG_Texture* MG_UI_texture_from_text(const char* font_name, const wchar_t* text, int font_size, MG_FontStyle font_style, MG_Vec4 color);

MG_API char* MG_UI_find_font_file_windows(const char* font_name);

#ifdef __cplusplus
}
#endif