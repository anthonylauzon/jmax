/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Francois Dechelle, Norbert Schnell.
 *
 */

#include <fts/fts.h>
#include "fmat.h"

fts_symbol_t fmat_symbol = 0;
fts_type_t fmat_type = 0;
fts_class_t *fmat_class = 0;

static fts_symbol_t sym_text = 0;

/********************************************************
 *
 *  utility functions
 *
 */

/* local */
static void
set_size(fmat_t *mat, int m, int n)
{
  int size = m * n;
  
  if(size > mat->alloc)
    {
      int i;

      if(mat->alloc)
	mat->values = fts_realloc(mat->values, size * sizeof(float));
      else
	mat->values = fts_malloc(size * sizeof(float));

      mat->m = m;
      mat->n = n;
      mat->alloc = size;
    }
  else
    {
      int old_size = mat->m * mat->n;
      int i;

      if(size <= 0)
	m = n = size = 0;

      /* zero region cut off */
      for(i=size; i<old_size; i++)
	mat->values[i] = 0.0;
      
      mat->m = m;
      mat->n = n;
    }
}

void
fmat_set_const(fmat_t *mat, float c)
{
  float *values = mat->values;
  int size = mat->m * mat->n;
  int i;
  
  for(i=0; i<size; i++)
    values[i] = c;
}

void
fmat_set_with_onset_from_atoms(fmat_t *mat, int offset, int ac, const fts_atom_t *at)
{
  int size = mat->m * mat->n;
  int i;
  
  if(offset + ac > size)
    ac = size - offset;
  
  for(i=0; i<ac; i++)
    {
      if(fts_is_number(at + i))
	mat->values[i + offset] = fts_get_number_float(at + i);
      else
	mat->values[i + offset] = 0.0f;
    }
}

void
fmat_set_from_lists(fmat_t *mat, int ac, const fts_atom_t *at)
{
  int m = fmat_get_m(mat);
  int n = fmat_get_n(mat);
  int i, j;
  
  for(i=0; i<ac; i++)
    {
      if(fts_is_array(at + i))
	{
	  fts_array_t *aa = fts_get_array(at + i);
	  int size = fts_array_get_size(aa);

	  if(size > n)
	    size = n;
	  
	  for(j=0; j<size; j++)
	    {
	      fts_atom_t *a = fts_array_get_element(aa, j);
	    
	      if(fts_is_number(a))
		mat->values[i * n + j] = fts_get_number_float(a);
	      else
		mat->values[i * n + j] = 0.0;
	    }
	}
      else
	break;
    }
}

/********************************************************
 *
 *  files
 *
 */

#define FMAT_BLOCK_SIZE 256

static void
fmat_grow(fmat_t *fmat, int size)
{
  int alloc = fmat->alloc;

  while(size > alloc)
    alloc += FMAT_BLOCK_SIZE;

  set_size(fmat, alloc, 1);
}

