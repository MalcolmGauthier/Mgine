#pragma once

#include "MG_include.h"
#include "MG_file.h"
#include "stc/MG_Model.h"
// assimp c++ stuff breaks cuz of windows minmax macros
#undef min
// assimp c++ does wacky conversions with quats and doubles
#pragma warning(push)
#pragma warning(disable : 4244)
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#pragma warning(pop)

#ifdef __cplusplus
extern "C" {
#endif

MG_API MG_Model* MG_model_init(MG_Instance* instance, const char* path);

MG_API MG_Model* MG_model_init_raw(MG_Instance* instance);

MG_API MG_Model* MG_model_init_MGA(MG_Instance* instance, const char* path, int32_t index_in_file);

MG_API int MG_model_load(MG_Model* model);

MG_API void MG_model_enable(MG_Model* model, bool static_model);

MG_API void MG_model_free(MG_Model* model);

#ifdef __cplusplus
}
#endif