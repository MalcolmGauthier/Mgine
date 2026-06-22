#pragma once
#include "MG_include.h"
#include "stc/MG_Hashmap.h"

#ifdef __cplusplus
extern "C" {
#endif

MG_API MG_NAME MG_id_hash_string(const char* name);

MG_API int MG_hashmap_init(MG_Hashmap** hashset_ref, uint32_t bucket_count);
// return 0 on success, 1 if there's a duplicate key and negative number on error
MG_API int MG_hashmap_add(MG_Hashmap* hashset, MG_ID key, void* value);
MG_API void* MG_hashmap_remove(MG_Hashmap* hashset, MG_ID key);
MG_API void* MG_hashmap_get(MG_Hashmap* hashset, MG_ID key);
MG_API MG_Hashmap* MG_hashmap_copy(MG_Hashmap* src, void* (*copy_func)(void*));
MG_API void MG_hashmap_free(MG_Hashmap** hashset_ref, void (*free_func)(void*));

#ifdef __cplusplus
}
#endif