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
#include <string.h>
#include "pretty_io.h"

int main(void)
{
    int i = 5, j = 10;
    char *prompt = "Command >", line[100];

    titre("Calculateur de prix TTC");

    info("Une petite info paramétrée par %d et %d", i, j);
    invite("Entrez un phrase (%d mots):", j);
    scanf("%*[^\n]");
    erreur("Une erreur inconnue (code:%d)\n\n", i);
    info("");
    info("Entrez des commandes, exit pour quitter\n");
    do {
        get_line(prompt, line);
    } while(strcmp(line, "exit"));


    return EXIT_SUCCESS;
}

