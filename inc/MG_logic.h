#pragma once

#include "MG_include.h"
#include "MG_window.h"
#include "stc/MG_Instance.h"
#include "MG_object.h"

#ifdef __cplusplus
extern "C" {
#endif

MG_API int MG_logic_loop(void* instance);

MG_API void MG_logic_free(MG_GameData* game_data);

#ifdef __cplusplus
}
#endif