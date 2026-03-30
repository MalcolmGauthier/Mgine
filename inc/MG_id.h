#pragma once
#include "MG_include.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t MG_ID;
typedef uint32_t MG_NAME;

MG_API MG_NAME MG_ID_hash_string(const char* name);

#ifdef __cplusplus
}
#endif