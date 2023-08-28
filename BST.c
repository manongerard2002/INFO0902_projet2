/* ========================================================================= *
 * BST definition
 * ========================================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#include "BST.h"
#include "List.h"

/* Opaque Structure */

typedef struct BNode_t BNode;

struct BNode_t
{
    BNode *parent;
    BNode *left;
    BNode *right;
    void *key;
    void *value;
};

struct BST_t
{
    BNode *root;
    size_t size;
    int (*compfn)(void *, void *);
};

/* Prototypes of static functions */

static void bstFreeRec(BNode *n, bool freeKey, bool freeValue);
static BNode *bnNew(void *key, void *value);


/* Function definitions */

/* ------------------------------------------------------------------------- *
 * Computes the depth of a node inside a BST.
 *
 * PARAMETERS
 * n          			A valid pointer to a node object
 * totalNodeDepth     	The depth of the parent of the node.
 
 * RETURN
 * bstDepthRec          The depth of the node.
 *
 * ------------------------------------------------------------------------- */
int bstDepthRec(BNode *n, size_t totalNodeDepth);

/* ------------------------------------------------------------------------- *
 * Performs an infix BST tree walk and inserts the searched values (in order)
 * in the provided list.
 *
 * PARAMETERS
 * bst          A valid pointer to a BST object.
 * n          	A valid pointer to a node object.
 * l	    	A valid pointer to a list object.
 * keymin		A valid pointer to the minimum value of the search range.
 * keymax		A valid pointer to the maximum value of the search range.
 *
 * ------------------------------------------------------------------------- */
void inOrderTreeWalk(BST* bst, BNode* n, List* l, void *keymin, void *keymax);

BNode *bnNew(void *key, void *value)
{
    BNode *n = malloc(sizeof(BNode));
    if (n == NULL)
    {
        printf("bnNew: allocation error\n");
        return NULL;
    }
    n->left = NULL;
    n->right = NULL;
    n->key = key;
    n->value = value;
    return n;
}

BST *bstNew(int comparison_fn_t(void *, void *))
{
    BST *bst = malloc(sizeof(BST));
    if (bst == NULL)
    {
        printf("bestNew: allocation error");
        return NULL;
    }
    bst->root = NULL;
    bst->size = 0;
    bst->compfn = comparison_fn_t;
    return bst;
}

void bstFree(BST *bst, bool freeKey, bool freeValue)
{
    bstFreeRec(bst->root, freeKey, freeValue);
    free(bst);
}

void bstFreeRec(BNode *n, bool freeKey, bool freeValue)
{
    if (n == NULL)
        return;
    bstFreeRec(n->left, freeKey, freeValue);
    bstFreeRec(n->right, freeKey, freeValue);
    if (freeKey)
        free(n->key);
    if (freeValue)
        free(n->value);
    free(n);
}

size_t bstSize(BST *bst)
{
    return bst->size;
}

bool bstInsert(BST *bst, void *key, void *value)
{
    if (bst->root == NULL)
    {
        bst->root = bnNew(key, value);
        if (bst->root == NULL)
        {
            return false;
        }
        bst->size++;
        return true;
    }
    BNode *prev = NULL;
    BNode *n = bst->root;
    while (n != NULL)
    {
        prev = n;
        int cmp = bst->compfn(key, n->key);
        if (cmp <= 0)
        {
            n = n->left;
        }
        else if (cmp > 0)
        {
            n = n->right;
        }
    }
    BNode *new = bnNew(key, value);
    if (new == NULL)
    {
        return false;
    }
    new->parent = prev;
    if (bst->compfn(key, prev->key) <= 0)
    {
        prev->left = new;
    }
    else
    {
        prev->right = new;
    }
    bst->size++;
    return true;
}

void *bstSearch(BST *bst, void *key)
{
    BNode *n = bst->root;
    while (n != NULL)
    {
        int cmp = bst->compfn(key, n->key);
        if (cmp < 0)
        {
            n = n->left;
        }
        else if (cmp > 0)
        {
            n = n->right;
        }
        else
        {
            return n->value;
        }
    }
    return NULL;
}

int bstDepthRec(BNode *n, size_t totalNodeDepth)
{
    if (n == NULL)
    {
        return 0;
    }
    return bstDepthRec(n->right, totalNodeDepth + 1) + bstDepthRec(n->left, totalNodeDepth + 1) + totalNodeDepth;
}

double bstAverageNodeDepth(BST *bst)
{
	double nodesNumber = (double) bst->size;
	
	if (nodesNumber == 0 || nodesNumber == 1 || bst->root == NULL)
		return nodesNumber;
	
    BNode *n = bst->root;
    int totalNodeDepth = bstDepthRec(n, 0);
	return (totalNodeDepth/nodesNumber);
}

void inOrderTreeWalk(BST* bst, BNode* n, List* l,  void *keymin, void *keymax)
{
    if(n == NULL)
        return;

    void *key = n->key;
	int comp_keymin_key = bst->compfn(keymin, key);
	int comp_keymax_key = bst->compfn(keymax, key);
	
	//Tree path left (recursive call)
	if (comp_keymin_key <= 0)
         inOrderTreeWalk(bst, n->left, l, keymin, keymax);
 		
	// Insert the value corresponding to the key only if keymin <= key <= keymax
	if (comp_keymin_key <= 0 && comp_keymax_key >= 0)
        listInsertLast(l, n->value);

	// Tree path right (recursive call)
	if (comp_keymax_key > 0)
        inOrderTreeWalk(bst, n->right, l, keymin, keymax);
}

List *bstRangeSearch(BST *bst, void *keymin, void *keymax)
{
	List *l = listNew();
    if (l == NULL)
	{
        return NULL;
	}
	
	BNode* root = bst->root;
	int comp_keymin_keymax = bst->compfn(keymin, keymax);

	//case 1 : If keymin > keymax or binarySearchTree is empty, return an empty list
	if (comp_keymin_keymax > 0 || root == NULL)
		return l;
	
	//case 2 : If keymin = keymax, search for the exact node inside the binarySearchTree
	if (comp_keymin_keymax == 0)
    {
		BNode* n = bstSearch(bst, keymin);
		if(n != NULL)
			listInsertFirst(l, n->value);
		return l;
	}
	
	//case 3 
	inOrderTreeWalk(bst, root, l, keymin, keymax);

	return l;
}