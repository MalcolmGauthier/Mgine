#pragma once

#include "MG_include.h"
#include "MG_window.h"
#include "MG_logic.h"
#include "MG_render.h"
#include "MG_file.h"
#include "MG_audio.h"

#ifdef __cplusplus
extern "C" {
#endif

// Creates a new instance of the engine, using the memory location of out_instance.
// This can also be used to create multiple instances at once, but that requires alot of memory and processing power, sowhatever you're doing better be worth it.
// It is highly advised to not lose track of the pointer out_instance, and to make sure to free it by changing the active boolean.
// 
// no_window: when set to true, it will make this instance invisible and in the background.
// 
// sub_instance: when set to true, it will indicate to the setup code to not initialize audio. the audio system used by the engine can only handle a single
// instance at once. creating two main instances could mess up the audio.
MG_API int MG_create_instance(MG_Instance* out_instance, bool no_window, bool sub_instance);

// Quits the engine by setting the active boolean to false.
MG_API void MG_quit(MG_Instance* instance);

#ifdef __cplusplus
}
#endif