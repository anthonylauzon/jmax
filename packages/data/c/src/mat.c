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


#include <stdlib.h>    /* for qsort */
#include <string.h>
#include <fts/fts.h>
#include <fts/packages/data/data.h>

fts_symbol_t mat_symbol = NULL;
fts_class_t *mat_class  = NULL;

static fts_symbol_t sym_text = 0;
static fts_symbol_t sym_comma = 0;
static fts_symbol_t sym_mat_append_row  = 0;
static fts_symbol_t sym_mat_insert_rows = 0;
static fts_symbol_t sym_insert_cols = 0;
static fts_symbol_t sym_delete_cols = 0;
static fts_symbol_t sym_select_row = 0;



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
  int i, j;  
  int min_m = (m < mat->m)? m: mat->m;
  
  if (newsize > alloc)
  {
    if(mat->data == NULL)
      mat->data  = fts_malloc(newsize * sizeof(fts_atom_t));
    else
      mat->data  = fts_realloc(mat->data, newsize * sizeof(fts_atom_t));
      
    mat->alloc = newsize;
  }
  else
  {    
    if (newsize < 0)    /* size can be 0, but n or m nonzero */
      m = n = newsize = 0;
  }
  
  /*************************/
  if(n > mat->n)
  {     
    /* copy values (from last to first row) */
    for(i=min_m-1; i>=1; i--)
    {
      fts_atom_t *old_row = mat->data + i * mat->n;
      fts_atom_t *new_row = mat->data + i * n;
      
      /* copy old rows */
      for(j=mat->n-1; j>=0; j--)
        new_row[j] = old_row[j];
    }
    
    /* complete rows by zeros */
    for(i=0; i<mat->m; i++)
    {
      fts_atom_t *row = mat->data + i * n;
      
      /* zero end of new rows */
      for(j=mat->n; j<n; j++)
        fts_set_int(row + j, 0);
    }
  }
  else /* if(n < old_n) */
  {      
    /* copy and shorten rows */
    for(i=1; i<min_m; i++)
    {
      fts_atom_t *old_row = mat->data + i * mat->n;
      fts_atom_t *new_row = mat->data + i * n;
      
      /* copy beginning of old rows */
      for(j=0; j<n; j++)
        new_row[j] = old_row[j];
    }
  }
  
  /* zero new rows at end (if any) */
  for(i=min_m*n; i<newsize; i++)
    fts_set_int(mat->data + i, 0);
  
  /*********************/
  if (newsize < alloc)
  {
    /* void region cut off */
    for (i = newsize; i < oldsize; i++)
    {
      fts_atom_release(mat->data + i);
      fts_set_int(mat->data + i, 0);
    }
  }
  /*********************/
  
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
  
  if (onset + ac > size)
    ac = size - onset;
  
  for (i = 0; i < ac; i++)
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
  
  for (i = 0; i < m * n; i++)
    fts_atom_assign(copy->data + i, org->data + i);
}

/********************************************************************
 *
 *   upload methods
 *
 */
#define MAT_CLIENT_BLOCK_SIZE 128

static fts_memorystream_t *mat_memory_stream ;

static fts_memorystream_t * mat_get_memory_stream()
{
  if(!mat_memory_stream)
    mat_memory_stream = (fts_memorystream_t *)fts_object_create(fts_memorystream_class, 0, 0);
  
  return mat_memory_stream;
}

static void 
mat_upload_size(mat_t *self)
{
  fts_atom_t a[2];
  int m = mat_get_m(self);
  int n = mat_get_n(self);
  
  fts_set_int(a, m);
  fts_set_int(a+1, n);
  fts_client_send_message((fts_object_t *)self, fts_s_size, 2, a);
}

