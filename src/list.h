#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

#include "train.h"


struct Elem {
    Train_t *train;
    struct Elem *next;
};

typedef struct Elem *List_t;

List_t initList();
void addList(List_t *l, Train_t *t);
int removeList(List_t *l, Train_t *t);
void freeList(List_t *l);

#endif // LIST_H_INCLUDED
