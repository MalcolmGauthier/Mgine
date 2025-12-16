#include "MG_debug.h"

static const char* stupid_debug_shader_v = "#version 330 core \
layout(location = 0) in vec2 aPos; \
void main() \
{ \
    gl_Position = vec4((aPos.x / screenWidth) * 2.0 - 1.0, 1.0 - (aPos.y / screenHeight) * 2.0, 0.0, 1.0 ); \
}";
static const char* stupid_debug_shader_f = "#version 330 core \
out vec4 FragColor; \
uniform vec4 uColor; \
void main() \
{ \
    FragColor = uColor; \
}";
const static int16_t char_draw_info_starting_indexes[] = {
    1, 18, 47, 60, 81, 98, 111, 132, 145, 158, 171, 184, 193, 210, 223, 240, // a - p
    257, 278, 299, 320, 329, 342, 351, 368, 377, 386, 399, 420, 433, 454, 471, // q - 4
    484, 513, 534, 543, 564, 585, 590, 599, 604, 609, 630, 651, 676, 697, 706, // 5 - ??
    711, 716, 721, 734, 747
};
#define CHAR_DRAW_INFO_LEN 756
const static char char_draw_info[CHAR_DRAW_INFO_LEN] = {
    127, // space/unknown

    0, 10, 0, 0, // a
    0, 0, 5, 0,
    5, 0, 5, 10,
    5, 5, 0, 5, 127,

    0, 10, 0, 0, // b
    0, 0, 5, 0,
    5, 0, 5, 3,
    5, 3, 0, 5,
    0, 5, 5, 7,
    5, 7, 5, 10,
    5, 10, 0, 10, 127,

    0, 10, 0, 0, // c
    0, 0, 5, 0,
    5, 10, 0, 10, 127,

    0, 10, 0, 0, // d
    0, 0, 2, 0,
    2, 10, 0, 10,
    2, 10, 5, 5,
    2, 0, 5, 5, 127,

    0, 10, 0, 0, // e
    0, 0, 5, 0,
    5, 10, 0, 10,
    5, 5, 0, 5, 127,

    0, 10, 0, 0, // f
    0, 0, 5, 0,
    5, 5, 0, 5, 127,

    0, 10, 0, 0, // g
    0, 0, 5, 0,
    5, 10, 0, 10,
    5, 10, 5, 5,
    5, 5, 3, 5, 127,

    0, 10, 0, 0, // h
    5, 0, 5, 10,
    5, 5, 0, 5, 127,

    0, 0, 5, 0, // i
    2, 0, 2, 10,
    5, 10, 0, 10, 127,

    5, 0, 5, 10, // j
    5, 10, 0, 10,
    0, 10, 0, 7, 127,

    0, 10, 0, 0, // k
    0, 5, 5, 0,
    0, 5, 5, 10, 127,

    0, 10, 0, 0, // l
    5, 10, 0, 10, 127,

    0, 10, 0, 0, // m
    5, 0, 5, 10,
    0, 0, 2, 5,
    5, 0, 2, 5, 127,

    0, 10, 0, 0, // n
    5, 0, 5, 10,
    0, 0, 5, 10, 127,

    0, 10, 0, 0, // o
    5, 0, 5, 10,
    0, 0, 5, 0,
    5, 10, 0, 10, 127,

    0, 10, 0, 0, // p
    0, 0, 5, 0,
    5, 0, 5, 5,
    5, 5, 0, 5, 127,

    0, 10, 0, 0, // q
    4, 0, 4, 10,
    0, 0, 4, 0,
    4, 10, 0, 10,
    5, 10, 3, 5, 127,

    0, 10, 0, 0, // r
    0, 0, 5, 0,
    5, 0, 5, 5,
    5, 5, 0, 5,
    2, 5, 5, 10, 127,

    0, 0, 5, 0, // s
    5, 10, 0, 10,
    5, 5, 0, 5,
    0, 0, 0, 5,
    5, 10, 5, 5, 127,

    2, 0, 2, 10, // t
    0, 0, 5, 0, 127,

    0, 10, 0, 0, // u
    5, 0, 5, 10,
    5, 10, 0, 10, 127,

    0, 0, 2, 10, // v
    5, 0, 2, 10, 127,

    0, 10, 0, 0, // w
    5, 0, 5, 10,
    5, 10, 0, 10,
    2, 10, 2, 4, 127,

    0, 0, 5, 10, // x
    5, 0, 0, 10, 127,

    5, 0, 0, 10, // y
    0, 0, 2, 5, 127,

    0, 0, 5, 0, // z
    5, 10, 0, 10,
    5, 0, 0, 10, 127,

    5, 0, 0, 10, // 0
    0, 10, 0, 0,
    5, 0, 5, 10,
    0, 0, 5, 0,
    5, 10, 0, 10, 127,

    0, 3, 2, 0, // 1
    2, 0, 2, 10,
    5, 10, 0, 10, 127,

    0, 3, 1, 0, // 2
    1, 0, 4, 0,
    5, 3, 4, 0,
    5, 3, 0, 10,
    5, 10, 0, 10, 127,

    5, 10, 5, 0, // 3
    0, 0, 5, 0,
    5, 10, 0, 10,
    5, 5, 0, 5, 127,

    0, 5, 0, 0, // 4
    5, 0, 5, 10,
    5, 5, 0, 5, 127,

    5, 0, 0, 0, // 5
    0, 5, 0, 0,
    5, 5, 0, 5,
    5, 5, 5, 8,
    5, 8, 4, 10,
    4, 10, 1, 10,
    1, 10, 0, 8, 127,

    0, 10, 0, 0, // 6
    0, 0, 5, 0,
    5, 10, 0, 10,
    5, 10, 5, 5,
    5, 5, 0, 5, 127,

    5, 0, 0, 0, // 7
    5, 0, 0, 10, 127,

    5, 5, 0, 5, // 8
    0, 10, 0, 0,
    5, 0, 5, 10,
    0, 0, 5, 0,
    5, 10, 0, 10, 127,

    5, 10, 5, 0, // 9
    0, 0, 5, 0,
    5, 10, 0, 10,
    0, 0, 0, 5,
    5, 5, 0, 5, 127,

    0, 10, 0, 10, 127, // .

    0, 3, 0, 3, // :
    0, 7, 0, 7, 127,

    2, 8, 0, 10, 127, // ,

    2, 2, 0, 0, 127, // '

    0, 10, 0, 0, // e acc. aigu
    0, 0, 5, 0,
    5, 10, 0, 10,
    5, 5, 0, 5,
    1, -2, 4, -4, 127,

    0, 10, 0, 0, // e acc. grave
    0, 0, 5, 0,
    5, 10, 0, 10,
    5, 5, 0, 5,
    1, -4, 4, -2, 127,

    0, 10, 0, 0, // e acc. circ.
    0, 0, 5, 0,
    5, 10, 0, 10,
    5, 5, 0, 5,
    1, -2, 2, -4,
    4, -2, 2, -4, 127,

    0, 10, 0, 0, // a acc. grave
    0, 0, 5, 0,
    5, 0, 5, 10,
    5, 5, 0, 5,
    1, -2, 4, 0, 127,

    2, 0, 2, 3, // "
    4, 0, 4, 3, 127,

    1, 5, 4, 5, 127, // -

    0, 10, 5, 0, 127, // /

    5, 10, 0, 0, 127, // \

    4, 0, 2, 3, // (
    2, 3, 2, 7,
    4, 10, 2, 7, 127,

    1, 0, 3, 3, // )
    3, 3, 3, 7,
    1, 10, 3, 7, 127,

    0, 5, 4, 5, // +
    2, 3, 2, 7, 127
};