static void 
mat_upload_from_index(mat_t *self, int row_id, int col_id, int size, int upload)
{
  fts_atom_t a[MAT_CLIENT_BLOCK_SIZE];
  fts_atom_t *d;
  
  int n_cols = mat_get_n(self);
  int sent = 0;
  int data_size = size;
  int ms = row_id;
  int ns = col_id;
  int start_id = (ms*n_cols + ns);
  
  if(upload)
    fts_client_send_message((fts_object_t *)self, fts_s_start_upload, 0, 0);
  
  while( data_size > 0)
  {
    int i = 0;
    int n = (data_size > MAT_CLIENT_BLOCK_SIZE-2)? MAT_CLIENT_BLOCK_SIZE-2: data_size;
    
    /* starting row and column index */
    if( sent)
    {
      ms = sent/n_cols;
      ns = sent - ms*n_cols;
    }
    fts_set_int(a, ms);
    fts_set_int(a+1, ns);
    
    for(i=0; i<n; i++)
    {
      d = self->data + start_id  + sent + i;
      
      /* upload only an object description */
      if(fts_is_object(d))
      {
        fts_object_t *dobj = fts_get_object(d);
        
        fts_client_upload_object(dobj, fts_object_get_client_id((fts_object_t *)self));
        
        /*fts_send_message(dobj, fts_s_update_gui, 0, 0, fts_nix);*/
        
        fts_set_object(&a[2+i], dobj);               
      }
      else
        a[2+i] = *d;
    }
    fts_client_send_message((fts_object_t *)self, fts_s_set, n+2, a);
    
    sent += n;
    data_size -= n;
  }    
  
  if(upload)
    fts_client_send_message((fts_object_t *)self, fts_s_end_upload, 0, 0);
}

static void 
mat_upload_data(mat_t *self, int upload)
{  
  mat_upload_from_index(self, 0, 0, mat_get_m(self) * mat_get_n(self), upload);
}

void
mat_upload(mat_t *self)
{
  fts_client_send_message((fts_object_t *)self, fts_s_start_upload, 0, 0);
  mat_upload_size(self);
  mat_upload_data(self, 0);
  fts_client_send_message((fts_object_t *)self, fts_s_end_upload, 0, 0);
}

/********************************************************************
*
*   user methods
*
*/

static fts_method_status_t
mat_fill(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mat_t *self = (mat_t *) o;
  
  if (ac > 0)
  {
    mat_set_const(self, at);
  
    if(mat_editor_is_open(self))
      mat_upload_data(self, 1);
    
    fts_object_set_state_dirty(o);
  }

  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
mat_set_elements(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mat_t *self = (mat_t *) o;
  
  if(ac > 2 && fts_is_number(at) && fts_is_number(at + 1))
  {
    int m = mat_get_m(self);
    int n = mat_get_n(self);
    int i = fts_get_number_int(at);
    int j = fts_get_number_int(at + 1);
    
    if (i >= 0  &&  i < m  &&  j >= 0  &&  j < n)
    {
      mat_set_with_onset_from_atoms(self, i * n + j, ac - 2, at + 2);
    
      if(mat_editor_is_open(self))
        mat_upload_from_index(self, i, j, ac-2, 1);
      
      fts_object_set_state_dirty(o);
    }

    fts_object_changed(o);
  }

  fts_set_object(ret, o);         /* return ourself */

  return fts_ok;
}


static fts_method_status_t
mat_set_row_elements(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
    {
      mat_set_with_onset_from_atoms(self, i * n, ac, at);
    
      if(mat_editor_is_open(self))
        mat_upload_from_index(self, i, 0, ac, 1);
      
      fts_object_set_state_dirty(o);
    }
  
    /* return ourselves */
    fts_object_changed(o);
  }

  fts_set_object(ret, o);         /* return ourself */

  return fts_ok;
}


/** append a row of atoms, augment m, clip row to n 
 * 
 * @method append
 * @param  atoms        row of atoms to append, will be clipped to width of matrix
 */
static fts_method_status_t
mat_append_row(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
  
  if(mat_editor_is_open(self))
  {
    fts_client_send_message(o, sym_mat_append_row, 0, 0);
    mat_upload_from_index(self, m, 0, n, 1);
  }
  fts_object_set_state_dirty(o);
  
  /* return ourselves */
  fts_object_changed(o);
  fts_set_object(ret, o);

  return fts_ok;
}


