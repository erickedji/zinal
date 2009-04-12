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

#include "cbtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


/*
 * Certain functions require the binary tree to be a binary search tree
 * aka BST. In those functions, the variable representing the tree is
 * named bst. It is named bt when the fonction applies to all binary trees.
 */

/*
 * make_node creates a brand new tree node and returns a pointer to it
 */
node *
make_node(data * new_data)
{
    node * new_node = (node *) malloc(sizeof(node));

    if(new_node != NULL)
    {
        new_node->ndata = *new_data;
        new_node->left  = NULL;
        new_node->right = NULL;
        return new_node;
    } else {
        perror("\n\t!!! Sorry, memory allocation failed");
        return NULL;
    }
}

/*
 * lookup searchs for a target data in a binary search tree using a given
 * comparison function and return the search_result (FOUND|NOTFOUND)
 */
search_result
lookup(node * bst, data * target, cmp_func compare)
{
    cmp_result result;

    if(bst == NULL)
        return NOTFOUND;
    
    result = compare(target, &bst->ndata);
    
    if(result == EQ)
        return FOUND;

    if(result == LT)
        return lookup(bst->left,target,compare);
    else // result = GT
        return lookup(bst->right,target,compare);
}

/*
 * insert takes a pointer to a binary search tree, a pointer to the data to be
 * inserted and inserts it in the binary tree using the provided comparision function
 */
void
insert(node ** bst, data * new_data, cmp_func compare)
{
    if(*bst == NULL)
    {
        *bst = make_node(new_data);
        return;
    } else {
        cmp_result result = compare(new_data, &(*bst)->ndata);

        if(result == GT)
            insert(&(*bst)->right,new_data,compare);
        else // new_data <= node_data
            insert(&(*bst)->left,new_data,compare);
    }
}

/*
 * size counts the number of nodes in a binary tree
 */
int
size(node * bt)
{
    if(bt == NULL)
        return 0;
    else
        return (size(bt->left) + 1 + size(bt->right));
}

/*
 * max_depth computes the number of nodes in the path from the root to
 * the farthest leaf in a binary tree
 */
int
max_depth(node * bt)
{
    if(bt == NULL)
    {
        return 0;
    } else {
        int left_depth  = max_depth(bt->left),
            right_depth = max_depth(bt->right);

        return 1 + ((left_depth > right_depth) ? left_depth : right_depth);
    }
}

/*
 * max_value return a pointer to the data that has the maximum value
 * in a non-empty binary search tree according to a given comparison function
 */
data *
max_value(node * bst, cmp_func compare)
{
    data * result = &bst->ndata;

    bst = bst->right;

    while(bst != NULL)
    {
        result = (compare(&bst->ndata, result) == LT) ? &bst->ndata : result;
        bst = bst->right; // greater values can be found only on the right side
    }

    return result;
}

/*
 * min_value return a pointer to the data that has the minimum value
 * in a non-empty binary search tree according to a given comparison function
 */
data *
min_value(node * bst, cmp_func compare)
{
    data * result = &bst->ndata;

    bst = bst->left;

    while(bst != NULL)
    {
        result = (compare(&bst->ndata, result) == LT) ? &bst->ndata : result;
        bst = bst->left; // smaller values can be found only on the left side
    }

    return result;
}

/*
 * print_tree outputs the data in the nodes of a bst in increasing order,
 * using a given node-data printing function (in-order traversal)
 */
void
print_tree(node * bst, void (* print_data)())
{
    if(bst != NULL)
    {
        print_tree(bst->left, print_data);
        print_data(&bst->ndata);
        print_tree(bst->right, print_data);
    }
}

/*
 * print_pre_order outputs th data in the nodes of a bt using pre-order traversal
 */
void
print_pre_order(node * bt, void (* print_data)())
{
    if(bt != NULL)
    {
        print_data(&bt->ndata);
        print_pre_order(bt->left, print_data);
        print_pre_order(bt->right, print_data);
    }
}

/*
 * print_post_order outputs th data in the nodes of a bt using post-order traversal
 */
void
print_post_order(node * bt, void (* print_data)())
{
    if(bt != NULL)
    {
        print_post_order(bt->left, print_data);
        print_post_order(bt->right, print_data);
        print_data(&bt->ndata);
    }
}

/*
 * has_path_sum searchs a bt for a path such that the sum of its elements is
 * a given value. The data contained in each node is an integer.
 */
int
has_path_sum(node * bt, int value)
{
    int new_value;

    if(bt == NULL) // an empty tree has no path
        return 0;
    
    new_value = value - bt->ndata;

    if(new_value == 0 && bt->left == NULL && bt->right == NULL) // bingo
    {
        return 1;
    } else {
        return (has_path_sum(bt->left, new_value) ||
                has_path_sum(bt->right, new_value));
    }
}

// print_paths_rec is a helper function for print_paths
static void
print_paths_rec(node * bt, data ** path, int path_length, void (* print_data)())
{
    if(bt->left == NULL && bt->right == NULL) // it is a leaf
    {
        int i;

        // print the path so far
        for(i = 0; i < path_length; i++)
            print_data(*(path + i));

        // print the current node (the leaf)
        print_data(&bt->ndata);

        printf("\n");
    } else { // bt is not a leaf
        *(path + path_length) = &bt->ndata;
        path_length ++;
        if(bt->left != NULL)
            print_paths_rec(bt->left, path, path_length, print_data);
        if(bt->right != NULL)
            print_paths_rec(bt->right, path, path_length, print_data);
    }
}

