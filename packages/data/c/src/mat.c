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

fts_symbol_t mat_symbol = 0;
fts_class_t *mat_type = 0;

static fts_symbol_t sym_text = 0;
static fts_symbol_t sym_comma = 0;


/********************************************************
 *
 *  utility functions
 *
 */

/* set matrix shape (without shuffling data around!), allocate space
   if necessary, initialise added atoms when growing, release lost
   atoms when shrinking */
void
mat_set_size(mat_t *mat, int m, int n)
{
  int alloc   = mat->alloc;
  int oldsize = mat->m * mat->n;
  int newsize = m * n;
  int i;
  
  if (newsize > alloc)
  {
      mat->data  = fts_realloc(mat->data, newsize * sizeof(fts_atom_t));
      mat->alloc = newsize;

      /* set newly allocated region to void */
      for (i = oldsize; i < newsize; i++)
	  fts_set_int(mat->data + i, 0);
  }
  else
  {
      if (newsize < 0)	/* size can be 0, but n or m nonzero */
	  m = n = newsize = 0;

      /* void region cut off */
      for (i = newsize; i < oldsize; i++)
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

  while(size > alloc)
    alloc += MAT_BLOCK_SIZE;

  mat_set_size(mat, alloc, 1);	/* initialises added atoms */
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
	  if(fts_atom_same_type(&a, &at[k]) && fts_get_int(&a) == fts_get_int(&at[k]))
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
  mat_t *self = (mat_t *) o;

  if (ac > 0)
    mat_set_const(self, at);
}

static void
mat_set_elements(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *self = (mat_t *) o;

  if(ac > 2 && fts_is_number(at) && fts_is_number(at + 1))
  {
      int m = mat_get_m(self);
      int n = mat_get_n(self);
      int i = fts_get_number_int(at);
      int j = fts_get_number_int(at + 1);

      if (i >= 0  &&  i < m  &&  j >= 0  &&  j < n)
	mat_set_with_onset_from_atoms(self, i * n + j, ac - 2, at + 2);
  }
}


static void
mat_set_row_elements(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *self = (mat_t *) o;

  if(ac > 1 && fts_is_number(at))
  {
      int m = mat_get_m(self);
      int n = mat_get_n(self);
      int i = fts_get_number_int(at);

      ac--;
      at++;
      
      /* clip to row */
      if (ac > n)
	ac = n;

      if (i >= 0  &&  i < m)
	mat_set_with_onset_from_atoms(self, i * n, ac, at);
  }
}


/* append a row of atoms, augment m, clip row to n */
static void
mat_append_row(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *self = (mat_t *) o;
  int m = mat_get_m(self);
  int n = mat_get_n(self);

  /* clip to row */
  if (ac > n)
      ac = n;

  /* add space, append data */
  mat_set_size(self, m + 1, n);
  mat_set_with_onset_from_atoms(self, m * n, ac, at);
}


static void
mat_return_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *self = (mat_t *) o;
  fts_atom_t a[2];
  fts_atom_t t;

  fts_set_int(a + 0, mat_get_m(self));
  fts_set_int(a + 1, mat_get_n(self));
  fts_set_object(&t, fts_object_create(fts_tuple_class, 2, a));

  fts_return(&t);
}

static void
mat_change_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *self = (mat_t *) o;
  int m = 0;
  int n = 0;

  if (ac == 1  &&  fts_is_number(at))
  {
    m = fts_get_number_int(at);
    n = mat_get_n(self);

    if (m >= 0  &&  n >= 0)
      mat_set_size(self, m, n);		/* initialises added atoms */
  }
  else if (ac == 2  &&  fts_is_number(at)  &&  fts_is_number(at + 1))
  {
    m = fts_get_number_int(at);
    n = fts_get_number_int(at + 1);

    if (m >= 0  &&  n >= 0)
      mat_set_size(self, m, n); 	/* initialises added atoms */
  }
}

static void
mat_return_element(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *self = (mat_t *) o;

  if(ac == 2 && fts_is_number(at) && fts_is_number(at + 1))
  {
    int i = fts_get_number_int(at);
    int j = fts_get_number_int(at + 1);

    if (i >= 0  &&  i < mat_get_m(self)  &&  j >= 0  &&  j < mat_get_n(self))
      fts_return(mat_get_element(self, i, j));
  }
}

static void
mat_import(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *self = (mat_t *) o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_format = fts_get_symbol_arg(ac, at, 1, sym_text);

  if(!file_name)
    return;

  if(file_format == sym_text)
    {
      fts_symbol_t separator = fts_get_symbol_arg(ac, at, 2, 0);
      int size = 0;

      if (separator)
	size = mat_read_atom_file_separator(self, file_name, separator, ac - 3, at + 3);
      else
	size = mat_read_atom_file_newline(self, file_name);

      if(size <= 0)
	fts_post("mat: can't import from text file \"%s\"\n", fts_symbol_name(file_name));
    }
  else
    fts_post("mat: unknown import file format \"%s\"\n", fts_symbol_name(file_format));
}

static void
mat_export(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *self = (mat_t *) o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_format = fts_get_symbol_arg(ac, at, 1, sym_text);

  if(!file_name)
    return;

  if(file_format == sym_text)
    {
      fts_symbol_t separator = fts_get_symbol_arg(ac, at, 2, 0);
      int size = 0;

      if (separator)
	size = mat_write_atom_file_separator(self, file_name, separator);
      else
	size = mat_write_atom_file_newline(self, file_name);

      if(size < 0)
	fts_post("mat: can't export to text file \"%s\"\n", fts_symbol_name(file_name));
    }
  else
    fts_post("mat: unknown export file format \"%s\"\n", fts_symbol_name(file_format));
}

