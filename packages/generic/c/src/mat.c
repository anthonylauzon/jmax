/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "fts.h"

static fts_symbol_t sym_atom_matrix = 0;

/******************************************************************
 *
 *  atom matrix
 *
 */

typedef struct
{
  fts_atom_t *data; /* matrix data */
  int m; /* # of rows */
  int n; /* # of columns */
  int alloc; /* current alloc size for lazy allocation */
} atom_matrix_t;

#define atom_matrix_set_element(mx, i, j, v) ((mx)->data[(i) * (mx)->n + (j)] = (v))
#define atom_matrix_get_element(mx, i, j) ((mx)->data[(i) * (mx)->n + (j)])

void
atom_matrix_void(atom_matrix_t *mx)
{
  int i;
  int size = mx->m * mx->n;

  for(i=0; i<size; i++)
    fts_set_void(mx->data + i);
}

atom_matrix_t *
atom_matrix_new(int m, int n)
{
  atom_matrix_t *mx = fts_malloc(sizeof(atom_matrix_t));
  int size = m * n;

  if(size > 0)
    {
      mx->data = fts_malloc(m * n * sizeof(fts_atom_t));
      mx->m = m;
      mx->n = n;
      mx->alloc = size;

      atom_matrix_void(mx);
    }
  else
    {
      mx->data = 0;
      mx->m = 0;
      mx->n = 0;    
      mx->alloc = 0;
    }

  return mx;
}

void
atom_matrix_delete(atom_matrix_t *mx)
{
  if(mx->m * mx->n)
    fts_free(mx->data);

  fts_free(mx);
}

void
atom_matrix_set_size(atom_matrix_t *mx, int m, int n)
{
  int size = m * n;
  
  if(size > mx->alloc)
    {
      if(mx->alloc)
	mx->data = fts_realloc(mx->data, m * n * sizeof(fts_atom_t));
      else
	mx->data = fts_malloc(m * n * sizeof(fts_atom_t));
    }
  else if(size > 0)
    {
      mx->m = m;
      mx->n = n;
    }
  else
    {
      mx->m = 0;
      mx->n = 0;
    }
}

void
atom_matrix_fill(atom_matrix_t *mx, fts_atom_t atom)
{
  int i;
  int size = mx->m * mx->n;

  for(i=0; i<size; i++)
    mx->data[i] = atom;
}

static int 
atom_matrix_import_ascii(atom_matrix_t *mx, fts_symbol_t file_name, int *m_read, int *n_read)
{
  fts_atom_file_t *file = fts_atom_file_open(fts_symbol_name(file_name), "r");
  fts_atom_t *data = mx->data;
  int m = mx->m;
  int n = mx->n;
  fts_atom_t a;
  char c;
  int i, j, k;

  if(!file)
    {
      post("mat: can't open file to read: %s\n", fts_symbol_name(file_name));
      return(0);
    }

  *m_read = 0;
  *n_read = 0;

  i = 0;
  j = 0;
  while(i < m && fts_atom_file_read(file, &a, &c))
    {
      if(j < n)
	{
	  data[i * n + j] = a;
	  j++;
	  
	  if(c == '\n')
	    {
	      /* find greatest number of columns in file */
	      if(j > *n_read)
		*n_read = j;

	      /* void end of row */
	      for(; j<n; j++)
		fts_set_void(data + i * n + j);

	      /* reset to beginning of next row */
	      i++;
	      j = 0;
	    }
	}
      else if(c == '\n')
	{
	  /* reset to beginning of next row */
	  i++;
	  j = 0;
	}
    }

  *m_read = i;

  for(k=i*n+j; k<m*n; k++)
    fts_set_void(data + k);

  fts_atom_file_close(file);
  return(i);
}

static int
atom_matrix_export_ascii(atom_matrix_t *mx, fts_symbol_t file_name)
{
  fts_atom_file_t *file;
  int m = mx->m;
  int n = mx->n;
  int i, j;

  file = fts_atom_file_open(fts_symbol_name(file_name), "w");

  if(!file)
    {
      post("mat: can't open file to write: %s\n", fts_symbol_name(file_name));
      return(0);
    }

  /* write the content of the matrix */
  for(i=0; i<m; i++)     
    {
      fts_atom_t *row = mx->data + i * n;

      for(j=0; j<n-1; j++)	
	fts_atom_file_write(file, row + j, ' ');

      fts_atom_file_write(file, row + n - 1, '\n');
    }

  fts_atom_file_close(file);
  return(m * n);
}

/******************************************************************
 *
 *  object
 *
 */

typedef struct 
{
  fts_object_t ob;
  atom_matrix_t *mx; /* atom matrix */
  fts_atom_t buf;
} mat_t;

static void
mat_init_refer(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;

  this->mx = (atom_matrix_t *)fts_get_ptr(at + 1);
  fts_set_void(&this->buf);
}

static void
mat_init_define(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;
  int m = fts_get_int_arg(ac, at, 1, 0);
  int n = fts_get_int_arg(ac, at, 2, 0);

  this->mx = atom_matrix_new(m, n);
  fts_set_void(&this->buf);
}

static void
mat_delete_define(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;

  atom_matrix_delete(this->mx);
}

/********************************************************************
 *
 *   user methods
 *
 */

static void
mat_set_buffer(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;

  this->buf = at[0];
}

