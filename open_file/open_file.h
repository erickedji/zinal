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

#ifndef OPEN_FILE_H
#define OPEN_FILE_H
#include <stdio.h>

extern char * quel_nom(void);
extern int fichier_existe(char * nom_fichier);
extern FILE * open_r(char * nom_fichier, int plus);
extern FILE * open_w(char * nom_fichier, int plus);
extern FILE * open_a(char * nom_fichier, int plus);

extern int OF_abandon;

#endif /* OPEN_FILE_H */

