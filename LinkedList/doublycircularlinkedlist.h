#ifndef _DOUBLE_CIRCULAR_H
#define _DOUBLE_CIRCULAR_H

typedef struct dcnode DCNode;

void printfallDCNodes(DCNode *);
void pushDCNode(DCNode **, int);
void appnedDCNode(DCNode **, int);
void insertDCNode(DCNode **, int, int);
void deleteDCNode(DCNode **, int);
DCNode *freeallDCNode(DCNode *);

#endif