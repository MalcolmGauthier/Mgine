#include "MG_LL.h"

void MG_LL_add(MG_Generic_LL** head_ptr, void* value)
{
    if (!head_ptr)
        return;

    MG_Generic_LL* new_node = malloc(sizeof(MG_Generic_LL));
    if (!new_node)
        return;

    new_node->data = value;
    new_node->next = NULL;

    if (!*head_ptr)
    {
        *head_ptr = new_node;
        return;
    }

    MG_Generic_LL* head = *head_ptr;
    while (head->next)
    {
        head = head->next;
    }
    head->next = new_node;
}

void* MG_LL_remove(MG_Generic_LL** head_ptr, void* find)
{
    if (!head_ptr || !*head_ptr || !find)
        return NULL;

    MG_Generic_LL* current = *head_ptr;
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
                // Update head if the first node is removed
                *head_ptr = current->next;
            }
            void* data = current->data;
            free(current);
            return data;
        }
        previous = current;
        current = current->next;
    }
    return NULL;
}

void* MG_LL_find(MG_Generic_LL* head, void* find)
{
    MG_Generic_LL* current = head;
    while (current)
    {
        if (current->data == find)
        {
            return current->data;
        }
        current = current->next;
    }
    return NULL;
}

int MG_LL_remove_free(MG_Generic_LL** head_ptr, void* find, void (*free_func)(void* data))
{
    if (!head_ptr || !*head_ptr || !find)
        return -1;

    MG_Generic_LL* current = *head_ptr;
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
                // Update head if the first node is removed
                *head_ptr = current->next;
            }
            void* data = current->data;
            free_func ? free_func(data) : free(data);
            free(current);
            return 0;
        }
        previous = current;
        current = current->next;
    }
    return 1;
}

MG_Generic_LL* MG_LL_copy(MG_Generic_LL* head, void* (*copy_func)(void* source))
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
            // this causes a memory leak if the data contains dynamic alloc.
            // but this branch is taken if the world is ending, so whatever.
            MG_LL_free(&new_head, NULL);
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

void MG_LL_free(MG_Generic_LL** head_ptr, void (*free_func)(void* data))
{
    if (!head_ptr || !*head_ptr)
        return;

    MG_Generic_LL* current = *head_ptr;
    while (current)
    {
        MG_Generic_LL* next = current->next;
        free_func ? free_func(current->data) : free(current->data);
        free(current);
        current = next;
    }
    *head_ptr = NULL;
}

void MG_LL_free_LL_only(MG_Generic_LL** head_ptr)
{
    if (!head_ptr || !*head_ptr)
        return;

    MG_Generic_LL* current = *head_ptr;
    while (current)
    {
        MG_Generic_LL* next = current->next;
        free(current);
        current = next;
    }
    *head_ptr = NULL;
}