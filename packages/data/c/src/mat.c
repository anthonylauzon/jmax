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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include <fts/fts.h>
#include "mat.h"

fts_symbol_t mat_symbol = 0;
fts_metaclass_t *mat_type = 0;
fts_class_t *mat_class = 0;

static fts_symbol_t sym_text = 0;
static fts_symbol_t sym_comma = 0;

/********************************************************
 *
 *  utility functions
 *
 */

void
mat_alloc(mat_t *mat, int m, int n)
{
  int size = m * n;

  if(size > 0)
    {
      int i;

      mat->data = fts_malloc(m * n * sizeof(fts_atom_t));
      mat->m = m;
      mat->n = n;
      mat->alloc = size;

      /* set all atoms to void */
      for(i=0; i<size; i++)
	fts_set_void(mat->data + i);
    }
  else
    {
      mat->data = 0;
      mat->m = 0;
      mat->n = 0;    
      mat->alloc = 0;
    }
}

void
mat_free(mat_t *mat)
{
  if(mat->m * mat->n)
    fts_free(mat->data);
}

void
mat_set_size(mat_t *mat, int m, int n)
{
  int size = m * n;
  
  if(size > mat->alloc)
    {
      int i;

      if(mat->alloc)
	mat->data = fts_realloc(mat->data, size * sizeof(fts_atom_t));
      else
	mat->data = fts_malloc(size * sizeof(fts_atom_t));

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

      /* void region cut off */
      for(i=size; i<old_size; i++)
	{
	  fts_atom_t *ap = mat->data + i;

	  fts_set_void(ap);
	}
      
      if(size > 0)
	{
	  mat->m = m;
	  mat->n = n;
	}
      else
	{
	  mat->m = 0;
	  mat->n = 0;
	}
    }
}

void
mat_set_element(mat_t *mat, int i, int j, fts_atom_t value)
{
  fts_atom_t *ap = mat->data + i * mat->n + j;
  
  fts_atom_assign(ap, &value);
}

void
mat_void_element(mat_t *mat, int i, int j)
{
  fts_atom_t *ap = mat->data + i * mat->n + j;
  
  fts_set_void(ap);
}

void
mat_set_const(mat_t *mat, fts_atom_t value)
{
  int size = mat->m * mat->n;

  int i;

  for(i=0; i<size; i++)
    {
      fts_atom_t *ap = mat->data + i;

      fts_atom_assign(ap, &value);
    }
}

void
mat_void(mat_t *mat)
{
  int i;
  int size = mat->m * mat->n;

  for(i=0; i<size; i++)
    {
      fts_atom_t *ap = mat->data + i;

      fts_set_void(ap);
    }
}

void
mat_set_with_onset_from_atoms(mat_t *mat, int onset, int ac, const fts_atom_t *at)
{
  fts_atom_t *ap = mat->data + onset;
  int m = mat_get_m(mat);
  int n = mat_get_n(mat);
  int size = m * n;
  int i;
  
  if(onset + ac > size)
    ac = size - onset;
  
  for(i=0; i<ac; i++)
    fts_atom_assign(ap + i, at + i);
}

void
mat_set_from_lists(mat_t *mat, int ac, const fts_atom_t *at)
{
  int m = mat_get_m(mat);
  int n = mat_get_n(mat);
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
	      fts_atom_t *matp = mat->data + i * n + j;
	      fts_atom_t *aap = fts_array_get_element(aa, j);
	    
	      fts_atom_assign(matp, aap);
	    }
	}
      else
	break;
    }
}

void
mat_copy(mat_t *org, mat_t *copy)
{
  int m = mat_get_m(org);
  int n = mat_get_n(org);
  int i;

  mat_set_size(copy, org->m, org->n);

  for(i=0; i<m*n; i++)
    copy->data[i] = org->data[i];
}

/********************************************************
 *
 *  files
 *
 */

#define MAT_BLOCK_SIZE 256

static void
mat_grow(mat_t *mat, int size)
{
  int alloc = mat->alloc;

  while(size > alloc)
    alloc += MAT_BLOCK_SIZE;

  mat_set_size(mat, alloc, 1);
}

