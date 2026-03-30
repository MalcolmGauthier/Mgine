/* 
* 
* MGINE
* 3D GAME ENGINE
* BY MALCOLM GAUTHIER
* 
* started 2025-05-10
* 
*/

#pragma once

// one of the libraries breaks without this, i forget which one
#define _MATH_DEFINES_DEFINED

// windows defines must come first to prevent macro redefinition warnings
#ifdef _WIN32
#include <windows.h>
#include <timeapi.h>
#include <shlwapi.h>
#endif

// external libraries
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <glad.h>
#include <cglm.h>

// stdlib
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

#ifdef MG_EXPORT
#define MG_API __declspec(dllexport)
#else
#define MG_API __declspec(dllimport)
#endif

#define UNUSED(input) (input);

// the root of all data
struct MG_Instance* MG_INSTANCE;
//TODO
#ifdef MG_NO_PREFIX
#define instance MG_INSTANCE
#endif

///////////////////////////////
// ENGINE SETTINGS

#define MG_DEBUG true
#define MG_USE_MG_FILE false

// WINDOW
#define MG_W_NAME "Mgine prototype"
#define MG_W_WIDTH 800
#define MG_W_HEIGHT 600
#define MG_W_EXTRA_FLAGS SDL_WINDOW_BORDERLESS | SDL_WINDOW_RESIZABLE | 0/*SDL_WINDOW_INPUT_GRABBED*/
#define MG_W_MOUSE_SHOWN true
#define MG_W_MOUSE_GRABBED false
#define MG_W_NO_WINDOW false

// CONTROLS
#define MG_C_EXIT_ON_ESC true

// LOGIC
#define MG_L_TRICKRATE 60

// RENDERING
#define MG_R_OIT_ENABLED true
#define MG_R_INTERPOLATION_ENABLED true
#define MG_R_INTERPOLATION_PREDICTION false
#define MG_R_INTERPOLATION_TRIPLE_BUFFER false
#define MG_R_VERTEX_COLOR_ENABLED true
#define MG_R_BACKGROUND_REFRESH true

// AUDIO
#define MG_A_OUTPUT_FREQUENCY 44100 / 2
#define MG_A_CHANNEL_COUNT 32