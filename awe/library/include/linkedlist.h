#ifndef AWE_LINKEDLIST_H
#define AWE_LINKEDLIST_H


#ifdef __cplusplus
    extern "C" {
#endif


/**@name LinkedList
    The purpose of the this little 'module' is to provide double-linked list
    functionality.
 */
/*@{*/


/** a double-linked list node
 */
struct AWE_DL_NODE {
    ///pointer to previous node
    struct AWE_DL_NODE *prev;

    ///pointer to next node
    struct AWE_DL_NODE *next;
};
typedef struct AWE_DL_NODE AWE_DL_NODE;


/** a double-linked list node with a 'data' void pointer
 */
struct AWE_DL_DATA_NODE {
    ///node
    AWE_DL_NODE node;

    ///pointer to data
    void *data;
};
typedef struct AWE_DL_DATA_NODE AWE_DL_DATA_NODE;


/** a double-linked list
 */
struct AWE_DL_LIST {
    ///pointer to first node
    AWE_DL_NODE *first;

    ///pointer to last node
    AWE_DL_NODE *last;
};
typedef struct AWE_DL_LIST AWE_DL_LIST;


/** inserts a node in a list
    @param list list to insert the node into
    @param node node to insert
    @param next next node; if NULL, then the new node is appended to the list
 */
void awe_list_insert(AWE_DL_LIST *list, AWE_DL_NODE *node, AWE_DL_NODE *next);


/** removes a node from a list
    @param list list to remove a node from
    @param node node to remove
 */
void awe_list_remove(AWE_DL_LIST *list, AWE_DL_NODE *node);


/** returns the index of given node, counting from first of given list
    @param list list to search into
    @param node node to search
    @return index of the given node into the given list, starting from first,
            or -1 if not found
 */
int awe_list_get_node_index_from_first(AWE_DL_LIST *list, AWE_DL_NODE *node);


/** returns the index of given node, counting from last of given list
    @param list list to search into
    @param node node to search
    @return index of the given node into the given list, starting from last,
            or -1 if not found
 */
int awe_list_get_node_index_from_last(AWE_DL_LIST *list, AWE_DL_NODE *node);


/** returns node of list starting from first node
    @param list list to search into
    @param index index of node
    @return pointer to node at given index, or null if index is invalid
 */
AWE_DL_NODE *awe_list_get_node_from_first(AWE_DL_LIST *list, int index);


/** returns node of list starting from last node
    @param list list to search into
    @param index index of node
    @return pointer to node at given index, or null if index is invalid
 */
AWE_DL_NODE *awe_list_get_node_from_last(AWE_DL_LIST *list, int index);


/*@}*/


#ifdef __cplusplus
    }
#endif


#endif //AWE_LINKEDLIST_H
