#include <stdio.h>
#include "singlylinkedlist.c"
#include "doublylinkedlist.c"
#include "circularlinkedlist.c"

int main(int argc, char const *argv[])
{
    printf("*---------------------*\n");
    printf("Singly Linkedlist Demo:\n");
    Node *head = NULL;
    appendNode(&head, 1);
    appendNode(&head, 2);
    appendNode(&head, 3);
    insertNode(head, 2, 4);
    insertNode(head, 4, 5);
    pushNode(&head, 0);
    pushNode(&head, -1);
    printfallNodes(head);
    deleteNode(&head, 2);
    deleteNode(&head, 1);
    printfallNodes(head);
    freeallNodes(head);

    printf("*---------------------*\n");
    printf("Doubly Linkedlist Demo:\n");
    DNode *first = NULL;
    appendDNode(&first, 1);
    appendDNode(&first, 2);
    appendDNode(&first, 3);
    pushDNode(&first, 0);
    pushDNode(&first, -1);
    insertDNode(first, 2, 4);
    insertDNode(first, 4, 6);
    printfallDNodes(first);
    deleteDNode(&first, 6);
    deleteDNode(&first, 2);
    printfallDNodes(first);
    DNode *last = first;
    while (last->next != NULL)
    {
        last = last->next;
    }
    printf("inverse print: \n");
    inverse_printfallDNodes(last);
    freeallDNodes(first);

    printf("*---------------------*\n");
    printf("Circular Linkedlist Demo:\n");
    CNode *second = NULL;
    appendCNode(&second, 6);
    appendCNode(&second, 4);
    appendCNode(&second, 5);
    appendCNode(&second, 7);
    appendCNode(&second, 8);
    printfallCNode(second);
    insertCNode(second, 4, 66);
    insertCNode(second, 7, 9);
    insertCNode(second, 6, 10);
    insertCNode(second, 8, 11);
    printfallCNode(second);
    deleteCNode(&second, 8);
    printfallCNode(second);
    deleteCNode(&second, 4);
    printfallCNode(second);
    deleteCNode(&second, 6);
    printfallCNode(second);
    pushCNode(&second, 1);
    pushCNode(&second, 0);
    printfallCNode(second);
    freeallCNodes(second);

    printf("*---------------------*\n");
    return 0;
}
