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

#ifndef CSTACK_H
#define CSTACK_H

#define CSTACK_SIZE 20

typedef int CS_element;

typedef struct _CS_stack{
    CS_element elements[CSTACK_SIZE];
    int top;
} CS_stack;

CS_stack mapile;

extern void CS_raz(CS_stack * stack);

extern int CS_empty(CS_stack * stack);
extern int CS_full(CS_stack * stack);

extern void CS_push(CS_stack * stack, CS_element * element);
extern CS_element CS_pop(CS_stack * stack);

void CS_print_stack(CS_stack * stack, void (* print_element)());

#endif /* CSTACK_H */

