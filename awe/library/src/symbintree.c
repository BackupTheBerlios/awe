#include "symbintree.h"

/*****************************************************************************
    PRIVATE
 *****************************************************************************/


#define NIL &_sentinel       /* all leafs are sentinels */


//leaf node
static AWE_SBT_NODE _sentinel = { BLACK, NIL, NIL, NIL, NULL };


//rotates a subtree to the left
static void _sbt_rotate_left(AWE_SBT_NODE **root, AWE_SBT_NODE *x) {

    /*************************
    *  rotate node x to left *
    **************************/

    AWE_SBT_NODE *y = x->right;

    /* establish x->right link */
    x->right = y->left;
    if (y->left != NIL) y->left->parent = x;

    /* establish y->parent link */
    if (y != NIL) y->parent = x->parent;
    if (x->parent) {
        if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
    } 
    else {
        *root = y;
    }

    /* link x and y */
    y->left = x;
    if (x != NIL) x->parent = y;
}


//rotates a subtree to the right
static void _sbt_rotate_right(AWE_SBT_NODE **root, AWE_SBT_NODE *x) {

    /***************************
    *  rotate node x to right  *
    ****************************/

    AWE_SBT_NODE *y = x->left;

    /* establish x->left link */
    x->left = y->right;
    if (y->right != NIL) y->right->parent = x;

    /* establish y->parent link */
    if (y != NIL) y->parent = x->parent;
    if (x->parent) {
        if (x == x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;
    } 
    else {
        *root = y;
    }

    /* link x and y */
    y->right = x;
    if (x != NIL) x->parent = y;
}


//inserts a new node into a tree and fixes the symetry
static void _sbt_insert_fixup(AWE_SBT_NODE **root, AWE_SBT_NODE *x) {

    /************************************
    *  maintain Red-Black tree balance  *
    *  after inserting node x           *
    *************************************/

    /* check Red-Black properties */
    while (x != *root && x->parent->color == RED) {
        /* we have a violation */
        if (x->parent == x->parent->parent->left) {
            AWE_SBT_NODE *y = x->parent->parent->right;
            if (y->color == RED) {

                /* uncle is RED */
                x->parent->color = BLACK;
                y->color = BLACK;
                x->parent->parent->color = RED;
                x = x->parent->parent;
            }
            else {

                /* uncle is BLACK */
                if (x == x->parent->right) {
                    /* make x a left child */
                    x = x->parent;
                    _sbt_rotate_left(root, x);
                }

                /* recolor and rotate */
                x->parent->color = BLACK;
                x->parent->parent->color = RED;
                _sbt_rotate_right(root, x->parent->parent);
            }
        } 
        else {

            /* mirror image of above code */
            AWE_SBT_NODE *y = x->parent->parent->left;
            if (y->color == RED) {

                /* uncle is RED */
                x->parent->color = BLACK;
                y->color = BLACK;
                x->parent->parent->color = RED;
                x = x->parent->parent;
            }
            else {

                /* uncle is BLACK */
                if (x == x->parent->left) {
                    x = x->parent;
                    _sbt_rotate_right(root, x);
                }
                x->parent->color = BLACK;
                x->parent->parent->color = RED;
                _sbt_rotate_left(root, x->parent->parent);
            }
        }
    }
    (*root)->color = BLACK;
}


//deletes a node from a tree and fixes the symetry
static void _sbt_delete_fixup(AWE_SBT_NODE **root, AWE_SBT_NODE *x) {

    /************************************
    *  maintain Red-Black tree balance  *
    *  after deleting node x            *
    *************************************/

    while (x != *root && x->color == BLACK) {
        if (x == x->parent->left) {
            AWE_SBT_NODE *w = x->parent->right;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                _sbt_rotate_left (root, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } 
            else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    _sbt_rotate_right (root, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                _sbt_rotate_left (root, x->parent);
                x = *root;
            }
        } 
        else {
            AWE_SBT_NODE *w = x->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                _sbt_rotate_right (root, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } 
            else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    _sbt_rotate_left (root, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                _sbt_rotate_right (root, x->parent);
                x = *root;
            }
        }
    }
    x->color = BLACK;
}


//helper callback that simply returns 0 for a comparison
static int _sbt_delete_all(const void *a, const void *b){
    return 0;    
}


/*****************************************************************************
    PUBLIC
 *****************************************************************************/


//initializes a root node
AWE_SBT_NODE *awe_sbt_init(void) {
    return NIL;
}


//inserts a new node into a tree
AWE_SBT_STATUS awe_sbt_insert(AWE_SBT_NODE **root, void *data, int (*compare_func)(const void *a, const void *b)) {
    AWE_SBT_NODE *current, *parent, *x;
    int ret;

    /**********************************************
    *  allocate node for data and insert in tree  *
    ***********************************************/

    /* find future parent */
    current = *root;
    parent = NULL;
    while (current != NIL) {
        ret = compare_func(data, current->data);
        if (ret == 0)
            return AWE_STATUS_DUPLICATE_KEY;
        parent = current;
        if (ret < 0)
            current = current->left;
        else
            current = current->right;
    }

    /* setup new node */
    if ((x = (AWE_SBT_NODE*)malloc(sizeof(AWE_SBT_NODE))) == NULL)
        return AWE_STATUS_MEM_EXHAUSTED;
    x->parent = parent;
    x->left = NIL;
    x->right = NIL;
    x->color = RED;
    x->data = data;

    /* insert node in tree */
    if(parent) {
        if (compare_func(data, parent->data) < 0)
            parent->left = x;
        else
            parent->right = x;
    }
    else {
        *root = x;
    }

    _sbt_insert_fixup(root, x);

    return AWE_STATUS_OK;
}


//deletes a node from a tree
AWE_SBT_STATUS awe_sbt_delete(AWE_SBT_NODE **root, void *data, int (*compare_func)(const void *a, const void *b), void (*delete_func)(const void *data)) {
    AWE_SBT_NODE *x, *y, *z;
    int ret;
    
    /****************************
    *  delete node z from tree  *

    *****************************/

    /* find node in tree */
    z = *root;
    while(z != NIL) {
        ret = compare_func(data, z->data);
        if (ret == 0)
            break;
        else if (ret < 0)
            z = z->left;
        else
            z = z->right;
    }
    if (z == NIL) return AWE_STATUS_KEY_NOT_FOUND;
    
    /* delete the data */
    delete_func(z->data);

    if (z->left == NIL || z->right == NIL) {
        /* y has a NIL node as a child */
        y = z;
    } 
    else {
        /* find tree successor with a NIL node as a child */
        y = z->right;
        while (y->left != NIL) y = y->left;
    }

    /* x is y's only child */
    if (y->left != NIL)
        x = y->left;
    else
        x = y->right;

    /* remove y from the parent chain */
    x->parent = y->parent;
    if (y->parent)
        if (y == y->parent->left)
            y->parent->left = x;
        else
            y->parent->right = x;
    else
        *root = x;

    if (y != z)
        z->data = y->data;
    
    if (y->color == BLACK)
        _sbt_delete_fixup (root, x);

    /* delete the node */
    free(y);

    return AWE_STATUS_OK;
}


//searches for a node in a tree
void *awe_sbt_find(AWE_SBT_NODE *root, void *data, int (*compare_func)(const void *a, const void *b)) {

    /******************************
    *  find node containing data  *
    *******************************/

    AWE_SBT_NODE *current = root;
    int ret;
    while(current != NIL) {
    ret = compare_func(data, current->data);
    if(ret == 0)
        return current->data;
    else if (ret < 0)
        current = current->left;
    else
        current = current->right;
    }
    return NULL;
}


//destroys an entire tree
void awe_sbt_destroy(AWE_SBT_NODE **root, void (*delete_func)(const void *data)){
    while(awe_sbt_delete(root, NULL, _sbt_delete_all, delete_func) == AWE_STATUS_OK);
}