/** insert @p num rows of atoms at row @p pos
 *  may insert num rows behind last row m --> append num rows
 *
 * @method insert
 * @param  int: pos    index of row where to insert @p num empty rows, default 0
 * @param  int: num    number of rows to insert, default 1
 * 
 * TODO: give data, clip rows to n, you can use mat_set_row_elements for now
 * @method insert
 * @param  int: pos    index of row where to insert
 * @param  tuples: atoms  list of tuples of rows of atoms to append, 
 *                     will be clipped to width of matrix
 */
static fts_method_status_t
mat_insert_rows (fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mat_t      *self    = (mat_t *) o;
  fts_atom_t *newptr;
  int         m       = mat_get_m(self);
  int         n       = mat_get_n(self);
  int         pos     = 0;      /* row position at which to insert */
  int         numrows = 1;      /* number of rows to insert */
  int         num, tomove, i;

  /* check and test args */
  if (ac > 0  &&  fts_is_number(at))
    pos = fts_get_number_int(at);

  if (pos < 0)   pos = 0;
  else if (pos > m)   pos = m;

  if (ac > 1  &&  fts_is_number(at+1))
    numrows = fts_get_number_int(at+1) ;
  
  fts_set_object(ret, o);  /* return ourself in any case */

  if (numrows <= 0)
    return fts_ok;

  /* make space, may change ptr, sets new atoms at the end to void */
  mat_set_size(self, m + numrows, n);

  /* move rows */
  newptr = mat_get_ptr(self) + n * pos;
  num    = n * numrows;         /* atoms to insert */
  tomove = n * (m - pos);       /* atoms to move */

  if (pos < m)  /* don't move when appending behind last row */
    memmove(newptr + num, newptr, tomove * sizeof(fts_atom_t));

  /* initialise inserted atoms (don't leave void atoms around) */
  for (i = 0; i < num; i++)
    fts_set_int(newptr + i, 0);

  /* update editor if open */
  if (mat_editor_is_open(self))
  {
    fts_atom_t a;
    mat_upload(self);
    
    fts_set_int(&a, pos);
    fts_client_send_message(o, sym_select_row, 1, &a);
  }
  fts_object_set_state_dirty(o);
  
  return fts_ok;
}


/** insert @p num rows of atoms at row @p pos
*  may insert num rows behind last row m --> append num rows
*
* @method insert
* @param  int: pos    index of row where to insert @p num empty rows, default 0
* @param  int: num    number of rows to insert, default 1
* 
* TODO: give data, clip rows to n, you can use mat_set_row_elements for now
* @method insert
* @param  int: pos    index of row where to insert
* @param  tuples: atoms  list of tuples of rows of atoms to append, 
*                      will be clipped to width of matrix
*/
static fts_method_status_t
mat_insert_columns(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mat_t *self = (mat_t *) o;
  int   m = mat_get_m(self);
  int   n = mat_get_n(self);
  int   pos = 0;        /* col position at which to insert */
  int numcols = 1;      /* number of cols to insert */
  int tomove, i, j, start, new_n;
  
  /* check and test args */
  if (ac > 0  &&  fts_is_number(at))
    pos = fts_get_number_int(at);
  
  if (pos < 0)  pos = 0;
  else if (pos > n)  pos = n;
  
  if (ac > 1  &&  fts_is_number(at+1))
    numcols = fts_get_number_int(at+1) ;
  
  fts_set_object(ret, o);  /* return ourself in any case */
  
  if (numcols <= 0)
    return fts_ok;
  
  /* make space, may change ptr, sets new atoms at the end to void */
  mat_set_size(self, m, n + numcols);
  new_n = n+numcols;
  
  /* move rows */
  start  = (m-1) * new_n + pos;
  tomove = new_n-pos-numcols;
  
  for (i = 0; i < m; i++)
  {
    for (j = tomove - 1; j >= 0; j--)
      self->data[start+j+numcols] = self->data[start+j];
    for (j = 0; j < numcols; j++)
      fts_set_int(&self->data[start + j], 0);
    start = start - new_n;
  }
  
  /* update editor if open */
  if(mat_editor_is_open(self))
    mat_upload(self);
  
  fts_object_set_state_dirty(o);
  
  return fts_ok;
}


