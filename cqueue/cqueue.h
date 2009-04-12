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
 
#ifndef C_QUEUE_H
#define C_QUEUE_H

#define CQUEUE_SIZE 100

typedef char CQ_element;
typedef struct _queue{
    CQ_element elements[CQUEUE_SIZE];
    int in,     /* Where to put the next element */
        out;    /* Where to delete next element*/ 
    short not_full; /* non zero if the queue can't be full
                       ie after a deletion */
} CQ_queue;

void
CQ_reset(CQ_queue * queue);

int
CQ_full(CQ_queue * queue);

int
CQ_empty(CQ_queue * queue);

void
CQ_push(CQ_queue * queue, CQ_element * element);

CQ_element *
CQ_pop(CQ_queue * queue);

void
CQ_print(CQ_queue * queue, void (* print_element)());

int
CQ_length(CQ_queue * queue);


#endif /* C_QUEUE_H */

