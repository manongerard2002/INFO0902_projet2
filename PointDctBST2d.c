/* ========================================================================= *
 * PointDct definition (with BST2d)
 * ========================================================================= */

#include "PointDct.h"
#include "List.h"
#include "Point.h"
#include "BST2d.h"

#include <stdlib.h>
#include <stdio.h>

struct PointDct_t
{
    BST2d *bst2d;
};

PointDct *pdctCreate(List *lpoints, List *lvalues)
{
    PointDct *pd = malloc(sizeof(PointDct));
    BST2d *bst2d = bst2dNew();
    if (pd == NULL || bst2d == NULL)
    {
        printf("pdctCreate: allocation error\n");
        return NULL;
    }
    bool error = false;
    LNode *pp = lpoints->head, *pv = lvalues->head;
    while (pp != NULL && pv != NULL)
    {
        error = error || !bst2dInsert(bst2d, pp->value, pv->value);
        pp = pp->next;
        pv = pv->next;
    }
    if (error) 
    {
        pdctFree(pd);
        return NULL;
    }
    pd->bst2d = bst2d;
    return pd;
}

void pdctFree(PointDct *pd)
{
    bst2dFree(pd->bst2d, false, false);
    free(pd);
}

size_t pdctSize(PointDct *pd)
{
    return bst2dSize(pd->bst2d);
}

void *pdctExactSearch(PointDct *pd, Point *p)
{
    return bst2dSearch(pd->bst2d, p);
}

List *pdctBallSearch(PointDct *pd, Point *p, double r)
{
    return bst2dBallSearch(pd->bst2d, p, r);
}
