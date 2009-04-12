/*
 * ZINAL (Zinal Is Not A Library) - A collection of a few programs
 * written by a dwarf standing on the shoulders of giants.
 *
 * Copyleft (C) 2007 Eric KEDJI <eric_kedji@yahoo.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/*
 * --- CLIST ---
 *
 * A library for simple list manipulation in C 
 * (bringing a little bit of lisp power to C!)
 *
 * Author:  KEDJI Komlan Akpédjé
 * Date:    06 January 2006, 16:30.
 * Licence: GPL (see the file COPYING)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "clist.h"

CL_DATA_TYPE dummy; /* I want all the calls to malloc to be located
                       in list() to ease the design of the garbage
                       collector. So each time I need a node, I ask
                       list() to create a list with this dummy data
                       and then I fill the node with the appropriate
i                       data - Maybe there is a more elegant hack ...*/


/**
 * list() builds a fresh linked list from a list of data
 * @param       {int} node_count The number of elements,
 * @param       {CL_DATA_TYPE ...} ... the actual elements
 *              in the desired order
 * @return      {CL_NODE_TYPE *} The freshly allocated linked list
 */
CL_NODE_TYPE *
list(int node_count, ...)
{
    va_list         ap;
    CL_NODE_TYPE *  last_node_added;
    CL_NODE_TYPE *  result; 
    int             i;

    // Readily return NULL if the node count is not valid or zero
    if(node_count < 1)
            return NULL;

    // Initialize the variable argument list (va_list) ...
    va_start(ap, node_count);

    // Allocate memory for the first element ...
    result              = (CL_NODE_TYPE *) malloc(sizeof (CL_NODE_TYPE));
    if(result == NULL)
    {
        malloc_failed("list()","Allocate memory for the first element");
        return NULL;
    }

    // Add the first element ...
    result->CL_DATA_VAR = va_arg(ap, CL_DATA_TYPE);
    last_node_added     = result;   
    
    // Add the remaining elements if any ...
    for(i = 1; i < node_count; i++)
    {
        last_node_added->next   = (CL_NODE_TYPE *) malloc(sizeof (CL_NODE_TYPE));
        if(last_node_added->next == NULL)
        {
            malloc_failed("list()","Allocate memory for a node in for loop");
            return NULL;
        }
        (last_node_added->next)->CL_DATA_VAR    = va_arg(ap, CL_DATA_TYPE);
        last_node_added                         = last_node_added->next;
    }

    // NULL-terminate the list.
    last_node_added->next = NULL;

    va_end(ap);

    return result;
}

/**
 * list_copy() ... (oh surprise!) creates a freshly allocated copy of
 * a linked list
 * @param       {CL_NODE_TYPE *} thelist The list to copy
 */
CL_NODE_TYPE *
list_copy(CL_NODE_TYPE * thelist)
i{
    CL_NODE_TYPE    * result        = NULL,
                    * new_node,     // a new node *in* the result
                    * last_added,   // last node added to *result*
                    * node_to_add;  // the next node *in thelist* to
                                    // add to the result

    if(thelist != NULL)
    {// Readily copy the first node then set the loop variables
        result      = list(1,thelist->CL_DATA_VAR);
        last_added  = result;
        node_to_add = thelist->next;

        // copy the other nodes if any
        while(node_to_add != NULL)        
        {
            new_node            = list(1,node_to_add->CL_DATA_VAR);
            last_added->next    = new_node;

            last_added          = new_node;
            node_to_add         = node_to_add->next;
        }
    }

    return result;
}

/**
 * length() computes the number of nodes (elements) in a linked list
 * @param       {CL_NODE_TYPE *} thelist The linked list
 * @return      {int} The computed length [ :-) What a good comment!]
 */
int
length(CL_NODE_TYPE * thelist)
{
    int length = 0;
    while(thelist != NULL)
    {
        length++;
        thelist = thelist->next;
    }

    return length;
}

/**
 * print_list() gives a visual representation of a linked list
 * @param       {CL_NODE_TYPE *} thelist The linked list to print
 * @param       {(*)()} print_data A pointer to the funtion
 *              that gives a visual representation of node data
 */
void
print_list(CL_NODE_TYPE * thelist, void (* print_data)(CL_DATA_TYPE data))
{
    if(thelist == NULL)
    {
        printf("\n[Empty List]\n");
    } else {
        while(thelist != NULL)
        {
            print_data(thelist->CL_DATA_VAR);
            printf("\n----------------------------------------");
            thelist = thelist->next;
        }
    }
}

/**
 * cons() adds a new node at the beginning of a linked list
 * @param       {CL_DATA_TYPE} data The data structure of the node to add
 * @param       {CL_NODE_TYPE **} list_pointer A pointer to the linked list
 * @requires    list()
 * @return      {CL_NODE_TYPE *} The resulting linked list
 * @note        The operation is carried *on the list*, non on a *copy*
 */
CL_NODE_TYPE *
cons(CL_DATA_TYPE data, CL_NODE_TYPE ** list_pointer)
{
    CL_NODE_TYPE * new_node;

    new_node            = list(1,data);

    if(*list_pointer == NULL)
        new_node->next  = NULL;
    else
        new_node->next  = *list_pointer;
    
    *list_pointer   = new_node;

    return *list_pointer;
}

/**
 * pointer_to_last() finds the address of the last element
 * in a given linked list
 * @param       {CL_NODE_TYPE *} thelist The linked list
 * @return      {CL_NODE_TYPE *} The adress of the last element
 * (NULL if the list is empty)
 */