static void
mat_set_from_instance(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *self = (mat_t *) o;
  mat_t *set  = (mat_t *) fts_get_object(at);

  mat_copy(set, self);
}

static void
mat_dump_state(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *self = (mat_t *) o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_atom_t *data = self->data;
  int m = mat_get_m(self);
  int n = mat_get_n(self);
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
	  
      if(n > 0)
	fts_dumper_message_send(dumper, mess);
    }
}

static void
mat_post(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *self = (mat_t *) o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
  int m = mat_get_m(self);
  int n = mat_get_n(self);
  int size = m * n;
  
  if(size == 0)
    fts_spost(stream, "<mat>");
  else
    fts_spost(stream, "<mat %d %d>", m, n);
}

static void
mat_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *self = (mat_t *) o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
  int m = mat_get_m(self);
  int n = mat_get_n(self);
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
	  fts_spost_atoms(stream, n, mat_get_row(self, i));
	  fts_spost(stream, ",\n");
	}

      fts_spost(stream, "}\n");
    }
}

static int
mat_equals(const fts_atom_t *a, const fts_atom_t *b)
{
  mat_t *o = (mat_t *)fts_get_object(a);
  mat_t *p = (mat_t *)fts_get_object(b);

  if(mat_get_m(o) == mat_get_m(p) && mat_get_n(o) == mat_get_n(p))
  {
    int size = mat_get_m(o) * mat_get_n(o);
    fts_atom_t *o_ptr = mat_get_ptr(o);
    fts_atom_t *p_ptr = mat_get_ptr(p);
    int i;

    for(i=0; i<size; i++)
      if(!fts_atom_equals(o_ptr + i, p_ptr + i))
        return 0;

    return 1;
  }

  return 0;
}

/********************************************************************
*
*   class
*
*/
static void
mat_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *self = (mat_t *) o;
  
  self->data = NULL;
  self->m = 0;
  self->n = 0;
  self->alloc = 0;
  
  if (ac == 0)
    mat_set_size(self, 0, 0);
  else if (ac == 1  &&  fts_is_int(at))
    mat_set_size(self, fts_get_int(at), 1);
  else if (ac == 2  &&  fts_is_int(at)  && fts_is_int(at + 1))
    mat_set_size(self, fts_get_int(at), fts_get_int(at + 1));
  else if (fts_is_tuple(at))
  {
    int m = 0;
    int n = 0;
    int i;
    
    /* check n (longest row) and m */
    for (i = 0; i < ac; i++)
    {
      if (fts_is_tuple(at + i))
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
    
    mat_set_size(self, m, n);
    mat_set_from_tuples(self, ac, at);
  }
  else
    fts_object_error(o, "bad arguments");
}

static void
mat_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *self = (mat_t *) o;
  
  if (self->data != NULL)
    fts_free(self->data);
}


static void
mat_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(mat_t), mat_init, mat_delete);

  fts_class_message_varargs(cl, fts_s_name, fts_object_name);
  fts_class_message_varargs(cl, fts_s_persistence, fts_object_persistence);
  fts_class_message_varargs(cl, fts_s_dump_state, mat_dump_state);

  fts_class_message_varargs(cl, fts_s_post, mat_post); 
  fts_class_message_varargs(cl, fts_s_print, mat_print); 

  fts_class_set_equals_function(cl, mat_equals);
  
  fts_class_message_varargs(cl, fts_s_set_from_instance, mat_set_from_instance);
  
  fts_class_message_varargs(cl, fts_s_fill, mat_fill);      
  fts_class_message_varargs(cl, fts_s_set, mat_set_elements);
  fts_class_message_varargs(cl, fts_s_row, mat_set_row_elements);
  fts_class_message_varargs(cl, fts_new_symbol("append"), mat_append_row);

  fts_class_message_varargs(cl, fts_s_import, mat_import); 
  fts_class_message_varargs(cl, fts_s_export, mat_export);

  fts_class_message_void(cl, fts_s_size, mat_return_size);
  fts_class_message_varargs(cl, fts_s_size, mat_change_size);

  fts_class_message_varargs(cl, fts_s_get_element, mat_return_element);
  
  fts_class_inlet_bang(cl, 0, data_object_output);

  fts_class_inlet_thru(cl, 0);
  fts_class_outlet_thru(cl, 0);


  /*
   * class doc 
   */

  fts_class_doc(cl, mat_symbol, "[<num: # of rows> [<num: # of columns (default is 1)>]] | \n    [<tuple: { one row of init values }> ...]", "matrix of atoms");
  fts_class_doc(cl, fts_s_set, "<num: row index> <num: column index> [<num:value> ...]" , "set matrix values at given index");
  fts_class_doc(cl, fts_s_set, "<mat: other>", "set from mat instance");
  fts_class_doc(cl, fts_s_row, "<num: index> [<num:value> ...]", "set values of given row");
  fts_class_doc(cl, fts_new_symbol("append"), "<list: values>", "append row of atoms");
  fts_class_doc(cl, fts_s_fill, "<atom: value>", "fill matrix with given value");
  fts_class_doc(cl, fts_s_size, "[<num: # of rows> [<num: # of columns (default is 1)>]]", "get/set size");
  
  fts_class_doc(cl, fts_s_get_element, "<num: row index> <num: column index>", "get value at given index");

  fts_class_doc(cl, fts_s_import, "[<sym: file name]", "import data from file");
  fts_class_doc(cl, fts_s_export, "[<sym: file name]", "export data to file");
}


void
mat_config(void)
{
  sym_text = fts_new_symbol("text");
  sym_comma = fts_new_symbol(",");
  mat_symbol = fts_new_symbol("mat");

  mat_type = fts_class_install(mat_symbol, mat_instantiate);
}


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
