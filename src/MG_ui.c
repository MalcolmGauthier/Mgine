#include "MG_ui.h"

MG_Texture* MG_UI_texture_from_text(const char* font_name, const wchar_t* text, int font_size, MG_FontStyle font_style, MG_Vec4 color)
{
    if (!font_name || !text)
        return NULL;

    TTF_Font* font = TTF_OpenFont(MG_UI_find_font_file_windows(font_name), font_size);
    if (!font)
        return NULL;

    TTF_SetFontStyle(font, font_style);

    SDL_Color sdl_color = MG_color_from_vec4(color);

    char* utf8 = SDL_iconv_wchar_utf8(text);
    if (!utf8)
    {
        TTF_CloseFont(font);
        return NULL;
    }
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, utf8, sdl_color);
    free(utf8);

    if (!surface)
    {
        TTF_CloseFont(font);
        return NULL;
    }

    // create managed texture struct and manually set its data
    MG_Texture* tex = MG_texture_init_MGA(NULL, 0);
    tex->base.asset_file_data = surface->pixels;
	tex->base.asset_file_size = (size_t)surface->w * (size_t)surface->h * surface->format->BytesPerPixel;
    if (MG_texture_load(tex))
        return NULL;

    SDL_FreeSurface(surface);
    TTF_CloseFont(font);

    return tex;
}

char* MG_UI_find_font_file_windows(const char* font_name)
{
    if (!font_name)
        return NULL;

    const char* font_dir = "C:\\Windows\\Fonts\\";
    char* full_path = malloc(MAX_PATH);
    if (!full_path) return NULL;

    WIN32_FIND_DATAA data;
    HANDLE h;

	// check for exact match first
    snprintf(full_path, MAX_PATH, "%s%s", font_dir, data.cFileName);
    if (PathFileExistsA(full_path))
    {
        return full_path;
    }

    // next, case-insensitive substring match
    char searchPath[MAX_PATH];
    snprintf(searchPath, sizeof(searchPath), "%s*.ttf", font_dir);

    h = FindFirstFileA(searchPath, &data);
    if (h == INVALID_HANDLE_VALUE)
        return NULL;

    do
    {
        if (!StrStrIA(data.cFileName, font_name))
            continue;

        snprintf(full_path, MAX_PATH, "%s%s", font_dir, data.cFileName);
        FindClose(h);
        return full_path;
    }
    while (FindNextFileA(h, &data));

    FindClose(h);
    return NULL;
}