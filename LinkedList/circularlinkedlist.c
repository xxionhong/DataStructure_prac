#include <stdlib.h>
#include <stdio.h>
#include "circularlinkedlist.h"

typedef struct cNode
{
    int data;
    struct cNode *next;

} CNode;

void printfallCNode(CNode *start)
{
    if (start == NULL)
    {
        printf("This circular linked list is empty!\n");
        return;
    }
    CNode *current = start;
    do
    {
        printf("%d, ", current->data);
        current = current->next;
    } while (current != start);
    printf("| %d\n", current->data);
}

void appendCNode(CNode **start, int value)
{
    CNode *newnode = (CNode *)malloc(sizeof(newnode));
    newnode->data = value;
    newnode->next = NULL;
    if ((*start) == NULL)
    {
        (*start) = newnode;
        newnode->next = newnode;
    }
    else
    {
        CNode *current = (*start);
        while (current->next != (*start))
        {
            current = current->next;
        }
        current->next = newnode;
        newnode->next = (*start);
    }
}

CNode *freeallCNodes(CNode *start)
{
    CNode *current = start;
    if (current->next == current)
    {
        free(current);
    }
    else
    {
        do
        {
            current = start->next;
            start->next = current->next;
            free(current);
        } while (current != start);
    }
    return NULL;
}

void insertCNode(CNode *start, int after_value, int value)
{
    CNode *current = start, *newnode = NULL;
    newnode = (CNode *)malloc(sizeof(CNode));
    newnode->data = value;
    if (current->data == after_value)
    {
        newnode->next = current->next;
        current->next = newnode;
    }
    else
    {
        while (current->data != after_value)
        {
            current = current->next;
        }
        newnode->next = current->next;
        current->next = newnode;
    }
}

void deleteCNode(CNode **start, int value)
{
    CNode *current = (*start), *temp = NULL;
    if (current->data == value)
    {
        temp = current;
        (*start) = (*start)->next;
        do
        {
            current = current->next;
        } while (current->next != temp);
        current->next = (*start);
        free(temp);
        return;
    }

    do
    {
        current = current->next;
    } while (current->next->data != value);
    temp = current->next;
    current->next = current->next->next;
    free(temp);
}

void pushCNode(CNode **start, int value)
{
    CNode *current = (*start), *newnode = NULL;
    newnode = (CNode *)malloc(sizeof(CNode));
    newnode->data = value;
    newnode->next = (*start);
    do
    {
        current = current->next;
    } while (current->next != (*start));
    current->next = newnode;
    (*start) = newnode;
}

/*
int main(int argc, char const *argv[])
{
    CNode *first = NULL;
    appendCNode(&first, 6);
    appendCNode(&first, 4);
    appendCNode(&first, 5);
    appendCNode(&first, 7);
    appendCNode(&first, 8);
    printfallCNode(first);
    insertCNode(first, 4, 66);
    insertCNode(first, 7, 9);
    insertCNode(first, 6, 10);
    insertCNode(first, 8, 11);
    printfallCNode(first);
    deleteCNode(&first, 8);
    printfallCNode(first);
    deleteCNode(&first, 4);
    printfallCNode(first);
    deleteCNode(&first, 6);
    printfallCNode(first);
    pushCNode(&first, 1);
    pushCNode(&first, 0);
    printfallCNode(first);
    first = freeallCNodes(first);
    printfallCNode(first);
    return 0;
}
*/