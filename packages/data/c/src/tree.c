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
#include <math.h>
#define HEIGHT 3
#define MAX_FLOAT 0x7FFFFFFF

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


static double 
vec_dist (fmat_t *a, fmat_t *b)
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

static void 
compute_mean(tree_t *t, int node) 
{
	int i, j;
	float tmp;
	float *mean_ptr;
	float *data_ptr; 
	int    m;		
    int    n;

	t->nodes[node].mean = fmat_create(1, t->ndim);
	fts_object_refer(t->nodes[node].mean);
	
	mean_ptr = fmat_get_ptr(t->nodes[node].mean);
	
	// number of vectors from the processed node
	int n_nvector = t->nodes[node].endind - t->nodes[node].startind + 1; 
	for(j = 0; j < t->ndim; j++) 
	{
		tmp = 0;
		
		for(i = t->nodes[node].startind; i <= t->nodes[node].endind; i++) 
		{
			data_ptr = fmat_get_ptr(t->data[i]);
			tmp += data_ptr[j];
		}
		mean_ptr[j] = tmp / n_nvector;
	}
	//fts_post("mean vect computed \n");
}

static void 
compute_splitplane(tree_t *t, int node, int level) 
{
	int b;
	int i;
	float *split_ptr;
	float *mean_ptr;
	
	//Level parameter gives the base vector to use
	b = level % t->ndim;
	
	t->nodes[node].splitplane = fmat_create(1, t->ndim + 1);
	fts_object_refer(t->nodes[node].splitplane);
	split_ptr = fmat_get_ptr(t->nodes[node].splitplane);
	mean_ptr = fmat_get_ptr(t->nodes[node].mean);
	
	//We compute an hyperplane which is orthogonal to the base vector number b
	for(i = 0; i < t->ndim; i++) 
		split_ptr[i] = 0;
	
	split_ptr[b] = 1;
	split_ptr[t->ndim] = -(mean_ptr[b]); 
	//fts_post("splitplane computed\n");
}

static float 
distV2H(fmat_t* vect, fmat_t* hplane) 
{
	//standard algebra computing
	int	  i;
	float tmp1 = 0;
	float tmp2 = 0;
	int   m  = fmat_get_m(vect);
	int   n  = fmat_get_n(vect);
	float *v = fmat_get_ptr(vect);
    float *h = fmat_get_ptr(hplane);
    int   size = m * n;
	
	for(i = 0; i < size; i++) 
	{
		tmp1 += v[i]*h[i];
		tmp2 += h[i]*h[i];
	}
	
	return (tmp1 + h[size])/sqrt(tmp2);
}

static void 
swap(tree_t* t, int i, int j) 
{
	int tmp;
	tmp = t->listind[i];
	t->listind[i] = t->listind[j];
	t->listind[j] = tmp;
}

static void 
tree_build (tree_t *t)
{
	int l;						//current level number
	int n;						//current node number
	int	i, j;					//loop counters
	
	/* Maximum length is equal to pow(2, height-1) */ 
	if(pow(2, t->height-1) > t->ndata)
	{
		if(t->ndata == 0) fts_post("tree is empty! \n");
		else fts_post("can't buid this tree, try with a smaller tree height \n");
	}
	
	for(l = 0; l < (t->height); l++) 
	{		
			for(n = pow(2, l) - 1; n < pow(2, l+1) - 1; n++)
			{							
				compute_mean(t, n);
				compute_splitplane(t, n, l);
				i = t->nodes[n].startind; 
				j =	t->nodes[n].endind;
		
				while(j > i) 
				{
					while(distV2H(t->data[i], t->nodes[n].splitplane) <= 0)
					{
						i++;
					}
					while(distV2H(t->data[j], t->nodes[n].splitplane) > 0) 
					{
						j--;
					}
					if(j > i)
					{ 
						swap(t, i, j);
					}
				}
				t->nodes[2*n+1].startind = t->nodes[n].startind; //begining index of left child of node n
				t->nodes[2*n+1].endind = j;						 //ending index of left child of node n
				t->nodes[2*n+2].startind = i;					 //begining index of left child of node n
				t->nodes[2*n+2].endind = t->nodes[n].endind;	 //ending index of left child of node n
			}
	}

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

/* For debuging */
static void 
tree_info_display(tree_t* t) 
{
	fts_post("\nTree Info:\n");
	fts_post("**********\n\n");
	fts_post("ndim = %i\n", t->ndim);
	fts_post("ndata = %i\n", t->ndata);
	fts_post("height = %i\n", t->height);
	fts_post("nnodes = %i\n", t->nnodes);
}

static void 
tree_display(tree_t* t) 
{
	int l, n, i, j;
	float* data_ptr;
	
	fts_post("\nTree Display:\n");
	fts_post("*************\n\n");
	
	if(t->height == 0 || t->ndata == 0) printf("Empty Tree\n");
	for(l = 0; l < t->height; l++) {
		fts_post("Level #%i ", l);
		for(n = (int)pow(2, l) - 1; n < (int)pow(2, l+1) - 1; n++) {
			fts_post("(");
			for(i = t->nodes[n].startind; i <= t->nodes[n].endind; i++) {
				fts_post("[");
				data_ptr = fmat_get_ptr(t->data[i]);
				if(data_ptr[0] == MAX_FLOAT) fts_post("zombi]"); 
				else {
					for(j = 0; j < (t->ndim - 1); j++) { 	
						fts_post("%.1f, ", data_ptr[j]);
					}
					fts_post("%.1f]", data_ptr[t->ndim - 1]);
				}
			}
			fts_post(")");
		}
		fts_post("\n");
	}
}

static void 
data_display(tree_t *t, int v) 
{
	int j;
	float* data_ptr;
	
	fts_post("vector #%i[", v);
	data_ptr = fmat_get_ptr(t->data[v]);			
	if(data_ptr[0] == MAX_FLOAT) fts_post("zombi]"); 
	else 
	{
		for(j = 0; j < (t->ndim - 1); j++) 
		{ 	
			fts_post("%.1f, ", data_ptr[j]);
		}
		fts_post("%.1f]\n", data_ptr[t->ndim - 1]);
	}
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
    
  /* Init */
  self->ndata  = 0;
  self->height = HEIGHT;      /*Function that returns an optimal tree height for a given number of data*/
  self->nnodes = (int)pow(2, self->height) - 1;	
  self->ndim   = 0;
  
  /* Init index list */
  for(i = 0; i < self->ndata; i++)
	self->listind[i] = i;
 
  fts_post("tree init is ok \n");

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

  /* Init root node */
  self->nodes[0].startind = 0;
  self->nodes[0].endind = self->ndata - 1;

  /* build tree */
  tree_build(self);
  tree_info_display(self);
  tree_display(self);

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

  /*fts_class_doc(cl, tree_symbol, "[<sym|int: key> <any: value> ...]", "treeionary");
  fts_class_doc(cl, fts_s_clear, NULL, "erase all entries");
  fts_class_doc(cl, fts_s_set, "<any: key> <any: value> ...", "set list of key-value pairs");
  fts_class_doc(cl, fts_s_remove, "<any: key> ...", "remove entries");
  fts_class_doc(cl, fts_s_print, NULL, "print list of entries");*/
}


void
tree_config(void)
{
  tree_symbol = fts_new_symbol("tree");
  
  tree_class = fts_class_install(tree_symbol, tree_instantiate);
}
