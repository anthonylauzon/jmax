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
 */

#include <fts/fts.h>
#include "mat.h"

fts_symbol_t mat_symbol = 0;
fts_class_t *mat_type = 0;

static fts_symbol_t sym_text = 0;
static fts_symbol_t sym_comma = 0;

#define MAT_BLOCK_SIZE 64

/********************************************************
 *
 *  utility functions
 *
 */

void
mat_set_size(mat_t *mat, int m, int n)
{
  int old_size = mat->m * mat->n;
  int alloc = mat->alloc;
  int size = m * n;
  int i;
  
  if(size > alloc)
    {
      mat->data = fts_realloc(mat->data, size * sizeof(fts_atom_t));
      mat->alloc = size;

      for(i=0; i<size; i++)
	fts_set_int(mat->data + i, 0);
    }
  else
    {
      if(size <= 0)
	m = n = size = 0;

      /* void region cut off */
      for(i=size; i<old_size; i++)
	{
	  fts_atom_release(mat->data + i);
	  fts_set_int(mat->data + i, 0);
	}
    }

  mat->m = m;
  mat->n = n;
}

void
mat_set_element(mat_t *mat, int i, int j, const fts_atom_t *value)
{
  fts_atom_t *ap = mat->data + i * mat->n + j;
  
  fts_atom_assign(ap, value);
}

void
mat_set_const(mat_t *mat, const fts_atom_t *value)
{
  int size = mat->m * mat->n;

  int i;

  for(i=0; i<size; i++)
    {
      fts_atom_t *ap = mat->data + i;

      fts_atom_assign(ap, value);
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
mat_set_from_tuples(mat_t *mat, int ac, const fts_atom_t *at)
{
  int m = mat_get_m(mat);
  int n = mat_get_n(mat);
  int i, j;
  
  for(i=0; i<ac; i++)
    {
      if(fts_is_tuple(at + i))
	{
	  fts_tuple_t *tup = (fts_tuple_t *)fts_get_object(at + i);
	  int size = fts_tuple_get_size(tup);

	  if(size > n)
	    size = n;
	  
	  for(j=0; j<size; j++)
	    {
	      fts_atom_t *matp = mat->data + i * n + j;
	      fts_atom_t *ap = fts_tuple_get_element(tup, j);
	    
	      fts_atom_assign(matp, ap);
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
  fts_atom_t *p = mat->data;
  int i;

  while(size > alloc)
    alloc += MAT_BLOCK_SIZE;

  mat_set_size(mat, alloc, 1);

  for(i=alloc; i<mat->alloc; i++)
    fts_set_int(p + i, 0);
}

int 
mat_read_atom_file_newline(mat_t *mat, fts_symbol_t file_name)
{
  fts_atom_file_t *file = fts_atom_file_open(file_name, "r");
  int m = 0;
  int n = 0;
  int i = 0;
  int j = 0;
  fts_atom_t a;
  char c;

  if(!file)
    return -1;

  mat_set_size(mat, 0, 0);

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

  file = fts_atom_file_open(file_name, "w");

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
  fts_atom_file_t *file = fts_atom_file_open(file_name, "r");
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

  mat_set_size(mat, 0, 0);

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

  file = fts_atom_file_open(file_name, "w");

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
mat_fill(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;

  if(ac > 0)
    mat_set_const(this, at);
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
    fts_set_int(this->data + i, 0);
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
	post("mat: can't import from text file \"%s\"\n", file_name);
    }
  else
    post("mat: unknown import file format \"%s\"\n", file_format);
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
	post("mat: can't export to text file \"%s\"\n", file_name);
    }
  else
    post("mat: unknown export file format \"%s\"\n", file_format);
}

static void
mat_set_from_instance(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;
  mat_t *set = (mat_t *)fts_get_object(at);

  mat_copy(set, this);
}

static void
mat_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;

  if(data_object_is_persistent(o))
    {
      fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
      fts_atom_t *data = this->data;
      int m = mat_get_m(this);
      int n = mat_get_n(this);
      int size = m * n;
      fts_message_t *mess;
      int i, j;
      
      /* save persistence flag */
      mess = fts_dumper_message_new(dumper, fts_s_persistence);
      fts_message_append_int(mess, 1);
      fts_dumper_message_send(dumper, mess);

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
	  
	  if(n > 0)
	    fts_dumper_message_send(dumper, mess);
	}
    }

  fts_name_dump_method(o, 0, 0, ac, at);
}

static void
mat_post(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
  int m = mat_get_m(this);
  int n = mat_get_n(this);
  int size = m * n;
  
  if(size == 0)
    fts_spost(stream, "(:mat)");
  else
    fts_spost(stream, "(:mat %d %d)", m, n);
}

static void
mat_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
  int m = mat_get_m(this);
  int n = mat_get_n(this);
  int size = m * n;
  
  if(size == 0)
    fts_spost(stream, "<empty mat>\n");
  else
    {
      int i;

      fts_spost(stream, "<mat %dx%d>\n", m, n);
      fts_spost(stream, "{\n");

      for(i=0; i<m; i++)
	{
	  fts_spost(stream, "  ");
	  fts_spost_atoms(stream, n, mat_get_row(this, i));
	  fts_spost(stream, ",\n");
	}

      fts_spost(stream, "}\n");
    }
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

  data_object_init(o);

  this->data = NULL;
  this->m = 0;
  this->n = 0;
  this->alloc = 0;

  if(ac == 0)
    mat_set_size(this, 0, 0);
  else if(ac == 1 && fts_is_int(at))
    mat_set_size(this, fts_get_int(at), 1);
  else if(ac == 2 && fts_is_int(at) && fts_is_int(at + 1))
    mat_set_size(this, fts_get_int(at), fts_get_int(at + 1));
  else if(fts_is_tuple(at))
    {
      int m = 0;
      int n = 0;
      int i;
      
      /* check n (longest row) and m */
      for(i=0; i<ac; i++)
	{
	  if(fts_is_tuple(at + i))
	    {
	      fts_tuple_t *tup = (fts_tuple_t *)fts_get_object(at + i);
	      int size = fts_tuple_get_size(tup);
	      
	      if(size > n)
		n = size;
	      
	      m++;
	    }
	  else
	    break;
	}
      
      mat_set_size(this, m, n);
      mat_set_from_tuples(this, ac, at);

      data_object_persistence_args(o);
    }
  else
    fts_object_set_error(o, "bad arguments");
}

static void
mat_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;
  
  if(this->m * this->n)
    fts_free(this->data);
}

