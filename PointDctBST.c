/* ========================================================================= *
 * PointDct definition (with BST)
 * ========================================================================= */

#include "PointDct.h"
#include "List.h"
#include "Point.h"
#include "BST.h"

#include <stdlib.h>
#include <stdio.h>

/* Opaque Structure */

typedef struct Value_t Value;

struct Value_t
{
    void *position;
    void *value;
};

struct PointDct_t
{
    BST *bst;
};

/* ------------------------------------------------------------------------- *
 * Compares two values using ptCompare
 *
 * PARAMETERS
 * a, b		The values to be compared
 *
 * RETURN
 * res      The result of the comparison
 * ------------------------------------------------------------------------- */
int compare_doubles(void* a, void* b);

PointDct *pdctCreate(List *lpoints, List *lvalues)
{
    PointDct *pd = malloc(sizeof(PointDct));
    BST *bst = bstNew(&compare_doubles);
    if (pd == NULL || bst == NULL)
    {
        printf("pdctCreate: allocation error\n");
        return NULL;
    }
    bool error = false;
    LNode *pp = lpoints->head, *pv = lvalues->head;
    while (pp != NULL && pv != NULL)
    {
        Value *value = malloc(sizeof(Value));
        value->value = pv->value;
        value->position = pp->value;
        error = error || !bstInsert(bst, pp->value, value);
        pp = pp->next;
        pv = pv->next;
    }
    if (error) 
    {
        pdctFree(pd);
        return NULL;
    }
    pd->bst = bst;
    return pd;
}

int compare_doubles(void* a, void* b)
{
    return ptCompare((Point*) a, (Point*) b);
}

void pdctFree(PointDct *pd)
{
    bstFree(pd->bst, false, true);
    free(pd);
}

size_t pdctSize(PointDct *pd)
{
    return bstSize(pd->bst);
}

void *pdctExactSearch(PointDct *pd, Point *p)
{
    void *val = bstSearch(pd->bst, p);
    if (val == NULL)
        return NULL;
    return ((Value*) val)->value;
}

List *pdctBallSearch(PointDct *pd, Point *p, double r)
{
    // first filtrage of the keys with the fonction bstRangeSearch
    Point *keymin = ptNew(ptGetx(p) - r, ptGety(p) - r);
    if (keymin == NULL)
        return NULL;
    Point *keymax = ptNew(ptGetx(p) + r, ptGety(p) + r);
    if (keymax == NULL)
    {
        ptFree(keymin);
        return NULL;
    }
    List *list = bstRangeSearch(pd->bst, keymin, keymax);
    if (list == NULL)
        return NULL;
    ptFree(keymin);
    ptFree(keymax);

    // erasing of the points outside the ball
    r = r * r;
    bool error = false;
    LNode *prev = NULL;
    for (LNode *n = list->head; n != NULL; )
    {
        if (!(ptSqrDistance(((Value *)n->value)->position, p) <= r))
        {
            //suppress n of the liste
            // if n = head and n = tail
            if (prev == NULL && n->next == NULL)
            {
                list->head = NULL;
                list->last = NULL;
                free(n);
                n = NULL;
            }
            // if n = head and n != tail
            else if (prev == NULL)
            {
                list->head = n->next;
                free(n);
                n = list->head;
            }
            // if n = tail and prev != NULL
            else if(n->next == NULL)
            {
                list->last = prev;
                prev->next = NULL;
                free(n);
                n = NULL;
            }
            else
            {
                prev->next = n->next;
                free(n);
                n = prev->next;
            }

            // if here, size is at least of 1, so ok
            list->size--;
        }
        else
        {
            // get the field value of "Value"
            n->value = ((Value *) n->value)->value;
            prev = n;
            n = n->next;
        }
    }
    if (error) 
    {
        listFree(list, false);
        return NULL;
    }
    return list;
}
