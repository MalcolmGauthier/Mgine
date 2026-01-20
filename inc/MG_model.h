#pragma once

#include "MG_include.h"
#include "stc/MG_Model.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#ifdef __cplusplus
extern "C" {
#endif

MG_API int MG_model_init(const char* path);

MG_API int MG_model_init_MGA(const char* path, int32_t index_in_file);

MG_API int MG_model_load(MG_Model* model);

MG_API void MG_model_enable(MG_Model* model, bool static_model);

#ifdef __cplusplus
}
#endif