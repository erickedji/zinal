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
#include "menug.h"
#include "../pretty_io/pretty_io.h"

void menu(void);
void dummy1(void);
void dummy2(void);
void dummy3(void);

int main(void)
{
    titre("TEST DE LA FONCTION DE MENU GENERIQUE");

    menu();

    return(EXIT_SUCCESS);
}

void menu(void)
{
    menug(4,"Option 1", dummy1,
    "Option 2", dummy2,
    "Option 3", dummy3,
    "Quitter", MG_quitter);
}

void dummy1(void)
{
    info("Appel de la fonction 1");

    menu();
}

void dummy2(void)
{
    info("Appel de la fonction 2\n");

    menu();
}

void dummy3(void)
{
    info("Appel de la fonction 3\n");

    menu();
}