static void
mat_element(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;
  atom_matrix_t *mx = (atom_matrix_t *)this->mx;
  int i = fts_get_int_arg(ac, at, 0, 0);
  int j = fts_get_int_arg(ac, at, 1, 0);

  if(i >= 0 && i < this->mx->m && j >= 0 && j < this->mx->m)
    {      
      if(!fts_is_void(&this->buf))
	{
	  atom_matrix_set_element(this->mx, i, j, this->buf);
	  fts_set_void(&this->buf);
	}
      else
	{
	  fts_atom_t *atom = &(atom_matrix_get_element(mx, i, j));
	  if(!fts_is_void(atom))
	    fts_outlet_send(o, 0, fts_type_get_selector(fts_get_type(atom)), 1, atom);
	}
    }
  else
    fts_object_set_runtime_error(o, "element indices out of bounds: %d %d", i, j);
}

/* not yet implemented */
static void
mat_set_from_atom_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;
}

static void
mat_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;
  atom_matrix_t *mx = (atom_matrix_t *)this->mx;
  
  if(ac >= 2)
    {
      int i = fts_get_int_arg(ac, at, 0, 0);
      int j = fts_get_int_arg(ac, at, 1, 0);

      if(i >= 0 && i < mx->m && j >= 0 && j < mx->m)
	fts_set_void(&atom_matrix_get_element(mx, i, j));
    }
  else
    atom_matrix_void(mx);
}

static void
mat_fill(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;

  if(ac)
    atom_matrix_fill(this->mx, at[0]);
}

static void
mat_resize(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;
  int m = fts_get_int_arg(ac, at, 0, -1);
  int n = fts_get_int_arg(ac, at, 0, -1);

  if(m * n >= 0)
    atom_matrix_set_size(this->mx, m, n);
}

static void
mat_import(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);

  if(file_name)
    {
      int m_read = 0;
      int n_read = 0; 
      int size = atom_matrix_import_ascii(this->mx, file_name, &m_read, &n_read);

      if(size)
	{
	  fts_atom_t a[2];
	  
	  fts_set_int(a + 0, m_read);
	  fts_set_int(a + 1, n_read);
	  fts_outlet_send(o, 1, fts_s_list, 2, a);
	}
    }
}

static void
mat_export(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);

  if(file_name)
    atom_matrix_export_ascii(this->mx, file_name);
}

static void
mat_assist(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_symbol_t cmd = fts_get_symbol_arg(ac, at, 0, 0);

  if (cmd == fts_s_object)
    fts_object_blip(o, "matrix of values");
  else if (cmd == fts_s_inlet)
    {
      int n = fts_get_int_arg(ac, at, 1, 0);

      switch(n)
	{
	case 0:
	  fts_object_blip(o, "list <int> <int>: row and col index to recall/store value from/to matrix");
	  break;
	case 1:
	  fts_object_blip(o, "<anything>: set value to be stored");
	}
    }
  else if (cmd == fts_s_outlet)
    {
      int n = fts_get_int_arg(ac, at, 1, 0);

      switch(n)
	{
	case 0:
	  fts_object_blip(o, "<anything>: recalled value", n);
	  break;
	case 1:
	  fts_object_blip(o, "list <int> <int>: # of rows and columns read from file");
	}
    }
}

/********************************************************************
 *
 *   class
 *
 */

static void
mat_get_atom_matrix(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  mat_t *this = (mat_t *)obj;

  fts_set_ptr(value, this->mx);
  fts_set_type(value, sym_atom_matrix);
}

static fts_status_t
mat_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  fts_class_init(cl, sizeof(mat_t), 2, 2, 0);
  
  if(ac >= 2 && fts_is_a(at + 1, sym_atom_matrix))
    fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, mat_init_refer);
  else if(ac >= 3 && fts_is_int(at + 1) && fts_is_int(at + 1))
    {
      /* define variable */
      fts_class_add_daemon(cl, obj_property_get, fts_s_state, mat_get_atom_matrix);

      /* .bmax load and save */
      /*fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, mat_set_from_atom_list);*/
      /*fts_method_define_varargs(cl, fts_SystemInlet, fts_s_save_bmax, mat_save_bmax);*/

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, mat_init_define);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, mat_delete_define);
    }
  else
    return &fts_CannotInstantiate;

  /* help */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("assist"), mat_assist); 

  /* user methods */
  fts_method_define_varargs(cl, 0, fts_s_list, mat_element);
  fts_method_define_varargs(cl, 1, fts_s_anything, mat_set_buffer);

  /*fts_method_define_varargs(cl, 0, fts_s_set, mat_set_from_atom_list);*/

  fts_method_define_varargs(cl, 0, fts_new_symbol("fill"), mat_fill);
  fts_method_define_varargs(cl, 0, fts_s_clear, mat_clear);
  
  /*fts_method_define_varargs(cl, 0, fts_new_symbol("size"), mat_resize);*/

  fts_method_define_varargs(cl, 0, fts_new_symbol("import"), mat_import); 
  fts_method_define_varargs(cl, 0, fts_new_symbol("export"), mat_export); 

  return fts_Success;
}

void
mat_config(void)
{
  sym_atom_matrix = fts_new_symbol("atom_matrix");
  fts_metaclass_install(fts_new_symbol("mat"), mat_instantiate, fts_narg_equiv);
}
