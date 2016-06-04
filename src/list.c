#include "list.h"

List_t initList()
{
    return (List_t) NULL;
}

void addList(List_t *l, Train_t *t)
{
    struct Elem *e = (struct Elem*) malloc(sizeof(struct Elem));
    e->train = t;
    e->next = NULL;
    if(*l == NULL)
        *l = e;
    else
    {
        struct Elem *prev = *l;
        while(prev->next != NULL)
            prev = prev->next;
        prev->next = e;
    }
}

int removeList(List_t *l, Train_t *t)
{
    return -1;
}

void freeList(List_t *l)
{
    struct Elem *act, *next;
    act = *l;
    while(act != NULL)
    {
        next = act-> next;
        free(l);
        act = next;
    }
}
