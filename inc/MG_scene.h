#pragma once

#include "MG_include.h"
#include "stc/MG_Instance.h"
#include "stc/MG_Scene.h"

#ifdef __cplusplus
extern "C" {
#endif

MG_API void MG_scene_load(MG_Instance* instance, uint32_t scene_index);

MG_API void MG_scene_free(MG_Scene* scene);

#ifdef __cplusplus
}
#endif