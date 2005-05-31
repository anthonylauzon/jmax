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

#include <fts/fts.h>
#include <fts/packages/data/data.h>
#include <fts/packages/data/tree.h>

fts_symbol_t tree_symbol = 0;
fts_class_t *tree_class  = 0;


/* for the search algorithm */
typedef struct stack_elem
{
    int		node;
    double	dist;
} stack_elem_t;

static void stack_push (fts_stack_t *s, int p, double d)
{
    stack_elem_t _new = { p, d }; 
    fts_stack_push(s, stack_elem_t, _new); 
}

static int stack_pop (fts_stack_t *s, int *p, double *d)
{
    int top = fts_stack_top(s);

    if (top >= 0)
    {
	stack_elem_t *_tmp = ((stack_elem_t *) fts_stack_base(s)) + top; 

	*p = _tmp->node; 
	*d = _tmp->dist; 

	fts_stack_pop(s, 1);
	return 1;
    }
    else
	return 0;
}


static double vec_dist (fmat_t *a, fmat_t *b)
{
    int    m 	= fmat_get_m(a);
    int    n 	= fmat_get_n(a);
    float *l 	= fmat_get_ptr(a);
    float *r 	= fmat_get_ptr(b);
    int    size = m * n;
    double dist = 0;
    int    i;
    
    for (i = 0; i < size; i++)
	dist += (l[i] - r[i]) * (l[i] - r[i]);

    return dist;    
}


static void tree_build (tree_t *tree)
{
/*    tree->nodes[p].splitplane = fmat_create(1, tree->ndim);
    fts_object_refer(tree->nodes[p].splitplane);
*/
}


static fmat_t *tree_search (tree_t *tree, fmat_t *x)
{
    fts_stack_t stack;

    fts_stack_init(&stack, stack_elem_t);
    
    stack_push(&stack, 0, 0);

    /* one element access */
/*     fmat_get_element(x, 0, i); */

}   



static fts_method_status_t _tree_add (fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  tree_t *self = (tree_t *) o;
  fmat_t *x = (fmat_t *) fts_get_object(at);

  /* tree_add(self, x); */

  return fts_ok;
}



static fts_method_status_t _tree_search (fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  tree_t *self = (tree_t *) o;
  fmat_t *x = (fmat_t *) fts_get_object(at);
  fmat_t *r = tree_search(self, x);

  fts_set_object(ret, r);
  fts_object_refer(r);

  return fts_ok;
}


static fts_method_status_t _tree_get (fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  tree_t *self = (tree_t *) o;
  int i = fts_get_int(at);

  /* access tree content by index */

  return fts_ok;
}




/**********************************************************
*
*  class
*
*/

static fts_method_status_t
tree_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  tree_t *self = (tree_t *) o;
  int i;

  self->ndata  = 0;
  self->nnodes = 0;
  self->ndim   = 0;
 

  /* make space for max. ac vectors */


  while (ac)
  {
      if (fts_is_a(at, fmat_class))
      {
	  fmat_t *x = (fmat_t *) fts_get_object(at);

	  /* check dimensions */
	  if (self->ndim == 0)
	      self->ndim = fmat_get_n(x);
	  else;

	  self->data[self->ndata++] = x;
	  fts_object_refer(x);
      }

      ac--;
      at++;
  }

  /* build tree */
  tree_build(self);

  return fts_ok;
}


static fts_method_status_t
tree_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  tree_t *self = (tree_t *) o;
  
  return fts_ok;
}


static void
tree_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(tree_t), tree_init, tree_delete);
  
/*  fts_class_set_copy_function(cl, tree_copy_function);
  fts_class_set_array_function(cl, tree_array_function);
*/
  fts_class_message_varargs(cl, fts_s_name, fts_object_name);
  fts_class_message_varargs(cl, fts_s_persistence, fts_object_persistence);
/*  fts_class_message_varargs(cl, fts_s_dump_state, tree_dump_state); */
  
/*  fts_class_message_varargs(cl, fts_s_set_from_instance, _tree_set_from_tree);
  fts_class_message_varargs(cl, fts_s_print, tree_print);
  
  fts_class_message_void(cl, fts_s_clear, _tree_clear);
  
  fts_class_message_varargs(cl, fts_new_symbol("rebuild"), _tree_rebuild);
  
  fts_class_message_varargs(cl, fts_s_remove, _tree_remove);
*/


  fts_class_message(cl, fmat_class, fts_new_symbol("add"), _tree_add);

  /* $tree[x] calls these methods, depending on the type of x */
  fts_class_message(cl, fmat_class, fts_s_get_element, _tree_search);
  fts_class_message_int(cl, fts_s_get_element, _tree_get);


  fts_class_inlet_thru(cl, 0);
  fts_class_outlet_thru(cl, 0);


  /*
   * class doc
   */

  fts_class_doc(cl, tree_symbol, "[<sym|int: key> <any: value> ...]", "treeionary");
  fts_class_doc(cl, fts_s_clear, NULL, "erase all entries");
  fts_class_doc(cl, fts_s_set, "<any: key> <any: value> ...", "set list of key-value pairs");
  fts_class_doc(cl, fts_s_remove, "<any: key> ...", "remove entries");
  fts_class_doc(cl, fts_s_print, NULL, "print list of entries");
}


void
tree_config(void)
{
  tree_symbol = fts_new_symbol("tree");
  
  tree_class = fts_class_install(tree_symbol, tree_instantiate);
}
