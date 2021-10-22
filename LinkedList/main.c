#include <stdio.h>
#include "singlylinkedlist.h"
#include "doublylinkedlist.h"
#include "circularlinkedlist.h"
#include "doublycircularlinkedlist.h"

int main(int argc, char const *argv[])
{
    printf("*---------------------*\n");
    printf("Singly Linkedlist Demo:\n");

    Node *singly = NULL;
    appendNode(&singly, 1);
    appendNode(&singly, 2);
    appendNode(&singly, 3);
    insertNode(singly, 2, 4);
    insertNode(singly, 4, 5);
    pushNode(&singly, 0);
    pushNode(&singly, -1);
    printfallNodes(singly);
    S_bubblesorted(singly);
    printfallNodes(singly);
    deleteNode(&singly, 2);
    deleteNode(&singly, -1);
    printfallNodes(singly);
    singly = freeallNodes(singly);
    printfallNodes(singly);

    printf("*---------------------*\n");
    printf("Doubly Linkedlist Demo:\n");

    DNode *doubly = NULL;
    appendDNode(&doubly, 1);
    appendDNode(&doubly, 2);
    appendDNode(&doubly, 3);
    pushDNode(&doubly, 0);
    pushDNode(&doubly, -1);
    insertDNode(doubly, 2, 4);
    insertDNode(doubly, 4, 6);
    printfallDNodes(doubly);
    D_bubblesorted(doubly);
    printfallDNodes(doubly);
    deleteDNode(&doubly, 6);
    deleteDNode(&doubly, 2);
    printfallDNodes(doubly);
    doubly = freeallDNodes(doubly);
    printfallDNodes(doubly);

    printf("*---------------------*\n");
    printf("Circular Linkedlist Demo:\n");

    CNode *circular = NULL;
    appendCNode(&circular, 6);
    appendCNode(&circular, 4);
    appendCNode(&circular, 5);
    appendCNode(&circular, 7);
    appendCNode(&circular, 8);
    printfallCNode(circular);
    insertCNode(circular, 4, 66);
    insertCNode(circular, 7, 9);
    insertCNode(circular, 6, 10);
    insertCNode(circular, 8, 11);
    printfallCNode(circular);
    deleteCNode(&circular, 8);
    printfallCNode(circular);
    deleteCNode(&circular, 4);
    printfallCNode(circular);
    deleteCNode(&circular, 6);
    printfallCNode(circular);
    pushCNode(&circular, 1);
    pushCNode(&circular, 0);
    printfallCNode(circular);
    circular = freeallCNodes(circular);
    printfallCNode(circular);

    printf("*---------------------*\n");
    printf("Doubly Circular Linkedlist Demo:\n");

    DCNode *doublecircular = NULL;
    appnedDCNode(&doublecircular, 3);
    appnedDCNode(&doublecircular, 5);
    printfallDCNodes(doublecircular);
    pushDCNode(&doublecircular, 4);
    pushDCNode(&doublecircular, 7);
    printfallDCNodes(doublecircular);
    insertDCNode(&doublecircular, 4, 6);
    insertDCNode(&doublecircular, 3, 8);
    printfallDCNodes(doublecircular);
    deleteDCNode(&doublecircular, 6);
    deleteDCNode(&doublecircular, 7);
    printfallDCNodes(doublecircular);
    doublecircular = freeallDCNode(doublecircular);
    printfallDCNodes(doublecircular);

    printf("*---------------------*\n");

    return 0;
}