static fts_method_status_t
mat_delete_columns(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mat_t *self = (mat_t *) o;
  int m = mat_get_m(self);
  int   n = mat_get_n(self);
  int   pos = 0;        /* col position at which to insert */
  int numcols = 1;      /* number of rows to insert */
  int tomove, i, j, start;
  
  /* check and test args */
  if (ac > 0  &&  fts_is_number(at))
    pos = fts_get_number_int(at);
  
  if (pos < 0)  pos = 0;
  else if (pos > n)  pos = n;
  
  if (ac > 1  &&  fts_is_number(at+1))
    numcols = fts_get_number_int(at+1) ;
  
  fts_set_object(ret, o);  /* return ourself in any case */

  if (numcols <= 0)
    return fts_ok;
  
  /* move rows */
  start  = pos + numcols;
  tomove = n-pos-numcols;
  
  for(i = 0; i < m; i++)
  {
    for (j = 0; j < tomove; j++)
      self->data[start-numcols+j] = self->data[start+j];
    start = start + n;
  }
  
  mat_set_size(self, m, n - numcols);
  
  /* update editor if open */
  if(mat_editor_is_open(self))
    mat_upload(self);
  
  fts_object_set_state_dirty(o);
  
  return fts_ok;
}


static void
mat_delete_rows (mat_t *self, int pos, int numrows)
{
  int         n = mat_get_n(self);
  int         numatom, tomove, i;
  fts_atom_t *killptr;
  
  killptr = mat_get_ptr(self) + n * pos;
  numatom = n * numrows;                  /* number of atoms to insert */
  tomove  = n * (mat_get_m(self) - pos);  /* number of atoms to move */

  /* release atoms to delete */
  for (i = 0; i < numatom; i++)
    fts_atom_release(killptr + i);

  /* move rows */
  memmove(killptr, killptr + numatom, tomove * sizeof(fts_atom_t));
  self->m -= numrows; 
}


/** delete @p num rows of atoms 
 * 
 * @method delete
 * @param  int: pos    index of row where to delete @p num rows, default 0
 * @param  int: num    number of rows to delete, default 1
 */
static fts_method_status_t
_mat_delete_rows (fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mat_t      *self    = (mat_t *) o;
  int         m       = mat_get_m(self);
  int         pos     = 0;
  int         numrows = 1;
  
  /* get and check args */
  if (ac > 0  &&  fts_is_number(at))
    pos = fts_get_number_int(at);
  
  fts_set_object(ret, o);  /* return ourself in any case */
    
  if (pos <  0)
    pos = 0;
  else if (pos >= m)
    return fts_ok;

  if (ac > 1  &&  fts_is_number(at+1))
    numrows = fts_get_number_int(at+1);

  if (numrows <= 0)
    return fts_ok;
  else if (numrows >  m - pos)
    numrows = m - pos;

  mat_delete_rows(self, pos, numrows);

  /* update editor if open */
  if (mat_editor_is_open(self))
  {
    fts_atom_t a;
    mat_upload(self);
    
    fts_set_int(&a, (pos>0) ? pos-1 : pos);
    fts_client_send_message(o, sym_select_row, 1, &a);
  }
  
  fts_object_set_state_dirty(o);
  
  return fts_ok;
}



static fts_method_status_t
mat_return_size(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mat_t *self = (mat_t *) o;
  fts_atom_t a[2];
  
  if (s == fts_s_rows)
    fts_set_int(ret, mat_get_m(self));
  else if (s == fts_s_cols)
    fts_set_int(ret, mat_get_n(self));
  else
  {
    fts_set_int(a + 0, mat_get_m(self));
    fts_set_int(a + 1, mat_get_n(self));
    fts_set_object(ret, fts_object_create(fts_tuple_class, 2, a));
  }
  
  return fts_ok;
}


