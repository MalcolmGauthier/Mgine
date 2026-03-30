#pragma once

#include "MG_include.h"
#include "MG_window.h"
#include "MG_logic.h"
#include "MG_render.h"
#include "MG_file.h"
#include "MG_audio.h"
#include "MG_scene.h"
#include "MG_model.h"
#include "MG_textures.h"

#ifdef __cplusplus
extern "C" {
#endif

// Creates a new instance of the engine, stored internally.
// The user should wait until instance->initialized is true to do anything. This can also be done by calling MG_ready(), which does exactly that.
// When done with the engine, call MG_quit().
MG_API int MG_init();

// Loops infinitely, waiting until the instance is initialized and ready to be used, or when it closes after a bootup error.
// Ideally, this should be called immediately after MG_init().
MG_API void MG_ready();

// Quits the engine safely, freeing all the memory.
MG_API void MG_quit();

#ifdef __cplusplus
}
#endif