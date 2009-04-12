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

#include <stdio.h>
#include <stdlib.h>
#include "cbtree.h"
#include "../pretty_io/pretty_io.h"

cmp_result
int_cmp(int * a, int * b);
void
print_int(int * printme);

int main(void)
{
    int i;
    data entiers[8] = {10, 3, 5, 8, 9, 2, 1, 15};
    node * mytree;

    titre("TESTING THE CBTREE LIBRARY");

    info("Building a bst of 8 integers ...");
    mytree = make_node(entiers);
    for(i = 1; i < 8; i++)
        insert(&mytree, entiers + i, int_cmp);

    info("Printing it out ...\n");
    print_tree(mytree, print_int);

    info("Printing it out in post order ...");
    print_post_order(mytree, print_int);

    info("Size =\t%d", size(mytree));

    info("MaxDepth =\t%d", max_depth(mytree));

    info("MinValue =\t%d", *min_value(mytree, int_cmp));

    info("HasPathSum 25 ?\t%d", has_path_sum(mytree, 25));

    info("HasPathSum 15 ?\t%d", has_path_sum(mytree, 15));

    info("IsBST ? (amateur version)\t%d", is_BST_amateur(mytree, int_cmp));

    info("IsBST ? (guru version)\t%d", is_BST(mytree));

    info("IsBST ? (inorder traversal version)\t%d", is_BST_inorder(mytree));

    info("Printing paths ...\n");
    print_paths(mytree, print_int);

    info("Doubling it ...\n");
    double_tree(mytree);
    print_tree(mytree, print_int);

    info("Printing paths ...\n");
    print_paths(mytree, print_int);

    info("Changing it to it\'s image in a mirror ...\n");
    mirror(mytree);
    print_tree(mytree, print_int);

    info("Printing paths ...\n");
    print_paths(mytree, print_int);

    info("How many structurally different bst with 4 different keys?\t%d", count_trees(4));

    info("Bye!\n\n");
    return EXIT_SUCCESS;
}

cmp_result
int_cmp(int * a, int * b)
{
    if(*a == *b)
        return EQ;
    if(*a < *b)
        return LT;
    else
        return GT;
}

void
print_int(int * printme)
{
    printf("\t%d", *printme);
}

