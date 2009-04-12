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

#ifndef CBTREE_H
#define CBTREE_H

/*
 * typedef struct
{
    ...
} data;
*/
typedef int data;

typedef struct _node
{
    data ndata; // node data
    struct _node * left;
    struct _node * right;
} node;

typedef enum search_result { NOTFOUND = 0, FOUND } search_result;
typedef enum cmp_result { LT = -1, EQ, GT } cmp_result;

typedef cmp_result (* cmp_func)(data * value, data * key);

node *
make_node(data * new_data);

search_result
lookup(node * bst, data * target, cmp_func compare);

extern void
insert(node ** bt, data * new_data, cmp_func compare);

extern int
size(node * bt);

extern int
max_depth(node * bt);

data *
min_value(node * bst, cmp_func compare);

data *
max_value(node * bst, cmp_func compare);

void
print_tree(node * bst, void (* print_data)());

void
print_post_order(node * bt, void (* print_data)());

int
has_path_sum(node * bt, int value);

void
print_paths(node * bt, void (* print_data)());

void
mirror(node * bt);

void
double_tree(node * bst);

int
same_tree(node * bt_a, node * bt_b, cmp_func compare);

int
count_trees(int num_keys);

int
is_BST_amateur(node * bt, cmp_func compare);

int
is_BST_rec(node * bt, int min, int max);

int
is_BST(node * bt);

int
is_BST_inorder(node * bt);

int
is_BST_inorder_rec(node * bt, int * lv);

#endif /* CBTREE_H */