int 
mat_read_atom_file_newline(mat_t *mat, fts_symbol_t file_name)
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

  mat_void(mat);

  while(fts_atom_file_read(file, &a, &c))
    {
      m = i + 1;

      /* first row determines # of columns */    
      if(i == 0)
	n = j + 1;
      
      if(m * n > mat->alloc)
	mat_grow(mat, m * n);

      if(j < n)
	{
	  mat->data[i * n + j] = a;
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

  mat->m = m;
  mat->n = n;
  
  fts_atom_file_close(file);

  return(m * n);
}

int
mat_write_atom_file_newline(mat_t *mat, fts_symbol_t file_name)
{
  fts_atom_file_t *file;
  int m = mat->m;
  int n = mat->n;
  int i, j;

  file = fts_atom_file_open(fts_symbol_name(file_name), "w");

  if(!file)
    return -1;

  /* write the content of the mat */
  for(i=0; i<m; i++)     
    {
      fts_atom_t *row = mat->data + i * n;

      for(j=0; j<n-1; j++)	
	fts_atom_file_write(file, row + j, ' ');

      fts_atom_file_write(file, row + n - 1, '\n');
    }

  fts_atom_file_close(file);
  return(m * n);
}

int 
mat_read_atom_file_separator(mat_t *mat, fts_symbol_t file_name, fts_symbol_t separator, int ac, const fts_atom_t *at)
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
    separator = sym_comma;

  mat_void(mat);

  while(fts_atom_file_read(file, &a, &c))
    {
      int skip = 0;
      int k;

      /* filter atoms */
      for(k=0; k<ac; k++)
	{
	  /* ooops! */
	  /*if( a.type == at[k].type && a.value.fts_int == at[k].value.fts_int)*/
	  if( fts_atom_same_type(&a, &at[k]) && a.value.fts_int == at[k].value.fts_int)
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
	      
	      if(n > mat->alloc)
		mat_grow(mat, n);
	      
	      mat->data[i * n + j] = a;
	      j++;
	    }
	  else if(j < n)
	    {
	      if(m * n > mat->alloc)
		mat_grow(mat, m * n);
	      
	      mat->data[i * n + j] = a;
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
      mat->m = m;
      mat->n = n;
    }
  else
    {
      mat->m = 0;
      mat->n = 0;      
    }
  
  fts_atom_file_close(file);

  return(m * n);
}

int
mat_write_atom_file_separator(mat_t *mat, fts_symbol_t file_name, fts_symbol_t separator)
{
  fts_atom_file_t *file;
  int m = mat->m;
  int n = mat->n;
  fts_atom_t sep;
  int i, j;

  file = fts_atom_file_open(fts_symbol_name(file_name), "w");

  if(!file)
    return -1;

  fts_set_symbol(&sep, separator);

  /* write the content of the mat */
  for(i=0; i<m; i++)     
    {
      fts_atom_t *row = mat->data + i * n;

      for(j=0; j<n; j++)	
	fts_atom_file_write(file, row + j, ' ');

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
mat_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t a[1];

  fts_set_object(a, o);
  fts_outlet_send(o, 0, mat_symbol, 1, a);
}

static void
mat_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;
  int m = mat_get_m(this);
  int n = mat_get_n(this);
  
  if(ac > 1 && fts_is_number(at) && fts_is_number(at + 1))
    {
      int i = fts_get_number_int(at);
      int j = fts_get_number_int(at + 1);

      if(i >= 0 && i < m && j >= 0 && j < n)
	mat_void_element(this, i, j);
    }
  else if(ac == 1)
    {
      int i = fts_get_number_int(at);

      if(i >= 0 && i < m)
	{
	  int j;

	  /* void row */
	  for(j=0; j<n; j++)
	    mat_void_element(this, i, j);
	}
    }
  else
    mat_void(this);
}

static void
mat_fill(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;

  if(ac > 0)
    mat_set_const(this, at[0]);
}

static void
mat_set_elements(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;

  if(ac > 2 && fts_is_number(at) && fts_is_number(at + 1))
    {
      int m = mat_get_m(this);
      int n = mat_get_n(this);
      int i = fts_get_number_int(at);
      int j = fts_get_number_int(at + 1);

      if(i >= 0 && i < m && j >= 0 && j < n)
	mat_set_with_onset_from_atoms(this, i * n + j, ac - 2, at + 2);
    }
}

static void
mat_set_row_elements(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;

  if(ac > 1 && fts_is_number(at))
    {
      int m = mat_get_m(this);
      int n = mat_get_n(this);
      int i = fts_get_number_int(at);

      ac--;
      at++;
      
      /* clip to row */
      if(ac > n)
	ac = n;

      if(i >= 0 && i < m)
	mat_set_with_onset_from_atoms(this, i * n, ac, at);
    }
}

static void
mat_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;
  int old_size = mat_get_m(this)* mat_get_n(this);
  int m = 0;
  int n = 0;
  int i;

  if(ac == 1 && fts_is_number(at))
    {
      m = fts_get_number_int(at);
      n = mat_get_n(this);
      
      if(m >= 0 && n >= 0)
	mat_set_size(this, m, n);
    }  
  else if(ac == 2 && fts_is_number(at) && fts_is_number(at + 1))
    {
      m = fts_get_number_int(at);
      n = fts_get_number_int(at + 1);
      
      if(m >= 0 && n >= 0)
	mat_set_size(this, m, n);
    }

  /* set newly allocated region to void */
  for(i=old_size; i<m*n; i++)
    fts_set_void(this->data + i);
}

static void
mat_import(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_format = fts_get_symbol_arg(ac, at, 1, sym_text);

  if(!file_name)
    return;

  if(file_format == sym_text)
    {
      fts_symbol_t separator = fts_get_symbol_arg(ac, at, 2, 0);
      int size = 0;

      if(separator)
	size = mat_read_atom_file_separator(this, file_name, separator, ac - 3, at + 3);
      else
	size = mat_read_atom_file_newline(this, file_name);

      if(size <= 0)
	post("mat: can't import from text file \"%s\"\n", fts_symbol_name(file_name));
    }
  else
    post("mat: unknown import file format \"%s\"\n", fts_symbol_name(file_format));
}