static void
mat_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(mat_t), mat_init, mat_delete);

  fts_class_message_varargs(cl, fts_s_name, fts_name_set_method);
  fts_class_message_varargs(cl, fts_s_persistence, data_object_persistence);
  fts_class_message_varargs(cl, fts_s_update_gui, data_object_update_gui); 
  fts_class_message_varargs(cl, fts_s_dump, mat_dump);

  fts_class_message_varargs(cl, fts_s_post, mat_post); 
  fts_class_message_varargs(cl, fts_s_print, mat_print); 

  fts_class_message_varargs(cl, fts_s_set_from_instance, mat_set_from_instance);
  
  fts_class_message_varargs(cl, fts_s_fill, mat_fill);      
  fts_class_message_varargs(cl, fts_s_set, mat_set_elements);
  fts_class_message_varargs(cl, fts_s_row, mat_set_row_elements);

  fts_class_message_varargs(cl, fts_s_size, mat_size);
            
  fts_class_message_varargs(cl, fts_s_import, mat_import); 
  fts_class_message_varargs(cl, fts_s_export, mat_export); 

  fts_class_inlet_anything(cl, 0);
}

void
mat_config(void)
{
  sym_text = fts_new_symbol("text");
  sym_comma = fts_new_symbol(",");
  mat_symbol = fts_new_symbol("mat");

  mat_type = fts_class_install(mat_symbol, mat_instantiate);
}
