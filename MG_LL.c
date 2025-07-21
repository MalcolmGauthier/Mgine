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