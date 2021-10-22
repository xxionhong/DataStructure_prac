#ifndef _DOUBLY_H
#define _DOUBLY_H

typedef struct dNode DNode;

void printfallDNodes(DNode *);
// void inverse_printfallDNodes(DNode *);
void appendDNode(DNode **, int);
void pushDNode(DNode **, int);
DNode *freeallDNodes(DNode *);
void insertDNode(DNode *, int, int);
void deleteDNode(DNode **, int);
void D_swap(DNode *, DNode *);
void D_bubblesorted(DNode *);

#endif