#include "linkedlist.h"


/*****************************************************************************
    PUBLIC
 *****************************************************************************/


//inserts a node in a list
void awe_list_insert(AWE_DL_LIST *list, AWE_DL_NODE *node, AWE_DL_NODE *next)
{
    AWE_DL_NODE *prev = next ? next->prev : list->last;
    node->prev = prev;
    node->next = next;
    if (prev) prev->next = node; else list->first = node;
    if (next) next->prev = node; else list->last = node;
}


//removes a node from a list
void awe_list_remove(AWE_DL_LIST *list, AWE_DL_NODE *node)
{
    AWE_DL_NODE *prev = node->prev;
    AWE_DL_NODE *next = node->next;
    if (prev) prev->next = next; else list->first = next;
    if (next) next->prev = prev; else list->last = prev;
    node->prev = node->next = 0;
}


/** returns the index of given node, counting from first of given list
    @param list list to search into
    @param node node to search
    @return index of the given node into the given list, starting from first,
            or -1 if not found
 */
int awe_list_get_node_index_from_first(AWE_DL_LIST *list, AWE_DL_NODE *node)
{
    int result = 0;
    AWE_DL_NODE *t;

    for(t = list->first; t; t = t->next, result++) {
        if (t == node) return result;
    }
    return -1;
}


/** returns the index of given node, counting from last of given list
    @param list list to search into
    @param node node to search
    @return index of the given node into the given list, starting from last,
            or -1 if not found
 */
int awe_list_get_node_index_from_last(AWE_DL_LIST *list, AWE_DL_NODE *node)
{
    int result = 0;
    AWE_DL_NODE *t;

    for(t = list->last; t; t = t->prev, result++) {
        if (t == node) return result;
    }
    return -1;
}


/** returns node of list starting from first node
    @param list list to search into
    @param index index of node
    @return pointer to node at given index, or null if index is invalid
 */
AWE_DL_NODE *awe_list_get_node_from_first(AWE_DL_LIST *list, int index)
{
    AWE_DL_NODE *node;

    if (index < 0) return 0;
    for(node = list->first; node && index; node = node->next, index--);
    return node;
}


/** returns node of list starting from last node
    @param list list to search into
    @param index index of node
    @return pointer to node at given index, or null if index is invalid
 */
AWE_DL_NODE *awe_list_get_node_from_last(AWE_DL_LIST *list, int index)
{
    AWE_DL_NODE *node;

    if (index < 0) return 0;
    for(node = list->last; node && index; node = node->prev, index--);
    return node;
}