static int16_t GetListEntry(char c)
{
    if (c >= 'a' && c <= 'z')
        return char_draw_info_starting_indexes[c - 'a'];
    else if (c >= '0' && c <= '9')
        return char_draw_info_starting_indexes[c - '0' + 26];
    else
    {
        switch (c)
        {
        case '.':
            return char_draw_info_starting_indexes[36];
        case ':':
            return char_draw_info_starting_indexes[37];
        case ',':
            return char_draw_info_starting_indexes[38];
        case '\'':
            return char_draw_info_starting_indexes[39];
        case '\x82':
            return char_draw_info_starting_indexes[40];
        case '\x8A':
            return char_draw_info_starting_indexes[41];
        case '\x89':
            return char_draw_info_starting_indexes[42];
        case '\x86':
            return char_draw_info_starting_indexes[43];
        case '"':
            return char_draw_info_starting_indexes[44];
        case '-':
            return char_draw_info_starting_indexes[45];
        case '/':
            return char_draw_info_starting_indexes[46];
        case '\\':
            return char_draw_info_starting_indexes[47];
        case '(':
            return char_draw_info_starting_indexes[48];
        case ')':
            return char_draw_info_starting_indexes[49];
        case '+':
            return char_draw_info_starting_indexes[50];
        }
    }

    return 0;
}

