#ifndef AWE_SYMBINTREE_H
#define AWE_SYMBINTREE_H

#include <stdio.h>
#include <stdlib.h>


#ifdef __cplusplus
   extern "C" {
#endif


/**@name Symetric Binary Tree (Red Black Tree)
    <p>The purpose of the Symetric Binary Tree module is to provide an easy way to 
       insert and delete nodes in a symetric binary tree</p>
 */
/*@{*/


/** return status
 */
enum AWE_SBT_STATUS{
    ///status was ok
    AWE_STATUS_OK,

    ///out of memory error
    AWE_STATUS_MEM_EXHAUSTED,

    ///key already exists
    AWE_STATUS_DUPLICATE_KEY,

    ///key not found
    AWE_STATUS_KEY_NOT_FOUND
};
typedef enum AWE_SBT_STATUS AWE_SBT_STATUS;


/** "color" of a node
 */
enum AWE_SBT_COLOR {
    ///"red" node
    RED,

    ///"black" node
    BLACK
};
typedef enum AWE_SBT_COLOR AWE_SBT_COLOR;


/** symetric binary tree node
 */
struct AWE_SBT_NODE {
    ///"color" of node
    AWE_SBT_COLOR color;

    ///data contained in node
    void *data;

    ///left leaf
    struct AWE_SBT_NODE *left;

    ///right leaf
    struct AWE_SBT_NODE *right;

    ///parent node
    struct AWE_SBT_NODE *parent;
};
typedef struct AWE_SBT_NODE AWE_SBT_NODE;


/** initializes a root node
    @return returns an initialized parent node
 */
AWE_SBT_NODE *awe_sbt_init(void);


/** inserts a new node into a tree
    @param root root node
    @param data data to be put into the tree
    @param compare_func comparison function used to traverse the tree
    @return returns AWE_STATUS_OK unless there was an error
 */
AWE_SBT_STATUS awe_sbt_insert(AWE_SBT_NODE **root, void *data, int (*compare_func)(const void *a, const void *b));


/** deletes a node from a tree
   @param root root node
   @param data data to be deleted from the tree
   @param compare_func comparison function used to traverse the tree
   @param delete_func function used to delete the data
   @return returns AWE_STATUS_OK unless there was an error
 */
AWE_SBT_STATUS awe_sbt_delete(AWE_SBT_NODE **root, void *data, int (*compare_func)(const void *a, const void *b), void (*delete_func)(const void *data));


/** searches a tree for a node
   @param root root node
   @param data data to be found in the tree
   @param compare_func comparison function used to traverse the tree
   @return returns the data that was found, otherwise NULL
 */
void *awe_sbt_find(AWE_SBT_NODE *root, void *data, int (*compare_func)(const void *a, const void *b));


/** destroys an entire tree
   @param root root node
   @param delete_func function used to delete the data
 */
void awe_sbt_destroy(AWE_SBT_NODE **root, void (*delete_func)(const void *data));


/*@}*/


#ifdef __cplusplus
   }
#endif


#endif //AWE_SYMBINTREE_H