CL_NODE_TYPE *
pointer_to_last(CL_NODE_TYPE * thelist)
{
    if(thelist != NULL)
    {
        while(thelist->next != NULL)
            thelist = thelist->next;
    }
    
    return thelist;
}

/**
 * append() ... appends :-) the elements of a list of linked lists
 * to a given linked list (the first one) in the given order
 *      e.g.:
 *      append(2,list(2,1,2),&list(3,2,2,3),&list(4,10,20,30,40))
 *                      ==> (1,2,3,2,2,3,10,20,30,40)
 *      (remember: the first arg of list() is the number of elements)
 * @param       {int} list_count The number of lists structures
 * to append to the first one
 * @param       {CL_NODE_TYPE *} first_list The linked list to append to
 * @param       {CL_NODE_TYPE * ...} ... The list of *pointers* to the
 * linked lists to append
 * @requires    length(),pointer_to_last()
 * @return      The number of elements in the resulting linked list
 */
int
append(int list_count, CL_NODE_TYPE * first_list, ...)
{
    va_list         ap;
    CL_NODE_TYPE    * last_element,
                    * current_list;
    int             element_count = 0; // Final number of elements in first_list


    // Readily return the length of the first list  if the list count is 0 or invalid
    if(list_count < 0)
        return length(first_list);

    // Get the last element of the first list
    // so we can change it's next-pointer
    last_element    = pointer_to_last(first_list);

    va_start(ap, first_list);

    while(list_count > 0)
    {
        current_list    = va_arg(ap, CL_NODE_TYPE *);

        if(current_list == NULL)
        {
            list_count --;
            continue;
        }

        last_element->next  = current_list;
        last_element        = pointer_to_last(current_list);

        element_count       += length(current_list);
        list_count --;
    }
    
    va_end(ap);
    return element_count;
}

/**
 * list_ref() finds the value of the element at a given position
 * in a linked list (using zero-origin indexing)
 * @param       {CL_NODE_TYPE *} thelist The linked list
 * @param       {int} index The desired index
 * @note        list_ref() does not check if the length of the list is
 *              greater than the index, it is the caller's duty!
 */
CL_DATA_TYPE
list_ref(CL_NODE_TYPE * thelist, int index)
{
    while(index > 0)
    {
        thelist = thelist->next;
        index --;
    }

    return thelist->CL_DATA_VAR;
}

/** 
 * sublist() extracts a sublist from a linked list
 * @param       {CL_NODE_TYPE *} thelist The linked list to operate on
 * @param       {int} start The index to start from
 * @param       {int} stop The index to stop at
 * @requires    list()
 * @return      {CL_NODE_TYPE *} A copy of the sublist
 * @note        Zero-origin indexing is used, the sublist is
 *              from start *inclusive* to stop *exclusive*
 */
CL_NODE_TYPE *
sublist(CL_NODE_TYPE * thelist, int start, int stop)
{
    CL_NODE_TYPE    *result         = NULL,
                    *node_to_add    = thelist,
                    *new_node,
                    *last_added; // the last node added to the result
    int             i = 0;

    if((thelist != NULL) && (start > stop)) // basic sanity test!
    {
        // Search the first node to add ...
        while(i < start)
        {
            node_to_add = node_to_add->next;
            i++;
        }

        // ... add it (the first node)
        result              = list(1, node_to_add->CL_DATA_VAR);

        last_added          = result;
        node_to_add         = node_to_add->next;
        start ++ ;           //  we've allready added the first element
        
        while(start < stop)
        {
            //  create a new node and link it...
            new_node                = list(1, node_to_add->CL_DATA_VAR);
            last_added->next        = new_node;

            //  ... refresh the loop variables.
            last_added              = new_node;
            node_to_add             = node_to_add->next;
            start ++;
        }

        //  a non-null-terminated list is not a list!
        last_added->next = NULL;
    }

    return result;
}

/**
 * list_head() creates a linked list containing the first
 * k elements of a linked list
 * @param       {CL_NODE_TYPE *} thelist The linked list to operate on
 * @param       {int} k The number of elements to copy
 * @return      {CL_NODE_TYPE *} The list created
 * @requires    sublist()
 */
CL_NODE_TYPE *
list_head(CL_NODE_TYPE * thelist, int k)
{
    return sublist(thelist, 0, k);
}

/**
 * list_tail() creates a linked list containing the last
 * k elements of a linked list
 * @param       {CL_NODE_TYPE *} thelist The linked list to operate on
 * @param       {int} k The number of elements to copy
 * @return      {CL_NODE_TYPE *} The list created
 * @requires    sublist()
 */
CL_NODE_TYPE *
list_tail(CL_NODE_TYPE * thelist, int k)
{
    int list_length = length(thelist);

    return sublist(thelist, list_length - k + 1,list_length);
}

/**
 * malloc_failed() is called upon a memory allocation failure.It prints
 * some debugging infomation and ends the program (can't we find a
 * softer way to handle the exception?)
 * @param       {char *} which_function The current function
 * @param       {char *} where The exact context
 */
void
malloc_failed(char * which_function, char * where)
{
    printf("\n!!!\tMemory Allocation error in function:\t%s",which_function);
    printf("\n   \tIt happenned here:\t%s",where);
    perror("\n   \tSystem said ");
    exit(EXIT_FAILURE);
}

