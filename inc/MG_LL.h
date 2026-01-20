#pragma once
#include "MG_include.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MG_Generic_LL
{
    void* data;
    struct MG_Generic_LL* next;
} MG_Generic_LL;

MG_API int MG_LL_add(MG_Generic_LL** head_ptr, void* value);

MG_API void* MG_LL_remove(MG_Generic_LL** head_ptr, void* find);

MG_API void* MG_LL_find(MG_Generic_LL* head, void* find);

// find_func must be able to handle null.
MG_API void* MG_LL_find_func(MG_Generic_LL* head, void* find, bool (*find_func)(void* data, void* find));

// copies a linked list by value.
// the copy_func is used to copy the data pointer of each node. if null, standard pointer assignment is used.
MG_API MG_Generic_LL* MG_LL_copy(MG_Generic_LL* head, void* (*copy_func)(void* source));

// goes recursively through the linked list and frees all nodes along with their data pointer.
// do not use this function if the data pointers are still in use.
// free_func gets called on each data pointer, unless free_func is null in which case a regular free is used.
MG_API void MG_LL_free(MG_Generic_LL** head_ptr, void (*free_func)(void* data));
// frees the linked list without freeing the data pointers.
MG_API void MG_LL_free_LL_only(MG_Generic_LL** head_ptr);

#ifdef __cplusplus
}
#endif