static void
mat_export(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_format = fts_get_symbol_arg(ac, at, 1, sym_text);

  if(!file_name)
    return;

  if(file_format == sym_text)
    {
      fts_symbol_t separator = fts_get_symbol_arg(ac, at, 2, 0);
      int size = 0;

      if(separator)
	size = mat_write_atom_file_separator(this, file_name, separator);
      else
	size = mat_write_atom_file_newline(this, file_name);

      if(size < 0)
	post("mat: can't export to text file \"%s\"\n", fts_symbol_name(file_name));
    }
  else
    post("mat: unknown export file format \"%s\"\n", fts_symbol_name(file_format));
}

static void
mat_set_from_instance(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;
  mat_t *set = mat_atom_get(at);

  mat_copy(set, this);
}

static void
mat_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_atom_t *data = this->data;
  int m = mat_get_m(this);
  int n = mat_get_n(this);
  int size = m * n;
  fts_message_t *mess;
  int i, j;

  /* dump size message */
  mess = fts_dumper_message_new(dumper, fts_s_size);  
  fts_message_append_int(mess, m);
  fts_message_append_int(mess, n);
  fts_dumper_message_send(dumper, mess);

  for(i=0; i<m; i++)
    {
      /* new row */
      mess = fts_dumper_message_new(dumper, fts_s_row);  
      fts_message_append_int(mess, i);
  
      for(j=0; j<n; j++)
	{
	  fts_atom_t *d = data + i * n + j;

	  /* cannot dump objects yet */
	  if(fts_is_object(d))
	    fts_message_append_int(mess, 0);
	  else
	    fts_message_append(mess, 1, d);
	}
    }
}

static void
mat_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;
  int m = mat_get_m(this);
  int n = mat_get_n(this);
  int i;
  
  post("{\n");

  for(i=0; i<m; i++)
    {
      post("  {");
      post_atoms(n, mat_get_row(this, i));
      post("}\n");
    }

  post("}\n");
}

static void
mat_set_keep(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  mat_t *this = (mat_t *)obj;

  if(this->keep != fts_s_args && fts_is_symbol(value))
    this->keep = fts_get_symbol(value);
}

static void
mat_get_keep(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  mat_t *this = (mat_t *)obj;

  fts_set_symbol(value, this->keep);
}

static void
mat_get_mat(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object(value, obj);
}

/********************************************************************
 *
 *   class
 *
 */

static void
mat_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;

  ac--;
  at++;

  this->keep = fts_s_no;

  if(ac == 0)
    mat_alloc(this, 0, 0);
  else if(ac == 1 && fts_is_int(at))
    mat_alloc(this, fts_get_int(at), 1);
  else if(ac == 2 && fts_is_int(at) && fts_is_int(at + 1))
    mat_alloc(this, fts_get_int(at), fts_get_int(at + 1));
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
      
      mat_alloc(this, m, n);
      mat_set_from_lists(this, ac, at);

      this->keep = fts_s_args;
    }
  else
    fts_object_set_error(o, "Wrong arguments");
}

static void
mat_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;
  
  mat_free(this);
}

static int
mat_check(int ac, const fts_atom_t *at)
{
  return (ac == 0 || (ac == 1 && fts_is_int(at)) || (ac == 2 && fts_is_int(at) && fts_is_int(at + 1)) || fts_is_array(at));
}

static fts_status_t
mat_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(mat_t), 1, 1, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, mat_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, mat_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_print, mat_print); 

  fts_class_add_daemon(cl, obj_property_put, fts_s_keep, mat_set_keep);
  fts_class_add_daemon(cl, obj_property_get, fts_s_keep, mat_get_keep);
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, mat_get_mat);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set_from_instance, mat_set_from_instance);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, mat_set_elements);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_row, mat_set_row_elements);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_size, mat_size);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_dump, mat_dump);
  
  /* user methods */
  fts_method_define_varargs(cl, 0, fts_s_bang, mat_output);

  fts_method_define_varargs(cl, 0, fts_s_clear, mat_clear);
  fts_method_define_varargs(cl, 0, fts_s_fill, mat_fill);      
  fts_method_define_varargs(cl, 0, fts_s_set, mat_set_elements);
  fts_method_define_varargs(cl, 0, fts_s_row, mat_set_row_elements);

  fts_method_define_varargs(cl, 0, fts_s_size, mat_size);
            
  fts_method_define_varargs(cl, 0, fts_s_import, mat_import); 
  fts_method_define_varargs(cl, 0, fts_s_export, mat_export); 

  /* type outlet */
  fts_outlet_type_define(cl, 0, mat_symbol, 1, &mat_symbol);

  return fts_Success;
}

void
mat_config(void)
{
  sym_text = fts_new_symbol("text");
  sym_comma = fts_new_symbol(",");
  mat_symbol = fts_new_symbol("mat");

  mat_type = fts_class_install(mat_symbol, mat_instantiate);
  mat_class = fts_class_get_by_name(mat_symbol);
}
