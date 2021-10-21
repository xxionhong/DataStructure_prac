#include <stdio.h>
#include <stdlib.h>

typedef struct dcnode
{
    int data;
    struct dcnode *prev, *next;
} DCNode;

void printfallDCNodes(DCNode *start)
{
    DCNode *current = start;
    if (current == NULL)
    {
        printf("This Doubly circular Linked list is empty! \n");
        return;
    }
    do
    {
        printf("%d, ", current->data);
        current = current->next; /* code */
    } while (current != start);
    printf("| %d\n", current->data);
}

void pushDCNode(DCNode **start, int value)
{
    DCNode *current = (*start), *newnode = NULL;
    newnode = (DCNode *)malloc(sizeof(DCNode));
    newnode->data = value;
    if ((*start) == NULL)
    {
        newnode->prev = newnode;
        newnode->next = newnode;
        (*start) = newnode;
    }
    else
    {
        current->prev = newnode;
        newnode->next = current;
        do
        {
            current = current->next;
        } while (current->next != (*start));
        current->next = newnode;
        newnode->prev = current;
        (*start) = newnode;
    }
}

void appnedDCNode(DCNode **start, int value)
{
    DCNode *newnode = NULL, *current = (*start);
    newnode = (DCNode *)malloc(sizeof(DCNode));
    newnode->data = value;
    if ((*start) == NULL)
    {
        (*start) = newnode;
        newnode->next = newnode;
        newnode->prev = newnode;
    }
    else if (current->next == (*start))
    {
        current->next = newnode;
        current->prev = newnode;
        newnode->prev = current;
        newnode->next = current;
    }
    else
    {
        do
        {
            current = current->next;
        } while (current->next != (*start));
        newnode->next = (*start);
        newnode->prev = current;
        current->next = newnode;
        (*start)->prev = newnode;
    }
}

void insertDCNode(DCNode **start, int after_value, int value)
{
    DCNode *current = (*start), *newnode = NULL;
    newnode = (DCNode *)malloc(sizeof(DCNode));
    newnode->data = value;
    if ((*start) == NULL)
    {
        (*start) = newnode;
        newnode->next = newnode;
        newnode->prev = newnode;
    }
    else
    {
        do
        {
            current = current->next;
        } while (current->data != after_value);
        newnode->prev = current;
        newnode->next = current->next;
        current->next->prev = newnode;
        current->next = newnode;
    }
}

void deleteDCNode(DCNode **start, int value)
{
    DCNode *current = (*start), *temp = NULL;
    if (current->data == value)
    {
        temp = current;
        (*start) = current->next;
        current->next->prev = current->prev;
        current->prev->next = current->next;
        free(temp);
    }
    else
    {
        do
        {
            current = current->next;
        } while (current->next->data != value);
        temp = current->next;
        current->next = current->next->next;
        current->next->prev = current;
        free(temp);
    }
}

DCNode *freeallDCNode(DCNode *start)
{
    DCNode *current = start, *temp = NULL;
    if (current->next = current)
    {
        free(current);
    }
    else
    {
        do
        {
            temp = current;
            current->next = current->next->next;
            current->next->prev = current->prev;
            current = current->next;
            free(temp);
        } while (current != start);
    }
    return NULL;
}


int main(int argc, char const *argv[])
{
    DCNode *first = NULL, *second = NULL;
    first = (DCNode *)malloc(sizeof(DCNode));
    second = (DCNode *)malloc(sizeof(DCNode));
    first->data = 1;
    first->next = second;
    first->prev = second;
    second->data = 2;
    second->next = first;
    second->prev = first;
    printfallDCNodes(first);
    appnedDCNode(&first, 3);
    appnedDCNode(&first, 5);
    printfallDCNodes(first);
    pushDCNode(&first, 4);
    pushDCNode(&first, 7);
    printfallDCNodes(first);
    insertDCNode(&first, 2, 6);
    insertDCNode(&first, 3, 8);
    printfallDCNodes(first);
    deleteDCNode(&first, 6);
    deleteDCNode(&first, 7);
    printfallDCNodes(first);
    first = freeallDCNode(first);
    printfallDCNodes(first);
    return 0;
}
