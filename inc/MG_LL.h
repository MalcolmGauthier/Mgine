#pragma once
#include "MG_include.h"

typedef struct MG_Generic_LL
{
    void* data;
    struct MG_Generic_LL* next;
} MG_Generic_LL;

void MG_LL_Add(MG_Generic_LL* head, void* value);

void* MG_LL_Remove(MG_Generic_LL* head, void* find);

// copies a linked list by value.
// the copy_func is used to copy the data pointer of each node. if null, standard pointer assignment is used.
MG_Generic_LL* MG_LL_Copy(MG_Generic_LL* head, void* (*copy_func)(void* source));

// goes recursively through the linked list and frees all nodes along with their data pointer.
// do not use this function if the data pointers are still in use.
// free_func gets called on each data pointer, unless free_func is null in which case a regular free is used.
void MG_LL_Free(MG_Generic_LL* head, void (*free_func)(void* data));
// frees the linked list without freeing the data pointers.
void MG_LL_Free_LL_Only(MG_Generic_LL* head);
