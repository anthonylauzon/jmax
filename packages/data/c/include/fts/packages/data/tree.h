/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#ifndef _DATA_TREE_H_
#define _DATA_TREE_H_

#include <fts/packages/data/fmat.h>



#define MAX_VECT   200
#define MAX_NODES  400

typedef struct node
{
    int startind;
    int endind;

    fmat_t *mean;
    fmat_t *splitplane;
} node_t;


typedef struct tree
{
    fts_object_t o;

    int ndim;
    int ndata;
	int height;

    fmat_t *data[MAX_VECT];
	
	//Vector index list
	int listind[MAX_VECT];


    int nnodes;
    node_t nodes[MAX_NODES];
} tree_t;


DATA_API fts_symbol_t tree_symbol;
DATA_API fts_class_t *tree_class;

DATA_API void tree_get(tree_t *tree, const fts_atom_t *key, fts_atom_t *atom);

#endif
