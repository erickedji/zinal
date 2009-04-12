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
#include "../pretty_io/pretty_io.h"
#include "cqueue.h"

void
CQ_reset(CQ_queue * queue)
{
    queue->in = queue->out;
    queue->not_full = 1;
}

int
CQ_full(CQ_queue * queue)
{
    return (!queue->not_full && (queue->in == queue->out));
}

int
CQ_empty(CQ_queue * queue)
{
    return (queue->not_full && (queue->in == queue->out));
}

void
CQ_push(CQ_queue * queue, CQ_element * element)
{
    if(!CQ_full(queue))
    {
        queue->elements[queue->in] = *element;
        queue->in = (queue->in - 1) % CQUEUE_SIZE;
        queue->not_full = 0; // The queue may be full now
    }
}

CQ_element *
CQ_pop(CQ_queue * queue)
{
    CQ_element * poped;

    if(!CQ_empty(queue))
    {
        poped = &(queue->elements[queue->out]);
        queue->out = (queue->out - 1) % CQUEUE_SIZE;
        queue->not_full = 1; // The queue is certainly not full now
        return poped;
    }
}

void
CQ_print(CQ_queue * queue, void (* print_element)())
{
    int i,j;

    if(CQ_empty(queue))
        info("[ Empty Queue]");
    else
    {
        // Print the first element
        info("[ Element 1 ]");
        print_element(&(queue->elements[queue->out]));
        printf("\n--------------------------------------------------");

        // Print the other elements if any
        for(i = ((queue->out - 1) % CQUEUE_SIZE),j = 2;
            i != queue->in;
            i = ((i - 1) % CQUEUE_SIZE),j++)
        {
            info("[ Element %d ]",j);
            print_element(&(queue->elements[i]));
            printf("\n--------------------------------------------------");
        }
    }
}

int
CQ_length(CQ_queue * queue)
{
    if(CQ_full(queue))
        return CQUEUE_SIZE;
    else
    {
        if(queue->out >= queue->in)
            return (queue->out - queue->in);
        else
            return CQUEUE_SIZE - queue->in + queue->out + 1;
    }
}