static fts_method_status_t
mat_change_size(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mat_t *self = (mat_t *) o;
  int m = 0;
  int n = 0;
  
  if (ac == 1  &&  fts_is_number(at))
  { /* only change # of rows */
    m = fts_get_number_int(at);
    n = mat_get_n(self);
    
    if (m >= 0  &&  n >= 0)
    {
      mat_set_size(self, m, n);         /* initialises added atoms */
      
      if(mat_editor_is_open(self))
        mat_upload(self);
      
      fts_object_set_state_dirty(o);
    }
  }
  else if (ac == 2  &&  fts_is_number(at)  &&  fts_is_number(at + 1))
  {
    m = fts_get_number_int(at);
    n = fts_get_number_int(at + 1);
    
    if (m >= 0  &&  n >= 0)
    {
      mat_set_size(self, m, n);         /* initialises added atoms */
  
      if(mat_editor_is_open(self))
        mat_upload(self);
      
      fts_object_set_state_dirty(o);
    }
  }
  
  fts_set_object(ret, o);  /* return ourself */

  return fts_ok;
}


static fts_method_status_t
mat_return_element(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mat_t *self = (mat_t *) o;
  int	  m   = mat_get_m(self);
  int	  n   = mat_get_n(self);
  int	  i   = 0;
  int	  j   = 0;
  
  if (m == 0  ||  n == 0)
    fts_set_float(ret, 0);        /* empty matrix: no error, just return 0 */
  else
  {
    if (ac > 0  &&  fts_is_number(at))
      i = fts_get_number_int(at);

    if (ac > 1  &&  fts_is_number(at + 1))
      j = fts_get_number_int(at  + 1);
  
    /* handle wraparound/clipping */
    while (i < 0)
      i += m;
  
    while (j < 0)
      j += n;
  
    if (i >= m)
      i = m - 1;
  
    if (j >= n)
      j = n - 1;
 
    *ret = *(mat_get_element(self, i, j));
  }
  
  return fts_ok;
}

static fts_method_status_t
mat_set_from_instance(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mat_t *self = (mat_t *) o;
  mat_t *set  = (mat_t *) fts_get_object(at);
  
  mat_copy(set, self);
  
  if(mat_editor_is_open(self))
    mat_upload(self);

  fts_object_set_state_dirty(o);
  fts_set_object(ret, o);

  return fts_ok;
}


#ifdef HAVE_QSORT_R

static int 
mat_element_compare_ascending (void *thunk, const void *a, const void *b)
{
  int col = (int) thunk;

  return fts_atom_compare((const fts_atom_t *) a + col, 
                  (const fts_atom_t *) b + col);
}

static int 
mat_element_compare_descending (void *thunk, const void *a, const void *b)
{
  int col = (int) thunk;

  return fts_atom_compare((const fts_atom_t *) b + col, 
                  (const fts_atom_t *) a + col);
}

#else

static int 
mat_element_compare_ascending (const void *a, const void *b)
{
  return fts_atom_compare((const fts_atom_t *) a, (const fts_atom_t *) b);
}

static int 
mat_element_compare_descending (const void *a, const void *b)
{
  return fts_atom_compare((const fts_atom_t *) b, (const fts_atom_t *) a);
}

#endif /* HAVE_QSORT_R */