int 
fmat_read_atom_file_newline(fmat_t *fmat, fts_symbol_t file_name)
{
  fts_atom_file_t *file = fts_atom_file_open(fts_symbol_name(file_name), "r");
  int m = 0;
  int n = 0;
  int i = 0;
  int j = 0;
  fts_atom_t a;
  char c;

  if(!file)
    return -1;

  fmat_zero(fmat);

  while(fts_atom_file_read(file, &a, &c))
    {
      m = i + 1;

      /* first row determines # of columns */    
      if(i == 0)
	n = j + 1;
      
      if(m * n > fmat->alloc)
	fmat_grow(fmat, m * n);

      if(j < n)
	{
	  if(fts_is_number(&a))
	    fmat->values[i * n + j] = fts_get_number_float(&a);
	  else
	    fmat->values[i * n + j] = 0.0;

	  j++;
	  
	  if(c == '\n')
	    {
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

  /* maybe empty rest of last line */
  if(j > 0)
    {
      i++;
      j = 0;
    }

  fmat->m = m;
  fmat->n = n;
  
  fts_atom_file_close(file);

  return(m * n);
}

int
fmat_write_atom_file_newline(fmat_t *fmat, fts_symbol_t file_name)
{
  fts_atom_file_t *file;
  int m = fmat->m;
  int n = fmat->n;
  int i, j;

  file = fts_atom_file_open(fts_symbol_name(file_name), "w");

  if(!file)
    return -1;

  /* write the content of the fmat */
  for(i=0; i<m; i++)     
    {
      float *row = fmat->values + i * n;
      fts_atom_t a;

      for(j=0; j<n-1; j++)	
	{
	  fts_set_float(&a, row[j]);
	  fts_atom_file_write(file, &a, ' ');
	}

      fts_set_float(&a, row[n - 1]);
      fts_atom_file_write(file, &a, '\n');
    }

  fts_atom_file_close(file);

  return(m * n);
}

int 
fmat_read_atom_file_separator(fmat_t *fmat, fts_symbol_t file_name, fts_symbol_t separator, int ac, const fts_atom_t *at)
{
  fts_atom_file_t *file = fts_atom_file_open(fts_symbol_name(file_name), "r");
  int m = 0;
  int n = 0;
  int i = 0;
  int j = 0;
  fts_atom_t a;
  char c;

  if(!file)
    return -1;

  if(!separator)
    separator = fts_s_comma;

  fmat_zero(fmat);

  while(fts_atom_file_read(file, &a, &c))
    {
      int skip = 0;
      int k;

      /* filter atoms */
      for(k=0; k<ac; k++)
	{
	  /* ooops! */
	  if(a.type == at[k].type && a.value.fts_int == at[k].value.fts_int)
	    {
	      skip = 1;
	      break;
	    }
	}

      if(!skip)
	{
	  m = i + 1;
	  
	  if(fts_get_symbol(&a) == separator)
	    {
	      /* reset to beginning of next row */
	      i++;
	      j = 0;
	    }
	  else if(i == 0)
	    {
	      /* first row determines # of columns */    
	      n = j + 1;
	      
	      if(n > fmat->alloc)
		fmat_grow(fmat, n);

	      if(fts_is_number(&a))
		fmat->values[i * n + j] = fts_get_number_float(&a);
	      else
		fmat->values[i * n + j] = 0.0;

	      j++;
	    }
	  else if(j < n)
	    {
	      if(m * n > fmat->alloc)
		fmat_grow(fmat, m * n);
	      
	      if(fts_is_number(&a))
		fmat->values[i * n + j] = fts_get_number_float(&a);
	      else
		fmat->values[i * n + j] = 0.0;

	      j++;
	    }
	}
    }

  /* maybe empty rest of last line */
  if(j > 0)
    {
      i++;
      j = 0;
    }

  if(n > 0)
    {
      fmat->m = m;
      fmat->n = n;
    }
  else
    {
      fmat->m = 0;
      fmat->n = 0;      
    }
  
  fts_atom_file_close(file);

  return(m * n);
}

int
fmat_write_atom_file_separator(fmat_t *fmat, fts_symbol_t file_name, fts_symbol_t separator)
{
  fts_atom_file_t *file;
  int m = fmat->m;
  int n = fmat->n;
  fts_atom_t sep;
  int i, j;

  file = fts_atom_file_open(fts_symbol_name(file_name), "w");

  if(!file)
    return -1;

  fts_set_symbol(&sep, separator);

  /* write the content of the fmat */
  for(i=0; i<m; i++)     
    {
      float *row = fmat->values + i * n;
      fts_atom_t a;

      for(j=0; j<n; j++)	
	{
	  fts_set_float(&a, row[j]);
	  fts_atom_file_write(file, &a, ' ');
	}

      fts_atom_file_write(file, &sep, '\n');
    }

  fts_atom_file_close(file);

  return(m * n);
}

/********************************************************************
 *
 *   user methods
 *
 */

static void
fmat_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;
  fts_atom_t a[1];

  fmat_atom_set(a, this);
  fts_outlet_send(o, 0, fmat_symbol, 1, a);
}

static void
fmat_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;

  fmat_set_const(this, 0.0f);
}

static void
fmat_fill(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;

  if(ac > 0 && fts_is_number(at))
    {
      float constant = fts_get_float(at);
      
      fmat_set_const(this, constant);
    }
}

static void
fmat_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;

  if(ac > 1 && fts_is_number(at))
    {
      int size = fmat_get_m(this) * fmat_get_n(this);
      int offset = fts_get_number_int(at);

      if(offset >= 0 && offset < size)
	fmat_set_with_onset_from_atoms(this, offset, ac - 1, at + 1);
    }
}

static void
fmat_import(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_format = fts_get_symbol_arg(ac, at, 1, sym_text);

  if(!file_name)
    return;

  if(file_format == sym_text)
    {
      fts_symbol_t separator = fts_get_symbol_arg(ac, at, 2, 0);
      int size = 0;

      if(separator)
	size = fmat_read_atom_file_separator(this, file_name, separator, ac - 3, at + 3);
      else
	size = fmat_read_atom_file_newline(this, file_name);

      if(size <= 0)
	post("fmat: can't import from text file \"%s\"\n", fts_symbol_name(file_name));
    }
  else
    post("fmat: unknown import file format \"%s\"\n", fts_symbol_name(file_format));
}

