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

#include "clist.h"

void
print_data(my_data_type data);

int
main(void)
{
    my_node_type * mylist,
                 * copy_of_mylist;
    my_data_type moinsun = { -1 };

    printf("\n----------------------------------------\n");
    printf("------ Testing the CLIST library -------");
    printf("\n----------------------------------------\n");

    printf("Creating a list of integers from 0 to 10 inclusive...\n");
    mylist = list(11,0,1,2,3,4,5,5,6,7,8,9,10);
    printf("Length:\t%d", length(mylist));

    printf("Printing the list just created ...\n");
    print_list(mylist, print_data);

    printf("Adding -1 to the beginning of the list ...\n");
    mylist = cons(moinsun, &mylist);
    printf("Result:\n");
    print_list(mylist, print_data);

    printf("The length of the list is now :\t%d", length(mylist));

    copy_of_mylist = list_copy(mylist);
    printf("Printing a copy of the list... \n");
    print_list(copy_of_mylist, print_data);

    printf("Appendding mylist to copy_of_mylist ...\n... and printing mylist ...\n");
    append(1,mylist,copy_of_mylist);
    print_list(mylist, print_data);

    printf("The first 3 elements of mylist are ...\n");
    print_list(list_head(mylist,3), print_data);

    printf("The last 5 elements of mylist are ...\n");
    print_list(list_tail(mylist,5), print_data);

    return EXIT_SUCCESS;
}

void
print_data(my_data_type data)
{
    printf("\n\t%d", data.number);
}