static fts_method_status_t
mat_sort(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mat_t *self = (mat_t *) o;
  fts_atom_t *ptr = mat_get_ptr(self);
  int m = mat_get_m(self);
  int n = mat_get_n(self); 
  int col = 0;
  int (*compare) (const void *, const void *);  /* comparison function */

  if (s == fts_s_sort)
    compare = mat_element_compare_ascending;
  else
    compare = mat_element_compare_descending;
  
  if (ac > 0)
    col = fts_get_number_int(at);

#if HAVE_QSORT_R
  qsort_r((void *) ptr, m, n * sizeof(fts_atom_t), (void *) col, compare);
#else
  if (col == 0)
    qsort((void *) ptr, m, n * sizeof(fts_atom_t), compare);
  else if (col > 0  &&  col < n)
  {
    int i;
    
    /* swap sort column to first one */
    for (i = 0; i < m * n; i += n)
    {
      fts_atom_t a = ptr[i];
      
      ptr[i]       = ptr[i + col];
      ptr[i + col] = a;
    }
    
    qsort((void *) ptr, m, n * sizeof(fts_atom_t), compare);

    /* swap column back */
    for (i = 0; i < m * n; i += n)
    {
      fts_atom_t a = ptr[i];
      
      ptr[i]       = ptr[i + col];
      ptr[i + col] = a;
    }    
  }
#endif /* HAVE_QSORT_R */
  
  if (mat_editor_is_open(self))
    mat_upload_data(self, 1);
  
  fts_set_object(ret, o);
  
  return fts_ok;
}


static fts_method_status_t
mat_unique (fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mat_t *self = (mat_t *) o;
  int    col  = 0;
  int    i, j;
  
  if (ac > 0)
    col = fts_get_number_int(at);

  /* don't mat_sort(o, s, ac, at, fts_nix); */

  if (col < mat_get_n(self))
  {
    for (i = 0; i < mat_get_m(self) - 1; i++)
    {
      j = i + 1;

      while (fts_atom_compare(mat_get_element(self, i, col),
			      mat_get_element(self, j, col)) == 0)
	j++;	/* look for contiguous lines with equal index */

      if (j > i + 1)
      { /* remove lines with equal index.  
	   N.B.: self->m is changed! */
	mat_delete_rows(self, i + 1, j - i - 1);
      }
    }

    if (mat_editor_is_open(self))
      mat_upload_data(self, 1);
  }
  /* else: the column doesn't exist, do nothing */

  fts_set_object(ret, o);
  
  return fts_ok;
}


/* dumping for save/paste/etc. */

static fts_method_status_t
mat_dump_state(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mat_t *self = (mat_t *) o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_atom_t *data = self->data;
  int m = mat_get_m(self);
  int n = mat_get_n(self);
  fts_message_t *mess = (fts_message_t *)fts_object_create(fts_message_class, 0, 0);
  int i;
  
  fts_object_refer((fts_object_t *)mess);

  fts_message_set(mess, fts_s_size, 0, 0);
  fts_message_append_int(mess, m);
  fts_message_append_int(mess, n);
  fts_dumper_message_send(dumper, mess);
  
  if(n > 0)
  {
    for(i=0; i<m; i++)
    {
      fts_message_set(mess, fts_s_row, 0, 0);
      fts_message_append_int(mess, i); /* row index */
      fts_message_append(mess, n, data + i * n); /* row data */      
      fts_dumper_message_send(dumper, mess);
    }
  }

  fts_object_release((fts_object_t *)mess);
  
  return fts_ok;
}

static fts_method_status_t
mat_print(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mat_t *self = (mat_t *) o;
  int m = mat_get_m(self);
  int n = mat_get_n(self);
  int size = m * n;
  fts_bytestream_t* stream = fts_get_default_console_stream();
  
  if(ac > 0 && fts_is_object(at))
    stream = (fts_bytestream_t *)fts_get_object(at);
  
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
  
  fts_set_object(ret, o);  /* return ourself */

  return fts_ok;
}


static int
mat_equals(const mat_t *a, const mat_t *b)
{
  if(mat_get_m(a) == mat_get_m(b) && mat_get_n(a) == mat_get_n(b))
  {
    int size = mat_get_m(a) * mat_get_n(a);
    fts_atom_t *a_ptr = mat_get_ptr(a);
    fts_atom_t *b_ptr = mat_get_ptr(b);
    int i;
    
    for(i=0; i<size; i++)
      if(!fts_atom_equals(a_ptr + i, b_ptr + i))
        return 0;
    
    return 1;
  }
  
  return 0;
}