void MG_debug_panic_text(MG_Instance* instance, char* text, MG_Vec2 position, float size, int color, int alpha, int scroll)
{
    if (scroll <= 0)
        return;

    if (alpha <= 0)
        return;

    size_t strlen = SDL_strlen(text);

    char* working_text = (char*)SDL_malloc(strlen + 1);
    SDL_memcpy(working_text, text, strlen + 1);
    for (int i = 0; working_text[i]; i++)
    {
        working_text[i] = SDL_tolower(working_text[i]);
    }

    int32_t extra_y = 0;
    int32_t return_length = 0;

    if (scroll < 0)
        scroll = 0;
    else if (scroll > strlen)
        scroll = (int)strlen;

    if (alpha < 0)
        alpha = 0;
    else if (alpha > SDL_MAX_UINT8)
        alpha = SDL_MAX_UINT8;

    if (position.x == CENTRE)
        position.x = (MG_W_WIDTH / 2) - ((LARGEUR_DEFAUT + ESPACE_DEFAUT) * size * strlen - 1) / 2;

    if (position.y == CENTRE)
        position.y = (MG_W_HEIGHT / 2) - (HAUTEUR_DEFAUT * size) / 2;

	glColorP4ui(GL_UNSIGNED_INT_8_8_8_8_REV, (uint32_t)((alpha << 24) | (color & 0xFFFFFF)));
    glColor4f(
        ((color >> 16) & 0xFF) / 255.0f,
        ((color >> 8) & 0xFF) / 255.0f,
        (color & 0xFF) / 255.0f,
        alpha / 255.0f
    );

    glBegin(GL_LINES);

    float x, y;
    int current_info_index;

    for (int i = 0; i < scroll; i++)
    {
        y = position.y + extra_y;
        x = position.x + i * (LARGEUR_DEFAUT + ESPACE_DEFAUT) * size - return_length;

        if (x + size * LARGEUR_DEFAUT * 4 > MG_W_WIDTH)
        {
            extra_y += (int32_t)((HAUTEUR_DEFAUT + ESPACE_DEFAUT) * size);
            return_length = (int32_t)((i + 1) * 8 * size);
        }

        if (working_text[i] == '\0')
            break;

        if (working_text[i] == '\n')
        {
            extra_y += (int32_t)((HAUTEUR_DEFAUT + ESPACE_DEFAUT) * size);
            return_length = (int32_t)((i + 1) * (LARGEUR_DEFAUT + ESPACE_DEFAUT) * size);
            continue;
        }

        current_info_index = GetListEntry(working_text[i]);

        while (current_info_index < CHAR_DRAW_INFO_LEN - 3)
        {
            if (char_draw_info[current_info_index] == 127)
                break;

            glVertex2f(
                char_draw_info[current_info_index] * size + x,
                char_draw_info[current_info_index + 1] * size + y
            );

            glVertex2f(
                char_draw_info[current_info_index + 2] * size + x,
                char_draw_info[current_info_index + 3] * size + y
            );

            current_info_index += 4;
        }
    }

    glEnd();

    SDL_free(working_text);
}