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
#include "cstack.h"
#include "../pretty_io/pretty_io.h"

void print_int(int * theint);

int main(void)
{
    int i;
    titre("TEST DE LA BIBLIOTHEQUE CSTACK");

    CS_raz(&mapile);

    info("Empilement de 10 20 30 40 50 60 70 80");
    for(i = 10; i < 90; i+=10)
        CS_push(&mapile, &i);

    info("Quelques tests");
    info("Pile vide\?:\t%d",CS_empty(&mapile));
    info("Pile pleine\?:\t%d",CS_full(&mapile));

    info("Quelques dépilements");
    info("Dépile [%d]",CS_pop(&mapile));
    info("Encore [%d]",CS_pop(&mapile));
    info("Encore [%d]",CS_pop(&mapile));
    
    info("Etat actuel de la pile");
    CS_print_stack(&mapile, print_int);

    info("5 dépilements de plus");
    for(i = 1; i < 6; i++)
        (void) CS_pop(&mapile);

    info("Pile vide\?:\t%d\n\n", CS_empty(&mapile));

    return EXIT_SUCCESS;
}

void print_int(int * theint)
{
    printf("** %d **",*theint);
}
