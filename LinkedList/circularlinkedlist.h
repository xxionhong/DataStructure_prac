#ifndef _CIRCULAR_H
#define _CIRCULAR_H

typedef struct cNode CNode;

void printfallCNode(CNode *);
void appendCNode(CNode **, int);
CNode *freeallCNodes(CNode *);
void insertCNode(CNode *, int, int);
void deleteCNode(CNode **, int);
void pushCNode(CNode **, int);

#endif