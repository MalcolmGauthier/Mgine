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

#define _MATH_DEFINES_DEFINED

#include <SDL.h>
#include <glad/glad.h>
#include <cglm.h>

#include <stdio.h>
#include <stdbool.h>

///////////////////////////////
// DEFAULT SETTINGS

#define MG_DEBUG true

// WINDOW
#define MG_W_NAME "Mgine prototype"
#define MG_W_WIDTH 800
#define MG_W_HEIGHT 600
#define MG_W_EXTRA_FLAGS SDL_WINDOW_RESIZABLE | 0/*SDL_WINDOW_INPUT_GRABBED*/
#define MG_W_MOUSE_SHOWN true
#define MG_W_MOUSE_GRABBED false

// CONTROLS

// LOGIC
#define MG_L_TRICKRATE 60

// RENDERING
#define MG_R_OIT_ENABLED true