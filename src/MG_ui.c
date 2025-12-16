#include "MG_ui.h"

MG_Texture* MG_UI_texture_from_text(const char* font_name, const wchar_t* text, int fonst_size, byte font_style, MG_Vec4 color)
{
    if (!font_name || !text)
        return NULL;

    TTF_Font* font = TTF_OpenFont(MG_UI_find_font_file_windows(font_name), fonst_size);
    if (!font)
        return NULL;

    TTF_SetFontStyle(font, font_style);

    SDL_Color sdl_color = MG_color_from_vec4(color);

    char* utf8 = SDL_iconv_string(
        "UTF-8",
        sizeof(wchar_t) == 2 ? "UTF-16" : "UTF-32",
        (const char*)text,
        (size_t)-1
    );

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

    MG_Texture* tex = MG_texture_init(NULL, 0);
    tex->base.asset_file_data = surface->pixels;
	tex->base.asset_file_size = (size_t)surface->w * surface->h * surface->format->BytesPerPixel;
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

    const char* fontDir = "C:\\Windows\\Fonts\\";
    WIN32_FIND_DATAA data;
    HANDLE h;

    char searchPath[MAX_PATH];
    snprintf(searchPath, sizeof(searchPath), "%s*.ttf", fontDir);

    h = FindFirstFileA(searchPath, &data);
    if (h == INVALID_HANDLE_VALUE)
        return NULL;

    do
    {
        // case-insensitive substring match
        if (!StrStrIA(data.cFileName, font_name))
            continue;

        char* fullPath = malloc(MAX_PATH);
        if (fullPath)
        {
            snprintf(fullPath, MAX_PATH, "%s%s",
                fontDir, data.cFileName);
        }
        FindClose(h);
        return fullPath;
    }
    while (FindNextFileA(h, &data));

    FindClose(h);
    return NULL;
}