static fts_method_status_t
_mat_equals (fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mat_t *self = (mat_t *) o;

  fts_set_int(ret, mat_equals(self, (mat_t *) fts_get_object(at)));

  return fts_ok;
}


/* class copy method compatible wrapper around copy function */
static void
mat_copy_function(const fts_object_t *from, fts_object_t *to)
{
  mat_t *dest = (mat_t *) to;
  mat_copy((mat_t *) from, dest);
  
  if(mat_editor_is_open(dest))
    mat_upload(dest);
}

static fts_method_status_t
mat_open_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mat_t *this = (mat_t *)o;
  
  mat_set_editor_open(this);
  fts_client_send_message(o, fts_s_openEditor, 0, 0);
  
  mat_upload(this);
  
  return fts_ok;
}

static fts_method_status_t 
mat_close_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mat_t *this = (mat_t *) o;
  
  if(mat_editor_is_open(this))
  {
    mat_set_editor_close(this);
    fts_client_send_message((fts_object_t *)this, fts_s_closeEditor, 0, 0);  
  }

  return fts_ok;
}

static fts_method_status_t
mat_destroy_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mat_t *this = (mat_t *)o;
  
  mat_set_editor_close(this);
  
  return fts_ok;
}


/********************************************************************
*
*   class
*
*/
static fts_method_status_t
mat_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mat_t *self = (mat_t *) o;
  
  self->data = NULL;
  self->m = 0;
  self->n = 0;
  self->alloc = 0;
  self->opened = 0;
  
  if (ac == 0)
    mat_set_size(self, 0, 0);
  else if (ac == 1  &&  fts_is_int(at))
    mat_set_size(self, fts_get_int(at), 1);
  else if (ac == 2  &&  fts_is_int(at)  && fts_is_int(at + 1))
    mat_set_size(self, fts_get_int(at), fts_get_int(at + 1));
  else if(ac > 2 &&  fts_is_int(at)  && fts_is_int(at + 1)) /* init from atom list */
  {
    int m = fts_get_int(at);
    int n = fts_get_int(at + 1);
    int size = m * n;    
    int i = 0;
    mat_set_size(self, m, n);
    mat_set_with_onset_from_atoms( self, 0, ac-2, at+2);
 
    if (ac-2 < size)
      for(i = size-ac+3; i < ac-2; i++)
        fts_set_int( self->data + i, 0);
  }
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
        
        if (size > n)
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
  
  return fts_ok;
}

static fts_method_status_t
mat_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mat_t *self = (mat_t *) o;
  fts_atom_t *data = self->data;
  int m = mat_get_m(self);
  int n = mat_get_n(self);
  int size = m * n;
  int i;
  
  fts_client_send_message(o, fts_s_destroyEditor, 0, 0);
  
  for(i=0; i< size; i++)
    fts_atom_release(data + i);
  
  if(self->data != NULL)
    fts_free(self->data);
  
  return fts_ok;
}

