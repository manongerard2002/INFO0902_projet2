/* ========================================================================= *
 * BST2d definition
 * ========================================================================= */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "BST2d.h"
#include "Point.h"
#include "List.h"

/* Opaque Structure */

typedef struct BNode_t BNode;

struct BNode_t
{
    BNode *parent;
    BNode *left;
    BNode *right;
    Point *point; 
    void *value;
};

struct BST2d_t
{
    BNode *root;
    size_t size;
};

/* Function definitions */

/* ------------------------------------------------------------------------- *
 * Frees the allocated memory of the given node.
 *
 * PARAMETERS
 * n          	A valid pointer to a node object.
 * freeKey      Whether to free the keys.
 * freeValue    Whether to free the values.
 *
 * ------------------------------------------------------------------------- */
static void bstFreeRec(BNode *n, bool freeKey, bool freeValue);

/* ------------------------------------------------------------------------- *
 * Creates a new node
 *
 * The BNode must later be deleted by calling bstFreeRec().
 *
 * PARAMETERS
 * point		The position of the new element (a Point object).
 * value    	The value to store.
 *
 * RETURN
 * BNode        A pointer to the node, or NULL in case of error.
 * ------------------------------------------------------------------------- */
static BNode *bnNew(Point *point, void *value);

/* ------------------------------------------------------------------------- *
 * Compares two points in order to place them correctly in the tree.
 *
 * PARAMETERS
 * p1, p2		Two valid pointers to point objets.
 * depth    	The depth of the BST2d.
 *
 * RETURN
 * nb			The result of the comparison.
 * ------------------------------------------------------------------------- */
static int compare(Point *p1, Point *p2, size_t depth);

/* ------------------------------------------------------------------------- *
 * Compares two points in order to define if they are identical or not.
 *
 * PARAMETERS
 * p1, p2		Two valid pointers to point objets.
 * depth    	The depth of the BST2d.
 *
 * RETURN
 * res          A boolean equal to true if both the points are identical,
 *				false otherwisethe.
 * ------------------------------------------------------------------------- */
static bool Equal(Point *p1, Point *p2, size_t depth);

/* ------------------------------------------------------------------------- *
 * Detects if a point is inside or oustide the search radius.
 *
 * PARAMETERS
 * n			A valid pointer to a node objet.
 * q	    	A valid pointer to a point objet.
 * r			The radius of the ball.
 * depth		The depth of the node.
 * list			A valid pointer to a list objet.
 * error		A boolean value to detect if errors occur inside the function.
 *
 * ------------------------------------------------------------------------- */
static void bst2dBallSearchRec(BNode *n, Point *q, double r, size_t depth, List *list, bool *error);

/* ------------------------------------------------------------------------- *
 * Defines if the search should continue on the left.
 *
 * PARAMETERS
 * p1, p2		Two valid pointers to point objets.
 * r			The radius of the ball.
 * depth    	The depth of the BST2d.
 
 * RETURN
 * res          A boolean equal to true if the seach continues,
 *				false otherwisethe.
 *
 * ------------------------------------------------------------------------- */
bool continueLeft(Point *p1, Point *p2, double r, size_t depth);

/* ------------------------------------------------------------------------- *
 * Defines if the search should continue on the right.
 *
 * PARAMETERS
 * p1, p2		Two valid pointers to point objets.
 * r			The radius of the ball.
 * depth    	The depth of the BST2d.
 
 * RETURN
 * res          A boolean equal to true if the seach continues,
 *				false otherwisethe.
 *
 * ------------------------------------------------------------------------- */
bool continueRight(Point *p1, Point *p2, double r, size_t depth);

/* ------------------------------------------------------------------------- *
 * Computes the depth of a node inside a BST2d.
 *
 * PARAMETERS
 * n          			A valid pointer to a node object
 * totalNodeDepth     	The depth of the parent of the node.
 
 * RETURN
 * bstDepthRec          The depth of the node.
 *
 * ------------------------------------------------------------------------- */
int bst2dDepthRec(BNode *n, int totalNodeDepth);

BNode *bnNew(Point *point, void *value)
{
    BNode *n = malloc(sizeof(BNode));
    if (n == NULL)
    {
        printf("bnNew: allocation error\n");
        return NULL;
    }
    n->left = NULL;
    n->right = NULL;
    n->point = point;
    n->value = value;
    return n;
}

BST2d *bst2dNew(void)
{
    BST2d *bst2d = malloc(sizeof(BST2d));
    if (bst2d == NULL)
    {
        printf("bst2dNew: allocation error");
        return NULL;
    }
    bst2d->root = NULL;
    bst2d->size = 0;
    return bst2d;
}

void bst2dFree(BST2d *bst2d, bool freeKey, bool freeValue)
{
    bstFreeRec(bst2d->root, freeKey, freeValue);
    free(bst2d);
}

void bstFreeRec(BNode *n, bool freeKey, bool freeValue)
{
    if (n == NULL)
        return;
    bstFreeRec(n->left, freeKey, freeValue);
    bstFreeRec(n->right, freeKey, freeValue);
    if (freeKey)
        ptFree(n->point);
    if (freeValue)
        free(n->value);
    free(n);
}

size_t bst2dSize(BST2d *bst2d)
{
    return bst2d->size;
}

