#pragma once

#include <shlwapi.h>
#include "MG_include.h"
#include "stc/MG_Scene.h"
#include "MG_textures.h"
#include "MG_transform.h"

MG_Texture* MG_UI_texture_from_text(const char* fontName, const wchar_t* text, int fontSize, byte fontStyle, MG_Vec4 color);

char* MG_UI_find_font_file_windows(const char* font_name);