static void
fmat_export(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_format = fts_get_symbol_arg(ac, at, 1, sym_text);

  if(!file_name)
    return;

  if(file_format == sym_text)
    {
      fts_symbol_t separator = fts_get_symbol_arg(ac, at, 2, 0);
      int size = 0;

      if(separator)
	size = fmat_write_atom_file_separator(this, file_name, separator);
      else
	size = fmat_write_atom_file_newline(this, file_name);

      if(size < 0)
	post("fmat: can't export to text file \"%s\"\n", fts_symbol_name(file_name));
    }
  else
    post("fmat: unknown export file format \"%s\"\n", fts_symbol_name(file_format));
}

/********************************************************************
 *
 *  system functions
 *
 */

static void
fmat_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;
  int m = fmat_get_m(this);
  int n = fmat_get_n(this);
  int i, j;
  
  post("{\n");

  for(i=0; i<m; i++)
    {
      post("  {");

      for(j=0; j<n-1; j++)
	post("%f ", fmat_get_element(this, i, j));

      post("%f", fmat_get_element(this, i, j));

      post("}\n");
    }

  post("}");
}

void 
fmat_dump(fmat_t *mat, fts_dumper_t *dumper)
{
  fts_atom_t av[256];
  int size = fmat_get_m(mat) * fmat_get_n(mat);
  int ac = 0;
  int offset = 0;
  int i;

  for(i=0; i<size; i++)
    {
      fts_set_float(&av[ac], mat->values[i]);

      ac++;

      if (ac == 256)
	{
	}
    }

  if (ac != 0)
    {
    }
}

static void
fmat_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fmat_t *this = (fmat_t *)obj;

  fmat_atom_set(value, this);
}

/*********************************************************
 *
 *  class
 *
 */
static void
fmat_alloc(fmat_t *mat, int m, int n)
{
  int size = m * n;
  int i;

  if(size > 0)
    {
      mat->values = (float *) fts_malloc(size * sizeof(float));
      mat->m = m;
      mat->n = n;
      fmat_zero(mat);

      /* init to zero */
      for(i=0; i<size; i++)
	mat->values[i] = 0.0;
    }
  else
    {
      mat->values = 0;
      mat->m = 0;
      mat->n = 0;
    }

  mat->alloc = size;
}

static void
fmat_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;
  
  /* skip class name */
  ac--;
  at++;

  if(ac == 0)
    fmat_alloc(this, 0, 0);
  else if(ac == 1 && fts_is_int(at))
    fmat_alloc(this, fts_get_int(at), 1);
  else if(ac == 2 && fts_is_int(at) && fts_is_int(at + 1))
    fmat_alloc(this, fts_get_int(at), fts_get_int(at + 1));
  else if(fts_is_array(at))
    {
      int m = 0;
      int n = 0;
      int i;
      
      /* check n (longest row) and m */
      for(i=0; i<ac; i++)
	{
	  if(fts_is_array(at + i))
	    {
	      fts_array_t *aa = fts_get_array(at + i);
	      int size = fts_array_get_size(aa);
	      
	      if(size > n)
		n = size;
	      
	      m++;
	    }
	  else
	    break;
	}
      
      fmat_alloc(this, m, n);
      fmat_set_from_lists(this, ac, at);
    }
}

static void
fmat_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;
  
  fts_free(this->values);
}

static int
fmat_check(int ac, const fts_atom_t *at)
{
  return (ac == 0 || (ac == 1 && (fts_is_int(at) || fts_is_array(at))) || (ac > 1));
}

static fts_status_t
fmat_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(fmat_check(ac - 1, at + 1))
    {
      fts_class_init(cl, sizeof(fmat_t), 1, 1, 0);
  
      /* init / delete */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fmat_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, fmat_delete);
  
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_print, fmat_print); 

      /* define variable */
      fts_class_add_daemon(cl, obj_property_get, fts_s_state, fmat_get_state);

      fts_method_define_varargs(cl, 0, fts_s_bang, fmat_output);

      fts_method_define_varargs(cl, 0, fts_s_clear, fmat_clear);
      fts_method_define_varargs(cl, 0, fts_s_fill, fmat_fill);
      fts_method_define_varargs(cl, 0, fts_s_set, fmat_set);
      
      fts_method_define_varargs(cl, 0, fts_s_import, fmat_import);
      fts_method_define_varargs(cl, 0, fts_s_export, fmat_export);

      /* type outlet */
      fts_outlet_type_define(cl, 0, fmat_symbol, 1, &fmat_type);      

      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}

/********************************************************************
 *
 *  config
 *
 */

static int
fmat_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  return fmat_check(ac1 - 1, at1 + 1);
}

void 
fmat_config(void)
{
  sym_text = fts_new_symbol("text");
  fmat_symbol = fts_new_symbol("fmat");
  fmat_type = fmat_symbol;

  fts_metaclass_install(fmat_symbol, fmat_instantiate, fmat_equiv);
  fmat_class = fts_class_get_by_name(fmat_symbol);

  fts_atom_type_register(fmat_symbol, fmat_class);
}