bool bst2dInsert(BST2d *b2d, Point *point, void *value)
{
    if (b2d->root == NULL)
    {
        b2d->root = bnNew(point, value);
        if (b2d->root == NULL)
        {
            return false;
        }
        b2d->size++;
        return true;
    }
    BNode *prev = NULL;
    BNode *n = b2d->root;
    int depth = -1;
    while (n != NULL)
    {
        prev = n;
        int cmp = compare(point, n->point, ++depth);
        if (cmp <= 0)
        {
            n = n->left;
        }
        else if (cmp > 0)
        {
            n = n->right;
        }
    }
    BNode *new = bnNew(point, value);
    if (new == NULL)
    {
        return false;
    }
    new->parent = prev;
    if (compare(point, prev->point, depth) <= 0)
    {
        prev->left = new;
    }
    else
    {
        prev->right = new;
    }
    b2d->size++;
    return true;
}

int compare(Point *p1, Point *p2, size_t depth)
{
    if (depth % 2 == 0)
    {
        double x1 = ptGetx(p1);
        double x2 = ptGetx(p2);
        if (x1 < x2)
        {
            return -1;
        }
        else if (x1 > x2)
        {
            return +1;
        }
    }

    else
    {
        double y1 = ptGety(p1);
        double y2 = ptGety(p2);
        if (y1 < y2)
        {
            return -1;
        }
        else if (y1 > y2)
        {
            return +1;
        }
    }

    return 0;
}

void *bst2dSearch(BST2d *b2d, Point *q)
{
    if (b2d->root == NULL)
    {
        return NULL;
    }
    BNode *n = b2d->root;
    int depth = -1; 
    int cmp;
    bool equal = false;
    while ((!equal) && n != NULL)
    {
        cmp = compare(q, n->point, ++depth);
        if (cmp == 0)
            if (Equal(q, n->point, depth)) 
                equal = true;
        if (!equal && cmp <= 0)
        {
            n = n->left;
        }
        else if (cmp > 0)
        {
            n = n->right;
        }
    }
    if (n == NULL)
    {
        return NULL;
    }
    return n->value;
}

bool Equal(Point *p1, Point *p2, size_t depth)
{
    if (depth % 2 == 0)
    {
        //compare y
        double y1 = ptGety(p1);
        double y2 = ptGety(p2);
        if (y1 == y2)
        {
            return true;
        }
    }

    else
    {
        //compare x
        double x1 = ptGetx(p1);
        double x2 = ptGetx(p2);
        if (x1 == x2)
        {
            return true;
        }
    }

    return false;
}

List *bst2dBallSearch(BST2d *bst2d, Point *q, double r)
{
    List *list = listNew();
    if (list == NULL)
    {
        return NULL;
    }
    if (bst2d->root == NULL)
    {
        return list;
    }

    bool error = false;
    BNode *n = bst2d->root;
    bst2dBallSearchRec(n, q, r, 0, list, &error);
    if (error) 
    {
        listFree(list, false);
        return NULL;
    }
    return list;
}

void bst2dBallSearchRec(BNode *n, Point *q, double r, size_t depth, List *list, bool *error)
{
    if (n == NULL)
    {
        return;
    }

    if (ptSqrDistance(n->point, q) <= (r*r))
    {
        *error = *error || !listInsertLast(list, n->value);
    }
    
    // call its successors if in interval of possible values
    if (continueLeft(n->point, q, r, depth))
    {
        bst2dBallSearchRec(n->left, q, r, depth + 1, list, error);
    }
    if (continueRight(n->point, q, r, depth))
    {
        bst2dBallSearchRec(n->right, q, r, depth + 1, list, error);
    }
}

bool continueLeft(Point *p1, Point *p2, double r, size_t depth)
{
    if (depth % 2 == 0)
    {
        //compare x
        double x1 = ptGetx(p1);
        double x2 = ptGetx(p2);
        if (x2 - r <= x1)
        {
            return true;
        }
    }

    else
    {
        //compare y
        double y1 = ptGety(p1);
        double y2 = ptGety(p2);
        if (y2 - r <= y1)
        {
            return true;
        }
    }

    return false;
}

bool continueRight(Point *p1, Point *p2, double r, size_t depth)
{
    if (depth % 2 == 0)
    {
        double x1 = ptGetx(p1);
        double x2 = ptGetx(p2);
        if (x2 + r > x1)
        {
            return true;
        }
    }

    else
    {
        //compare y
        double y1 = ptGety(p1);
        double y2 = ptGety(p2);
        if (y2 + r > y1)
        {
            return true;
        }
    }

    return false;
}

double bst2dAverageNodeDepth(BST2d *bst2d)
{
    double nodesNumber = (double) bst2d->size;
    if (nodesNumber == 0 || nodesNumber == 1 || bst2d->root == NULL)
		return nodesNumber;

    BNode *n = bst2d->root;
    int totalNodeDepth = bst2dDepthRec(n, 0);
    return totalNodeDepth / nodesNumber;
}

int bst2dDepthRec(BNode *n, int totalNodeDepth)
{
    if (n == NULL)
    {
        return 0;
    }
    return bst2dDepthRec(n->right, totalNodeDepth + 1) + bst2dDepthRec(n->left, totalNodeDepth + 1) + totalNodeDepth;
}
