#pragma once
#include "MG_include.h"

#ifdef __cplusplus
extern "C" {
#endif

// generics in C? what is this, some kind of OOP?
typedef struct MG_Generic_LL
{
    void* data;
    struct MG_Generic_LL* next;
} MG_Generic_LL;

// takes in a reference to a linked list and the pointer to store as data.
// head_ptr can point to a null value, in which case a new linked list is allocated at head_ptr.
// dynamic memory is used, so MG_LL_free must be used to free list. (or MG_LL_free_LL_only if you wish to keep the data)
MG_API int MG_LL_add(MG_Generic_LL** head_ptr, void* value);

// iterates through linked list and removes the first occurence of a node whose data pointer matches the input.
// this does not free the memory located at the data pointer.
// returns null on failure or failure to find data, otherwise returns the data pointer of the removed node.
MG_API void* MG_LL_remove(MG_Generic_LL** head_ptr, void* find);

// iterates through linked list and returns the first occurence of the data pointer matching the input.
// returns null on failure to find data.
MG_API void* MG_LL_find(MG_Generic_LL* head, void* find);

// just like MG_LL_find, but takes in a specialized function to compare data. 
// the node's data pointer and the inputed pointer will be passed into this function, and the function is expected to return true on match and false on mismatch.
// find_func must be able to handle a null pointer being passed as input.
// returns null on failure to find data, otherwise returns the first occurence of find_func returning true.
MG_API void* MG_LL_find_func(MG_Generic_LL* head, void* find, bool (*find_func)(void* data, void* find));

// copies a linked list into a newly allocated linked list by value.
// copy_func is called with each node's data pointer, and thus should be used to copy the data found at that memory and return a pointer to the newly copied memory. 
// if copy_func is null, standard pointer assignment is used, so both nodes of each linked list will point to the same memory.
// copy_func must be able to handle a null pointer being passed as input.
// returns pointer to newly allocated linked list, or null on failure.
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