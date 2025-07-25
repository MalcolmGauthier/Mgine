#include "MG_LL.h"

void MG_LL_Add(MG_Generic_LL* head, void* value)
{
    if (!head)
        return;

    while (head->next)
    {
        head = head->next;
    }

    MG_Generic_LL* new_node = (MG_Generic_LL*)malloc(sizeof(MG_Generic_LL));
    if (new_node)
    {
        new_node->data = value;
        new_node->next = NULL;
        head->next = new_node;
    }
}

void* MG_LL_Remove(MG_Generic_LL* head, void* find)
{
    if (!head || !find)
        return NULL;

    MG_Generic_LL* current = head;
    MG_Generic_LL* previous = NULL;
    while (current)
    {
        if (current->data == find)
        {
            if (previous)
            {
                previous->next = current->next;
            }
            else
            {
                head = current->next; // Update head if the first node is removed
            }
            void* data = current->data;
            free(current);
            return data;
        }
        previous = current;
        current = current->next;
    }
    return NULL; // Not found
}

// copies a linked list by value.
// the copy_func is used to copy the data pointer of each node. if null, standard pointer assignment is used.
MG_Generic_LL* MG_LL_Copy(MG_Generic_LL* head, void* (*copy_func)(void* source))
{
    if (!head)
        return NULL;

    MG_Generic_LL* new_head = calloc(1, sizeof(MG_Generic_LL));
    if (!new_head)
        return NULL;
    new_head->data = copy_func ? copy_func(head->data) : head->data;

    MG_Generic_LL* current_dst = new_head;
    MG_Generic_LL* current_src = head->next;
    while (current_src)
    {
        MG_Generic_LL* new_node = calloc(1, sizeof(MG_Generic_LL));
        if (!new_node)
        {
            MG_LL_Free(new_head);
            return NULL;
        }
        new_node->data = copy_func ? copy_func(current_src->data) : current_src->data;
        new_node->next = NULL;
        current_dst->next = new_node;
        current_dst = new_node;
        current_src = current_src->next;
    }
	return new_head;

}

// goes recursively through the linked list and frees all nodes along with their data pointer.
// do not use this function if the data pointers are still in use.
void MG_LL_Free(MG_Generic_LL* head)
{
    MG_Generic_LL* current = head;
    while (current)
    {
        MG_Generic_LL* next = current->next;
        free(current->data);
        free(current);
        current = next;
    }
}