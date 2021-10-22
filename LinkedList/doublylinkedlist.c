#include <stdio.h>
#include <stdlib.h>
#include "doublylinkedlist.h"

typedef struct dNode
{
    int data;
    struct dNode *prev, *next;
} DNode;

void printfallDNodes(DNode *start)
{
    if (start == NULL)
    {
        printf("This Double Linked List is empty! \n");
        return;
    }
    while (start != NULL)
    {
        printf("%d, ", start->data);
        start = start->next;
    }
    printf("\n");
}

/*
void inverse_printfallDNodes(DNode *end)
{
    while (end != NULL)
    {
        printf("%d, ", end->data);
        end = end->prev;
    }
    printf("\n");
}
*/

void appendDNode(DNode **start, int value)
{
    DNode *current = (*start), *newnode = NULL;
    newnode = (DNode *)malloc(sizeof(DNode));
    newnode->data = value;
    newnode->prev = NULL;
    newnode->next = NULL;

    if (*start == NULL)
    {
        *start = newnode;
    }
    else
    {
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = newnode;
        newnode->prev = current;
    }
}

void pushDNode(DNode **start, int value)
{
    DNode *current = *start, *newnode = NULL;
    newnode = (DNode *)malloc(sizeof(DNode));
    newnode->data = value;
    newnode->prev = NULL;
    newnode->next = NULL;
    if (current != NULL)
    {
        newnode->next = current;
        current->prev = newnode;
    }
    *start = newnode;
}

DNode *freeallDNodes(DNode *start)
{
    DNode *current = start, *temp = NULL;
    while (current != NULL)
    {
        temp = current;
        current = current->next;
        free(temp);
    }
    return NULL;
}

void insertDNode(DNode *start, int after_value, int value)
{
    DNode *current = start;
    while (current != NULL)
    {
        if (current->data == after_value)
        {
            DNode *newnode = (DNode *)malloc(sizeof(DNode));
            newnode->data = value;
            newnode->prev = NULL;
            newnode->next = NULL;
            if (current->next == NULL)
            {
                current->next = newnode;
                newnode->prev = current;
                return;
            }
            else
            {
                newnode->next = current->next;
                newnode->prev = current;
                current->next->prev = newnode;
                current->next = newnode;
                return;
            }
        }
        current = current->next;
    }
}

void deleteDNode(DNode **start, int value)
{
    DNode *current = *start, *temp = NULL;

    if (current->data == value)
    {
        *start = current->next;
        (*start)->prev = NULL;
        free(current);
    }
    while (current != NULL)
    {
        if (current->next->data == value)
        {
            temp = current->next;
            current->next = current->next->next;
            if (current->next != NULL)
            {
                current->next->prev = current;
            }
            free(temp);
            return;
        }
        current = current->next;
    }
}

void D_swap(DNode *left, DNode *right)
{
    left->data = left->data + right->data;
    right->data = left->data - right->data;
    left->data = left->data - right->data;
}

void D_bubblesorted(DNode *start)
{
    int s;
    DNode *current, *l_current = NULL;
    if (start == NULL)
    {
        printf("This Linked list is empty!\n");
        return;
    }
    do
    {
        s = 0;
        current = start;
        while (current->next != l_current)
        {
            if (current->data > current->next->data)
            {
                D_swap(current, current->next);
                s = 1;
            }
            current = current->next;
        }
        l_current = current;

    } while (s);
}

/*
int main(int argc, char const *argv[])
{
    DNode *first = NULL;

    appendDNode(&first, 1);
    appendDNode(&first, 2);
    appendDNode(&first, 3);
    pushDNode(&first, 0);
    pushDNode(&first, -1);
    insertDNode(first, 2, 4);
    insertDNode(first, 4, 6);
    printfallDNodes(first);
    D_bubblesorted(first);
    printfallDNodes(first);
    deleteDNode(&first, 6);
    deleteDNode(&first, 2);
    DNode *last = first;
    while (last->next != NULL)
    {
        last = last->next;
    }
    printf("Inverse print: \n");
    inverse_printfallDNodes(last);
    first = freeallDNodes(first);
    printfallDNodes(first);
    return 0;
}
*/