#include <stdio.h>
#include <stdlib.h>

typedef struct node
{
    int data;
    struct node *next;

} Node;

void printfallNodes(Node *node)
{
    while (node != NULL)
    {
        printf("%d, ", node->data);
        node = node->next;
    }
    printf("\n");
}

void appendNode(Node **start, int value)
{
    Node *newnode = (Node *)malloc(sizeof(Node));
    newnode->data = value;
    newnode->next = NULL;

    if (*start == NULL)
    {
        *start = newnode;
        return;
    }
    else
    {
        Node *current = *start;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = newnode;
        return;
    }
}

void freeallNodes(Node *start)
{
    Node *current, *temp;
    current = start;
    while (current != NULL)
    {
        temp = current;
        current = current->next;
        free(temp);
    }
}

void insertNode(Node *start, int value_after, int value)
{
    if (start == NULL)
    {
        printf("Error, This Linked list is empty! \n");
        return;
    }

    Node *current = start;
    while (current != NULL)
    {
        if (current->data == value_after)
        {
            Node *newnode = (Node *)malloc(sizeof(Node));
            newnode->data = value;
            newnode->next = NULL;

            if (current->next == NULL)
            {
                current->next = newnode;
                break;
            }
            else
            {
                newnode->next = current->next;
                current->next = newnode;
                break;
            }
        }
        current = current->next;
    }
}

void pushNode(Node **start, int value)
{
    Node *current = (*start);
    Node *newnode = (Node *)malloc(sizeof(Node));
    newnode->data = value;
    newnode->next = current;
    *start = newnode;
    return;
}

void deleteNode(Node **start, int value)
{
    Node *current = (*start), *temp = NULL;
    if (*start == NULL)
    {
        printf("Error, This Linked list is empty! \n");
    }
    if (current->data == value)
    {
        *start = current->next;
        free(current);
        return;
    }
    while (current != NULL)
    {
        if (current->next->data == value)
        {
            temp = current->next;
            current->next = current->next->next;
            free(temp);
            return;
        }
        current = current->next;
    }
}
void swap(Node *left, Node *right)
{
    left->data = left->data + right->data;
    right->data = left->data - right->data;
    left->data = left->data - right->data;
}

void bubblesorted(Node *start)
{
    int s;
    Node *current, *l_current = NULL;
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
                swap(current, current->next);
                s = 1;
            }
            current = current->next;
        }
        l_current = current;

    } while (s);
}

int main(int argc, char const *argv[])
{
    Node *first = NULL;
    appendNode(&first, 1);
    appendNode(&first, 2);
    appendNode(&first, 3);
    insertNode(first, 2, 4);
    insertNode(first, 4, 5);
    pushNode(&first, 0);
    pushNode(&first, -1);
    printfallNodes(first);
    bubblesorted(first);
    printfallNodes(first);
    deleteNode(&first, 2);
    deleteNode(&first, -1);
    printfallNodes(first);
    freeallNodes(first);
    return 0;
}
