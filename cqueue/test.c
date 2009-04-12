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
#include "cqueue.h"
#include "../pretty_io/pretty_io.h"

void print_element(CQ_element * element);
void push(char element);

CQ_queue mafile;

int main(void)
{
    titre("TEST DE LE BIBLIOTHEQUE CQUEUE");

    CQ_reset(&mafile);
    info("Mise en attente de a c e g i et k:");
    push('a'); push('c'); push('e'); push('g'); push('i'); push('k');

    info("Nombre d'éléménts dans la file:\t%d",CQ_length(&mafile));

    info("Etat de la file:");
    CQ_print(&mafile, print_element);

    info("6 dépilements successifs:");
    (void) CQ_pop(&mafile);
    (void) CQ_pop(&mafile);
    (void) CQ_pop(&mafile);
    (void) CQ_pop(&mafile);
    (void) CQ_pop(&mafile);
    (void) CQ_pop(&mafile);

    info("Pile vide\?:\t%d",CQ_empty(&mafile));
    info("Pile pleine\?:\t%d\n\n",CQ_full(&mafile));
    
    return EXIT_SUCCESS;
}

void print_element(CQ_element * element)
{
    printf(" *** %c *** ",*element);
}

void push(char element)
{
    CQ_push(&mafile, &element);
}

