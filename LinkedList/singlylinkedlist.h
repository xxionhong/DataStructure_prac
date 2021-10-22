#ifndef _SINGLY_H
#define _SINGLY_H

typedef struct node Node;
void printfallNodes(Node *);
void appendNode(Node **, int);
Node *freeallNodes(Node *);
void insertNode(Node *, int, int);
void pushNode(Node **, int);
void deleteNode(Node **, int);
void S_swap(Node *, Node *);
void S_bubblesorted(Node *);

#endif