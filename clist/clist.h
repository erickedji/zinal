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

#ifndef CLIST_H
#define CLIST_H

/*typedef struct _hdata{
	...
 my_hdata_type;*/

/*typedef struct _head{
	my_hdata_type my_hdata_var;
	my_node_type * next;
} my_head_type;*/

typedef struct _data{
	int number;
} my_data_type;

typedef struct _node{
	my_data_type my_data_var;
	struct _node * next;
} my_node_type;

#define CL_NODE_TYPE my_node_type
#define CL_DATA_TYPE my_data_type
#define CL_DATA_VAR  my_data_var

//These 3 lines are no more relevant, simple clist and enhanced clist will be separated
/*#define CL_HEAD_TYPE  CL_NODE_TYPE
#define CL_HDATA_TYPE CL_DATA_TYPE
#define CL_HDATA_VAR  CL_DATA_VAR*/


extern CL_NODE_TYPE *
list(int node_count, ... );

extern void
print_list(CL_NODE_TYPE * thelist, void (* print_data)(CL_DATA_TYPE data));

extern CL_NODE_TYPE *
list_copy(CL_NODE_TYPE * thelist);

extern int
length(CL_NODE_TYPE * thelist);

extern CL_NODE_TYPE *
cons(CL_DATA_TYPE data, CL_NODE_TYPE ** list_pointer);

extern CL_NODE_TYPE *
pointer_to_last(CL_NODE_TYPE * thelist);

extern int 
append(int list_count, CL_NODE_TYPE * first_list, ...);

extern CL_DATA_TYPE
list_ref(CL_NODE_TYPE * thelist, int index);

extern CL_NODE_TYPE *
sublist(CL_NODE_TYPE * thelist, int start, int end);

extern CL_NODE_TYPE *
list_head(CL_NODE_TYPE * thelist, int k);

extern CL_NODE_TYPE *
list_tail(CL_NODE_TYPE * thelist, int k);

extern void
malloc_failed(char * which_function, char * where);


#endif /* CLIST_H */

