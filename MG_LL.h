#pragma once
#include "MG_include.h"

typedef struct MG_Generic_LL
{
    void* data;
    struct MG_Generic_LL* next;
}
MG_Generic_LL;

void MG_LL_Add(MG_Generic_LL* head, void* value);

void* MG_LL_Remove(MG_Generic_LL* head, void* find);

void MG_LL_Free(MG_Generic_LL* head);