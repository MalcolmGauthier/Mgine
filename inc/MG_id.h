#pragma once
#include "MG_include.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t MG_ID;

MG_API MG_ID MG_ID_get_id(const char* name);

#ifdef __cplusplus
}
#endif