/*
 * print_paths outputs the sequence of data in each root-to-leaf path in a bt,
 * a sequence per line.
 * Pointers to the data in the path so far built are stored in the array 'path'
 */
void
print_paths(node * bt, void (* print_data)())
{
    data ** path    = (data **) malloc(max_depth(bt) * sizeof(data *));
    int path_length = 0;

    if(bt == NULL)
        printf("\n\t--- [Empty tree]");
    else
        print_paths_rec(bt, path, path_length, print_data);
}

/*
 * mirror transforms a bt in its image (as given by a mirror)
 */
void
mirror(node * bt)
{
    node * tmp;

    if(bt != NULL)
    {
        mirror(bt->left);
        mirror(bt->right);

        tmp         = bt->left;
        bt->left    = bt->right;
        bt->right   = tmp;
    }
}

/*
 * double_tree creates a new duplicate node of each node in a bst and inserts
 * the duplicate node as the left-child of the original node.
 */
void
double_tree(node * bst)
{
    node * new_node;

    if(bst != NULL)
    {
        new_node        = make_node(&bst->ndata);
        new_node->left  = bst->left;
        bst->left       = new_node;

        double_tree(new_node->left); // new_node->left == bst->left->left
        double_tree(bst->right);
    }
}

/*
 * same_tree checks if two bt have the same structure (made of nodes having the
 * same data arranged in the same way)
 */
int
same_tree(node * bt_a, node * bt_b, cmp_func compare)
{
    if(bt_a == NULL && bt_b == NULL)
        return 1;

    if((bt_a == NULL && bt_b != NULL) || (bt_a != NULL && bt_b == NULL))
    {
        return 0;
    } else { // each tree is not empty
        if(compare(&bt_a->ndata, &bt_b->ndata) != EQ)
        {
            return 0;
        } else {
            int same_left   = same_tree(bt_a->left, bt_b->left, compare),
                same_right  = same_tree(bt_a->right, bt_b->right, compare);

            if(same_left && same_right)
                return 1;
            else
                return 0;
        }
    }
}

/*
 * count_trees computes the number of structurally different bst
 * that can be made out of n different node keys
 * We assume num_keys is positive
 */
int
count_trees(int num_keys)
{
    if(num_keys <= 1)
    {
        // with 1 key, one can build a unique leaf,
        // with 0 key, one can build the (unique) empty tree
        return 1;
    } else {
        int root, nb_left_subtrees, nb_right_subtrees, count = 0;

        for(root = 1; root <= num_keys; root ++)
        {
            // only 1 ... (root - 1) can go to the left of root
            nb_left_subtrees = count_trees(root - 1);

            // only (root + 1) ... n can go to the right of root
            nb_right_subtrees = count_trees(num_keys - root);

            count += nb_left_subtrees * nb_right_subtrees;
        }

        return count;
    }
}

/*
 * is_BST_amateur check if a given bt is a bst in a very simple manner:
 * at every node, it computes the max value in the left sub-tree and the
 * min value in the right sub-tree and verifies if they comply with the
 * BST requirements
 */
int
is_BST_amateur(node * bt, cmp_func compare)
{
    if(bt == NULL)
    {
        return 1;
    } else {
        // if a value on the left is greater than us , return false
        if(bt->left != NULL
                && compare(max_value(bt->left, compare), &bt->ndata) == GT)
            return 0;
        // if a value on the right is smaller than us , return false
        if(bt->right != NULL
                && compare(min_value(bt->right, compare), &bt->ndata) != GT)
            return 0;
        // if a subtree is not a binary tree, return false
        if(!is_BST_amateur(bt->left, compare)
                || !is_BST_amateur(bt->right, compare))
            return 0;

        // Having succesfully passed all these test, it is a binary tree
        return 1;
    }
}

// is_BST_rec is a helper function for is_BST
int
is_BST_rec(node * bt, int min, int max)
{
    if(bt == NULL)
        return 1;

    if(bt->ndata < min || bt->ndata > max)
        return 0;

    return (is_BST_rec(bt->left, min, bt->ndata)
            && is_BST_rec(bt->right, bt->ndata + 1, max));
}

/*
 * is_BST checks if a given bt is a bst in an efficient manner
 * It works only for trees containing integers
 */
int
is_BST(node * bt)
{
    return (is_BST_rec(bt, INT_MIN, INT_MAX));
}

/*
 * is_BST_inorder checks if a given bt is a bst using the fact that
 * a bt is a bst if and only if an inorder traversal yield values
 * arranged in increased order
 */
int
is_BST_inorder(node * bt)
{
    // lv is a pointer to the last value in the ordered list of node values
    int value = INT_MIN, * lv;

    lv = &value;
    
    return is_BST_inorder_rec(bt, lv);
}

/*
 * is_BST_inorder_rec is a helper function for is_BST_inorder. It checks if
 * a given subtree is a bst
 */

int is_BST_inorder_rec(node * bt, int * lv)
{
    if(bt == NULL)
        return 1; // an empty tree is a bst

    if((!is_BST_inorder_rec(bt->left, lv)) || (*lv > bt->ndata))
    {
        return 0;
    } else {
        // insert (virtually) a.ndata in the list and update
        // the last value
        *lv = bt->ndata;
        return (is_BST_inorder_rec(bt->right, lv));
    }
}
