#pragma once
#include "MG_include.h"
#include "stc/MG_Hashmap.h"

#ifdef __cplusplus
extern "C" {
#endif

MG_API MG_NAME MG_id_hash_string(const char* name);

MG_API int MG_hashmap_init(MG_Hashmap** hashset_ref, uint32_t bucket_count);
MG_API int MG_hashmap_add(MG_Hashmap* hashset, MG_ID key, void* value);
MG_API void* MG_hashmap_remove(MG_Hashmap* hashset, MG_ID key);
MG_API void* MG_hashmap_get(MG_Hashmap* hashset, MG_ID key);
MG_API void MG_hashmap_free(MG_Hashmap** hashset_ref, void (*free_func)(void*));

#ifdef __cplusplus
}
#endif