static void
mat_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(mat_t), mat_init, mat_delete);
  
  fts_class_set_copy_function(cl, mat_copy_function);

  fts_class_message_varargs(cl, fts_s_name, fts_object_name);
  fts_class_message_varargs(cl, fts_s_persistence, fts_object_persistence);
  fts_class_message_varargs(cl, fts_s_dump_state, mat_dump_state);
  fts_class_message_varargs(cl, fts_s_print, mat_print); 
  fts_class_message        (cl, fts_s_equals, mat_class, _mat_equals);
  
  fts_class_message_varargs(cl, fts_s_set_from_instance, mat_set_from_instance);
  fts_class_message        (cl, fts_s_set, mat_class,    mat_set_from_instance);
  
  fts_class_message_varargs(cl, fts_s_fill, mat_fill);      
  fts_class_message_varargs(cl, fts_s_set, mat_set_elements);
  fts_class_message_varargs(cl, fts_s_row, mat_set_row_elements);
  fts_class_message_varargs(cl, fts_s_append, mat_append_row);
  fts_class_message_varargs(cl, fts_s_insert, mat_insert_rows);
  fts_class_message_varargs(cl, fts_s_delete, _mat_delete_rows);
  fts_class_message_varargs(cl, sym_insert_cols, mat_insert_columns);
  fts_class_message_varargs(cl, sym_delete_cols, mat_delete_columns);

  fts_class_message_void   (cl, fts_s_sort,    mat_sort);
  fts_class_message_number (cl, fts_s_sort,    mat_sort);
  fts_class_message_void   (cl, fts_s_sortrev, mat_sort);
  fts_class_message_number (cl, fts_s_sortrev, mat_sort);
  fts_class_message_void   (cl, fts_s_unique,  mat_unique);
  fts_class_message_number (cl, fts_s_unique,  mat_unique);
  
  fts_class_message_void (cl, fts_s_size, mat_return_size);
  fts_class_message_void (cl, fts_s_rows, mat_return_size);
  fts_class_message_void (cl, fts_s_cols, mat_return_size);
  fts_class_message_number (cl, fts_s_rows, mat_change_size);
/* set cols??? */
  fts_class_message_varargs(cl, fts_s_size, mat_change_size);

  fts_class_message_varargs(cl, fts_s_get_element, mat_return_element);
  
  fts_class_message_varargs(cl, fts_s_openEditor, mat_open_editor);
  fts_class_message_varargs(cl, fts_s_closeEditor, mat_close_editor); 
  fts_class_message_varargs(cl, fts_s_destroyEditor, mat_destroy_editor);  
  
  fts_class_inlet_bang(cl, 0, data_object_output);
  
  fts_class_inlet_thru (cl, 0);
  fts_class_outlet_thru(cl, 0);
  
  
  /*
   * class doc 
   */
  
  fts_class_doc(cl, mat_symbol, "[<num: # of rows> [<num: # of columns (default is 1)>]] | [<tuple: { one row of init values }> ...]", "matrix of atoms");
  fts_class_doc(cl, fts_s_set, "<num: row index> <num: column index> [<num:value> ...]" , "set matrix values at given index");
  fts_class_doc(cl, fts_s_set, "<mat: other>", "set from mat instance");
  fts_class_doc(cl, fts_s_row, "<num: index> [<num:value> ...]", "set values of given row");

  fts_class_doc(cl, fts_s_append, "<list: values>", "append row of atoms (up to given number of columns)");
  fts_class_doc(cl, fts_s_insert, "<int: pos> <int: num>", "insert num empty rows at row pos");
  fts_class_doc(cl, fts_s_delete, "<int: pos> <int: num>", "delete num rows from row pos");

  fts_class_doc(cl, fts_s_fill, "<atom: value>", "fill matrix with given value");
  fts_class_doc(cl, fts_s_size, "[<num: # of rows> [<num: # of columns (default is 1)>]]", "get/set size");
  fts_class_doc(cl, fts_s_rows, "<void | int: num>", "get or set # of rows");
  fts_class_doc(cl, fts_s_cols, "<void>", "get # of columns");
  
  fts_class_doc(cl, fts_s_get_element, "<num: row index> <num: column index>", "get value at given index");
}

FTS_MODULE_INIT(mat)
{
  sym_text = fts_new_symbol("text");
  sym_comma = fts_new_symbol(",");
  sym_mat_append_row  = fts_new_symbol("mat_append_row");
  sym_mat_insert_rows = fts_new_symbol("mat_insert_rows");
  sym_insert_cols = fts_new_symbol("insert_cols");
  sym_delete_cols = fts_new_symbol("delete_cols");
  sym_select_row = fts_new_symbol("select_row");
  mat_symbol = fts_new_symbol("mat");
  
  mat_class = fts_class_install(mat_symbol, mat_instantiate);
}


/** EMACS **
* Local variables:
* mode: c
* c-basic-offset:2
* End:
*/
