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
#include <fts/packages/utils/utils.h>
#include <fts/packages/data/data.h>
#include "tabeditor.h"

#include <stdlib.h>
#include <ctype.h>
#include <float.h>

#define FVEC_NO_ALLOC -1

fts_symbol_t fvec_symbol = 0;
fts_class_t *fvec_type = 0;

static fts_symbol_t sym_text = 0;
static fts_symbol_t sym_open_file = 0;

/********************************************************
 *
 *  utility functions
 *
 */

void
fvec_set_size(fvec_t *vec, int size)
{
  int alloc = vec->alloc;
  float *values = vec->values;

  if(size < 0)
    size = 0;

  if(size > alloc)
    {
      if(values == NULL)
	values = (float *)fts_malloc((size + FTS_CUBIC_HEAD + FTS_CUBIC_TAIL) * sizeof(float));
      else
	values = (float *)fts_realloc((vec->values - FTS_CUBIC_HEAD), (size + FTS_CUBIC_HEAD + FTS_CUBIC_TAIL) * sizeof(float));

      values += FTS_CUBIC_HEAD;

      values[-1] = 0.0;
      values[size] = 0.0;
      values[size + 1] = 0.0;

      vec->values = values;
      vec->alloc = size;
    }

  vec->m = size;
}

void
fvec_set_const(fvec_t *vec, float c)
{
  float *values = vec->values;
  int i;
  
  for(i=0; i<vec->m; i++)
    values[i] = c;
}

void
fvec_set_with_onset_from_atoms(fvec_t *vec, int offset, int ac, const fts_atom_t *at)
{
  int size = fvec_get_size(vec);
  int i;
  
  if(offset + ac > size)
    ac = size - offset;
  
  for(i=0; i<ac; i++)
    {
      if(fts_is_number(at + i))
	vec->values[i + offset] = (float)fts_get_number_float(at + i);
      else
	vec->values[i + offset] = 0.0f;
    }
}

/* sum, min, max */
static float 
fvec_get_sum(fvec_t *vec)
{
  float sum = 0;
  int i;

  for(i=0; i<vec->m; i++)
    sum += vec->values[i];

  return sum;
}

static float
fvec_get_min_value(fvec_t *vec)
{
  float min;
  int i;

  min = vec->values[0];

  for (i=1; i<vec->m; i++)
    if (vec->values[i] < min)
      min = vec->values[i];

  return min;
}


float
fvec_get_max_value(fvec_t *vec)
{
  float max;
  int i;

  max = vec->values[0];

  for (i=1; i<vec->m; i++)
    if (vec->values[i] > max)
      max = vec->values[i];

  return max;
}

float
fvec_get_max_abs_value_in_range(fvec_t *vec, int a, int b)
{
  float max, abs_max, abs_val;
  int i;

  max = vec->values[a];
  abs_max = (float)fabs((double)max);

  for (i = a+1; (i < vec->m) && (i < b); i++)
  {
    abs_val = (float)fabs((double)vec->values[i]);
    if ( abs_val > abs_max)
    {
      max = vec->values[i];
      abs_max = abs_val;
    }
  }
  return max;
}

float
fvec_get_max_value_in_range(fvec_t *vec, int a, int b)
{
  float max;
  int i;

  max = vec->values[a];

  for (i = a+1; (i < vec->m) && (i < b); i++)
    if ( vec->values[i] > max)
      max = vec->values[i];

  return max;
}

float
fvec_get_min_value_in_range(fvec_t *vec, int a, int b)
{
  float min;
  int i;

  min = vec->values[a];

  for (i = a+1; (i < vec->m) && (i < b); i++)
    if ( vec->values[i] < min)
      min = vec->values[i];

  return min;
}

void
fvec_copy(fvec_t *org, fvec_t *copy)
{
  int size = fvec_get_size(org);
  int i;

  fvec_set_size(copy, size);

  for(i=0; i<size; i++)
    copy->values[i] = org->values[i];  
}

/********************************************************
 *
 *  files
 *
 */

#define FVEC_BLOCK_SIZE 256

static void
fvec_grow(fvec_t *vec, int size)
{
  int alloc = vec->alloc;

  while(!alloc || size > alloc)
    alloc += FVEC_BLOCK_SIZE;

  fvec_set_size(vec, alloc);
}

int 
fvec_read_atom_file(fvec_t *vec, fts_symbol_t file_name)
{
  fts_atom_file_t *file = fts_atom_file_open(file_name, "r");
  int n = 0;
  fts_atom_t a;
  char c;

  if(file)
    {
      while(fts_atom_file_read(file, &a, &c))
	{
	  if(n >= vec->alloc)
	    fvec_grow(vec, n);
	  
	  if(fts_is_number(&a))
	    fvec_set_element(vec, n, (float)fts_get_number_float(&a));
	  else
	    fvec_set_element(vec, n, 0.0f);
	  
	  n++;
	}
      
      if(n > 0)
	fvec_set_size(vec, n);
      else
	fts_object_error((fts_object_t *)vec, "cannot load from file \"%s\"", fts_symbol_name(file_name));
      
      fts_atom_file_close(file);
    }
  else
    fts_object_error((fts_object_t *)vec, "cannot open file \"%s\"", fts_symbol_name(file_name));
  
  return n;
}

int
fvec_write_atom_file(fvec_t *vec, fts_symbol_t file_name)
{
  fts_atom_file_t *file;
  int size = fvec_get_size(vec);
  int i;

  file = fts_atom_file_open(file_name, "w");

  if(!file)
    return -1;

  /* write the content of the vec */
  for(i=0; i<size; i++)     
    {
      fts_atom_t a;
      
      fts_set_float(&a, fvec_get_element(vec, i));
      fts_atom_file_write(file, &a, '\n');
    }

  fts_atom_file_close(file);

  return (i);
}

static int
fvec_load_audiofile(fvec_t *vec, fts_symbol_t file_name, int onset, int n_read)
{
  fts_audiofile_t *sf = fts_audiofile_open_read(file_name);
  int size = 0;
  
  if(fts_audiofile_is_valid(sf))
    {
      float *ptr;
      
      if(onset > 0 && fts_audiofile_seek(sf, onset) != 0) 
	{
	  fts_object_error((fts_object_t *)vec, "cannot seek position in file \"%s\"", fts_symbol_name(file_name));
	  fts_audiofile_close(sf);
	  return 0;
	}
      
      if(!n_read)
	n_read = fts_audiofile_get_num_frames(sf);
      
      fvec_set_size(vec, n_read);
      ptr = fvec_get_ptr(vec);
      
      size = fts_audiofile_read(sf, &ptr, 1, n_read);
      
      fts_audiofile_close(sf);
      
      if(size <= 0)
	{
	  fts_object_error((fts_object_t *)vec, "cannot load from soundfile \"%s\"", fts_symbol_name(file_name));
	  size = 0;
	}
    }
  else
    {
      fts_object_error((fts_object_t *)vec, "cannot open file \"%s\"", fts_symbol_name(file_name));
      fts_audiofile_close(sf);
    }

  return size;
}


/* if another object changed our data, do the necessary stuff */
void fvec_changed(fvec_t *this)
{
    if (this->editor)
	tabeditor_send((tabeditor_t *) this->editor);

    data_object_set_dirty((fts_object_t *) this);
}



/*********************************************************
*
*  client methods
*
*
*********************************************************/

static void 
fvec_open_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fts_atom_t a;

  if(this->editor == NULL)
    {
      fts_set_object(&a, o);
      this->editor = fts_object_create( tabeditor_type, 1, &a);
      fts_object_refer( this->editor);
    }

  if( !fts_object_has_id( this->editor))
    {
      fts_client_register_object( this->editor, fts_object_get_client_id( o));
	  
      fts_set_int(&a, fts_object_get_id( this->editor));
      fts_client_send_message( o, fts_s_editor, 1, &a);
      
      /*fts_send_message( (fts_object_t *)this->editor, fts_s_upload, 0, 0);*/
    }

  fvec_set_editor_open( this);
  fts_client_send_message(o, fts_s_openEditor, 0, 0);
}

static void
fvec_destroy_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  fvec_set_editor_close( this);
}

static void 
fvec_close_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *) o;

  if(fvec_editor_is_open(this))
    {
      fvec_set_editor_close(this);
      fts_client_send_message((fts_object_t *)this, fts_s_closeEditor, 0, 0);  
    }
}

/********************************************************************
 *
 *   user methods
 *
 */

static void
fvec_fill(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  fvec_set_const(this, (float)fts_get_number_float(at));

  if( this->editor)
    tabeditor_send( (tabeditor_t *)this->editor);

  data_object_set_dirty( o);
}

static void
fvec_set_elements(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  if(ac > 1 && fts_is_number(at))
    {
      int size = fvec_get_size(this);
      int offset = fts_get_number_int(at);

      if(offset >= 0 && offset < size)
	{
	  fvec_set_with_onset_from_atoms(this, offset, ac - 1, at + 1);
	
	  if( this->editor)
	    tabeditor_insert_append( (tabeditor_t *)this->editor, offset, ac, at);
	
	  data_object_set_dirty( o);
	}
    }
}

static void
fvec_reverse(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float *ptr = fvec_get_ptr(this);
  int size = fvec_get_size(this); 
  int i, j;

  for(i=0, j=size-1; i<size/2; i++, j--)
    {
      float f = ptr[i];

      ptr[i] = ptr[j];
      ptr[j] = f;
    }

  data_object_set_dirty( o);
}

static void
fvec_rotate(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float *ptr = fvec_get_ptr(this);
  int size = fvec_get_size(this);

  if(size > 1)
    {
      int shift = 1;
      
      if(ac && fts_is_number(at))
	shift = fts_get_number_int(at);
      
      if(shift == 1)
	{
	  float f = ptr[size - 1];
	  int i;

	  for(i=size-2; i>=0; i--)
	    ptr[i + 1] = ptr[i];

	  ptr[0] = f;
	}
      else if(shift == -1)
	{
	  float f = ptr[0];
	  int i;

	  for(i=0; i<size-1; i++)
	    ptr[i] = ptr[i + 1];

	  ptr[size - 1] = f;
	}
      else
	{
	  float forward;
	  int i, j, next, end;

	  while(shift < 0)
	    shift += size;
	  
	  while(shift >= size)
	    shift -= size;
	  
	  i = 0;
	  j = shift;
	  end = shift; 

	  forward = ptr[shift];
	  ptr[shift] = ptr[0];
	  
	  while(i < end)
	    {
	      next = (j + shift) % size;
	      
	      if(next != i)
		{
		  float swap = ptr[next];

		  if(next < end)
		    end = next;

		  ptr[next] = forward;
		  forward = swap;

		  j = next;
		}
	      else
		{
		  ptr[i] = forward;

		  i++;
		  j = i;

		  forward = ptr[i];
		}
	    }
	}
      
      data_object_set_dirty( o);
    }
}

static int 
fvec_element_compare(const void *left, const void *right)
{
  float l = *((const float *)left);
  float r = *((const float *)right);

  return (int)(l - r);
}

static void
fvec_sort(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float *ptr = fvec_get_ptr(this);

  qsort((void *)ptr, fvec_get_size(this), sizeof(float), fvec_element_compare);

  data_object_set_dirty( o);
}

static void
fvec_scramble(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float *ptr = fvec_get_ptr(this);
  int size = fvec_get_size(this);
  double range = size;
  int i;

  for(i=0; i<size-1; i++)
    {
      int random = (int)(range * fts_random() / FTS_RANDOM_RANGE);
      float f = ptr[i];

      ptr[i] = ptr[i + random];
      ptr[i + random] = f;

      range -= 1.0;
    }

  data_object_set_dirty( o);
}

static void
fvec_normalize(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float *ptr = fvec_get_ptr(this);
  int size = fvec_get_size(this);
  float scale = 1.0;

  if(ac > 0 && fts_is_number(at))
    scale = (float)fts_get_number_float(at);
  
  if(size > 0)
    {
      float max = ptr[0];
      int i;
     
      /* find maximum */
      for(i=1; i<size; i++)
	{
	  float f = ptr[i];

	  if(f > max)
	    max = f;
	  else if(f < -max)
	    max = -f;
	}
      
      scale /= max;
      
      for(i=0; i<size; i++)
	ptr[i] *= scale;
 
      data_object_set_dirty( o);
   }
}

static void
fvec_return_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fts_atom_t a;

  fts_set_int(&a, fvec_get_size(this));
  fts_return(&a);
}

static void
fvec_change_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  int size = fts_get_number_int(at);

  if(size >= 0)
  {
    int old_size = this->m;
    int i;

    fvec_set_size(this, size);

    /* when extending: zero new values */
    for(i=old_size; i<size; i++)
      this->values[i] = 0.0;
 
    if( this->editor)
      {
	fts_client_send_message( this->editor, fts_s_size, ac, at);
	data_object_set_dirty( o);
      }
  }
}

/**************************************************************************************
 *
 *  arithmetics
 *
 */

static void
fvec_add_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  fvec_t *right = (fvec_t *)fts_get_object(at);
  int this_size = fvec_get_size(this);
  int right_size = fvec_get_size(right);
  int size = (this_size <= right_size)? this_size: right_size;
  float *l = fvec_get_ptr(this);
  float *r = fvec_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] += r[i];
}

static void
fvec_add_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  ivec_t *right = (ivec_t *)fts_get_object(at);
  int this_size = fvec_get_size(this);
  int right_size = ivec_get_size(right);
  int size = (this_size <= right_size)? this_size: right_size;
  float *l = fvec_get_ptr(this);
  int *r = ivec_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] += (float)r[i];
}

static void
fvec_add_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fvec_get_size(this);
  float *p = fvec_get_ptr(this);
  int i;

  for(i=0; i<size; i++)
    p[i] += r;
}

static void
fvec_sub_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fvec_t *right = (fvec_t *)fts_get_object(at);
  int this_size = fvec_get_size(this);
  int right_size = fvec_get_size(right);
  int size = (this_size <= right_size)? this_size: right_size;
  float *l = fvec_get_ptr(this);
  float *r = fvec_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] -= r[i];
}

static void
fvec_sub_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  ivec_t *right = (ivec_t *)fts_get_object(at);
  int this_size = fvec_get_size(this);
  int right_size = ivec_get_size(right);
  int size = (this_size <= right_size)? this_size: right_size;
  float *l = fvec_get_ptr(this);
  int *r = ivec_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] -= (float)r[i];
}

static void
fvec_sub_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  float r = (float)fts_get_number_float(at);
  int size = fvec_get_size(this);
  float *p = fvec_get_ptr(this);
  int i;

  for(i=0; i<size; i++)
    p[i] -= r;
}

static void
fvec_mul_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fvec_t *right = (fvec_t *)fts_get_object(at);
  int this_size = fvec_get_size(this);
  int right_size = fvec_get_size(right);
  int size = (this_size <= right_size)? this_size: right_size;
  float *l = fvec_get_ptr(this);
  float *r = fvec_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] *= r[i];
}

static void
fvec_mul_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  ivec_t *right = (ivec_t *)fts_get_object(at);
  int this_size = fvec_get_size(this);
  int right_size = ivec_get_size(right);
  int size = (this_size <= right_size)? this_size: right_size;
  float *l = fvec_get_ptr(this);
  int *r = ivec_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] *= (float)r[i];
}

static void
fvec_mul_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fvec_get_size(this);
  float *p = fvec_get_ptr(this);
  int i;

  for(i=0; i<size; i++)
    p[i] *= r;
}

static void
fvec_div_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fvec_t *right = (fvec_t *)fts_get_object(at);
  int this_size = fvec_get_size(this);
  int right_size = fvec_get_size(right);
  int size = (this_size <= right_size)? this_size: right_size;
  float *l = fvec_get_ptr(this);
  float *r = fvec_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
  {
    if(r[i] != 0.0)
      l[i] /= r[i];
    else
      l[i] = 0.0;
  }
}

static void
fvec_div_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  ivec_t *right = (ivec_t *)fts_get_object(at);
  int this_size = fvec_get_size(this);
  int right_size = ivec_get_size(right);
  int size = (this_size <= right_size)? this_size: right_size;
  float *l = fvec_get_ptr(this);
  int *r = ivec_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
  {
    if(r[i] != 0)
      l[i] /= (float)r[i];
    else
      l[i] = 0.0;
  }
}

static void
fvec_div_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fvec_get_size(this);
  float *p = fvec_get_ptr(this);
  int i;

  for(i=0; i<size; i++)
    p[i] /= r;
}

static void
fvec_bus_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fvec_t *right = (fvec_t *)fts_get_object(at);
  int this_size = fvec_get_size(this);
  int right_size = fvec_get_size(right);
  int size = (this_size <= right_size)? this_size: right_size;
  float *l = fvec_get_ptr(this);
  float *r = fvec_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = r[i] - l[i];
}

static void
fvec_bus_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  ivec_t *right = (ivec_t *)fts_get_object(at);
  int this_size = fvec_get_size(this);
  int right_size = ivec_get_size(right);
  int size = (this_size <= right_size)? this_size: right_size;
  float *l = fvec_get_ptr(this);
  int *r = ivec_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = (float)r[i] - l[i];
}

static void
fvec_bus_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fvec_get_size(this);
  float *p = fvec_get_ptr(this);
  int i;

  for(i=0; i<size; i++)
    p[i] = r - p[i];
}

static void
fvec_vid_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fvec_t *right = (fvec_t *)fts_get_object(at);
  int this_size = fvec_get_size(this);
  int right_size = fvec_get_size(right);
  int size = (this_size <= right_size)? this_size: right_size;
  float *l = fvec_get_ptr(this);
  float *r = fvec_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
  {
    if(l[i] != 0.0)
      l[i] = r[i] / l[i];
    else
      l[i] = 0.0;
  }
}

static void
fvec_vid_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  ivec_t *right = (ivec_t *)fts_get_object(at);
  int this_size = fvec_get_size(this);
  int right_size = ivec_get_size(right);
  int size = (this_size <= right_size)? this_size: right_size;
  float *l = fvec_get_ptr(this);
  int *r = ivec_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
  {
    if(l[i] != 0.0)
      l[i] = (float)r[i] / l[i];
    else
      l[i] = 0.0;
  }
}

static void
fvec_vid_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fvec_get_size(this);
  float *p = fvec_get_ptr(this);

  if(r != 0.0)
  {
    int i;

    for(i=0; i<size; i++)
      p[i] = r / p[i];
  }
  else
  {
    int i;

    for(i=0; i<size; i++)
      p[i] = 0.0;
  }
}

/**************************************************************************************
 *
 *  comparison
 *
 */

static void
fvec_ee_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fvec_t *right = (fvec_t *)fts_get_object(at);
  int this_size = fvec_get_size(this);
  int right_size = fvec_get_size(right);
  int size = (this_size <= right_size)? this_size: right_size;
  float *l = fvec_get_ptr(this);
  float *r = fvec_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = (float)(l[i] == r[i]);
}

static void
fvec_ee_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fvec_get_size(this);
  float *p = fvec_get_ptr(this);
  int i;

  for(i=0; i<size; i++)
    p[i] = (float)(p[i] == r);
}

static void
fvec_ne_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fvec_t *right = (fvec_t *)fts_get_object(at);
  int this_size = fvec_get_size(this);
  int right_size = fvec_get_size(right);
  int size = (this_size <= right_size)? this_size: right_size;
  float *l = fvec_get_ptr(this);
  float *r = fvec_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = (float)(l[i] != r[i]);
}

static void
fvec_ne_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fvec_get_size(this);
  float *p = fvec_get_ptr(this);
  int i;

  for(i=0; i<size; i++)
    p[i] = (float)(p[i] != r);
}

static void
fvec_gt_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fvec_t *right = (fvec_t *)fts_get_object(at);
  int this_size = fvec_get_size(this);
  int right_size = fvec_get_size(right);
  int size = (this_size <= right_size)? this_size: right_size;
  float *l = fvec_get_ptr(this);
  float *r = fvec_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = (float)(l[i] > r[i]);
}

static void
fvec_gt_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fvec_get_size(this);
  float *p = fvec_get_ptr(this);
  int i;

  for(i=0; i<size; i++)
    p[i] = (float)(p[i] > r);
}

static void
fvec_ge_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fvec_t *right = (fvec_t *)fts_get_object(at);
  int this_size = fvec_get_size(this);
  int right_size = fvec_get_size(right);
  int size = (this_size <= right_size)? this_size: right_size;
  float *l = fvec_get_ptr(this);
  float *r = fvec_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = (float)(l[i] >= r[i]);
}

static void
fvec_ge_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fvec_get_size(this);
  float *p = fvec_get_ptr(this);
  int i;

  for(i=0; i<size; i++)
    p[i] = (float)(p[i] >= r);
}

static void
fvec_lt_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fvec_t *right = (fvec_t *)fts_get_object(at);
  int this_size = fvec_get_size(this);
  int right_size = fvec_get_size(right);
  int size = (this_size <= right_size)? this_size: right_size;
  float *l = fvec_get_ptr(this);
  float *r = fvec_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = (float)(l[i] < r[i]);
}

static void
fvec_lt_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fvec_get_size(this);
  float *p = fvec_get_ptr(this);
  int i;

  for(i=0; i<size; i++)
    p[i] = (float)(p[i] < r);
}

static void
fvec_le_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fvec_t *right = (fvec_t *)fts_get_object(at);
  int this_size = fvec_get_size(this);
  int right_size = fvec_get_size(right);
  int size = (this_size <= right_size)? this_size: right_size;
  float *l = fvec_get_ptr(this);
  float *r = fvec_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = (float)(l[i] <= r[i]);
}

static void
fvec_le_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fvec_get_size(this);
  float *p = fvec_get_ptr(this);
  int i;

  for(i=0; i<size; i++)
    p[i] = (float)(p[i] <= r);
}

static void
fvec_min_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fvec_t *right = (fvec_t *)fts_get_object(at);
  int this_size = fvec_get_size(this);
  int right_size = fvec_get_size(right);
  int size = (this_size <= right_size)? this_size: right_size;
  float *l = fvec_get_ptr(this);
  float *r = fvec_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = (r[i] <= l[i])? r[i]: l[i];
}

static void
fvec_min_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fvec_get_size(this);
  float *p = fvec_get_ptr(this);
  int i;

  for(i=0; i<size; i++)
    p[i] = (r <= p[i])? r: p[i];
}

static void
fvec_max_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fvec_t *right = (fvec_t *)fts_get_object(at);
  int this_size = fvec_get_size(this);
  int right_size = fvec_get_size(right);
  int size = (this_size <= right_size)? this_size: right_size;
  float *l = fvec_get_ptr(this);
  float *r = fvec_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = (r[i] >= l[i])? r[i]: l[i];
}

static void
fvec_max_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fvec_get_size(this);
  float *p = fvec_get_ptr(this);
  int i;

  for(i=0; i<size; i++)
    p[i] = (r >= p[i])? r: p[i];
}

static void
fvec_abs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  int size = fvec_get_size(this);
  float *p = fvec_get_ptr(this);
  int i;

  for(i=0; i<size; i++)
  {
    float f = p[i];

    p[i] = (f > 0.0)? f: -f;
  }
}

static void
fvec_abs_cvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float *ptr = fvec_get_ptr(this);
  cvec_t *in = (cvec_t *)fts_get_object(at);
  complex *in_ptr = cvec_get_ptr(in);
  int in_size = cvec_get_size(in);
  int i;

  fvec_set_size(this, in_size);

  for(i=0; i<in_size; i++)
    ptr[i] = (float)sqrt(in_ptr[i].re * in_ptr[i].im);
}

static void
fvec_log(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  int size = fvec_get_size(this);
  float *p = fvec_get_ptr(this);
  int i;

  for(i=0; i<size; i++)
  {
    double f = p[i];

    p[i] = (float)log(f);
  }
}

static void
fvec_exp(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  int size = fvec_get_size(this);
  float *p = fvec_get_ptr(this);
  int i;

  for(i=0; i<size; i++)
  {
    double f = p[i];

    p[i] = (float)exp(f);
  }
}

static void
fvec_ifft_cvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  cvec_t *in = (cvec_t *)fts_get_object(at);
  float *in_ptr = (float *)cvec_get_ptr(in);
  int in_size = cvec_get_size(in);
  unsigned int fft_size = fts_get_fft_size(2 * in_size);
  float *fft_ptr;
  int i;

  fvec_set_size(this, fft_size);
  fft_ptr = fvec_get_ptr(this);

  for(i=0; i<2*in_size; i++)
    fft_ptr[i] = in_ptr[i];

  /* zero padding */
  for(; i< (int)fft_size; i++)
    fft_ptr[i] = 0.0;

  fts_rifft_inplc(fft_ptr, fft_size);
}

static void
fvec_pick_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fvec_t *source = (fvec_t *)fts_get_object(at);
  int source_size = fvec_get_size(source);
  int size = fvec_get_size(this);
  float *src = fvec_get_ptr(source);
  float *ptr = fvec_get_ptr(this);
  int onset = 0;
  int i;

  if(ac > 1 && fts_is_number(at + 1))
    onset = fts_get_number_int(at + 1);

  if(ac > 2 && fts_is_number(at + 2))
    size = fts_get_number_int(at + 2);

  if(onset + size > source_size)
    size = source_size - onset;

  if(size > 0)
  {
    fvec_set_size(this, size);

    for(i=0; i<size; i++)
      ptr[i] = src[onset + i];
  }
}

static void
fvec_fade(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  int size = fvec_get_size(this);
  float *ptr = fvec_get_ptr(this);
  int fade = 0;
  float f, incr;
  int n, i;

  if(ac > 0 && fts_is_number(at))
    fade = fts_get_number_int(at);

  n = fade;
  incr = (float)(1.0 / (float)fade);

  if(n > size / 2)
    n = size / 2;
	  
  f = 0.0;
  for(i=0; i<n; i++)
    {
      ptr[i] *= f;
      ptr[size - 1 - i] *= f;
      f += incr;
    }
}




/******************************************************************************
 *
 * functions, i.e. methods that return a value but don't change the object
 *
 * todo: to be called in functional syntax, e.g. (.max $myfvec)
 */

static void
fvec_getmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
    const fvec_t *this = (fvec_t *) o;
    const int	  size = fvec_get_size(this);
    const float  *p    = fvec_get_ptr(this);
    float	  max  = p[0];	/* start with first element */
    int		  i;

    if (size == 0)
	return;			/* no output (void) for empty vector */

    for (i = 1; i < size; i++)
	if (p[i] > max)
	    max = p[i];

    fts_return_float(max);
}


static void
fvec_getmaxindex(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
    const fvec_t *this = (fvec_t *) o;
    const int	  size = fvec_get_size(this);
    const float  *p    = fvec_get_ptr(this);
    float	  max  = p[0];	/* start with first element */
    int		  maxi = 0;
    int		  i;

    if (size == 0)
	return;			/* no output (void) for empty vector */

    for (i = 1; i < size; i++)
	if (p[i] > max)
	{
	    max  = p[i];
	    maxi = i;
	}

    fts_return_int(maxi);
}


static void
fvec_getmin(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
    const fvec_t *this = (fvec_t *) o;
    const int	  size = fvec_get_size(this);
    const float  *p    = fvec_get_ptr(this);
    float	  min  = p[0];	/* start with first element */
    int		  i;

    if (size == 0)
	return;			/* no output (void) for empty vector */

    for (i = 1; i < size; i++)
	if (p[i] < min)
	    min = p[i];

    fts_return_float(min);
}


static void
fvec_getminindex(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
    const fvec_t *this = (fvec_t *) o;
    const int	  size = fvec_get_size(this);
    const float  *p    = fvec_get_ptr(this);
    float	  min  = p[0];	/* start with first element */
    int		  mini = 0;
    int		  i;

    if (size == 0)
	return;			/* no output (void) for empty vector */

    for (i = 1; i < size; i++)
	if (p[i] < min)
	{
	    min  = p[i];
	    mini = i;
	}

    fts_return_int(mini);
}




/*******************************************************************************
 *
 *  load, save, import, export
 *
 */

static void
fvec_import(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_format = fts_get_symbol_arg(ac, at, 1, sym_text);
  int size = 0;

  if(!file_name)
    return;

  if(file_format == sym_text)
    {
      size = fvec_read_atom_file(this, file_name);
      
      if(size <= 0)
	fts_object_error(o, "cannot import from text file \"%s\"", fts_symbol_name(file_name));
      else
	data_object_set_dirty( o);
    }
  else
    fts_object_error(o, "unknown import file format \"%s\"", fts_symbol_name(file_format));
}

static void
fvec_export(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_format = fts_get_symbol_arg(ac, at, 1, sym_text);
  int size = 0;

  if(!file_name)
    return;

  if(file_format == sym_text)
    {
      size = fvec_write_atom_file(this, file_name);
      
      if(size < 0)
	fts_object_error(o, "cannot export to text file \"%s\"", fts_symbol_name(file_name));
    }
  else
    fts_object_error(o, "export file format \"%s\"", fts_symbol_name(file_format));
}

static void
fvec_load(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  if(ac > 0 && fts_is_symbol(at))
    {
      fts_symbol_t file_name = fts_get_symbol(at);
      int size = 0;
      int onset, n_read;

      if(ac > 1 && fts_is_number(at + 1))
	onset = fts_get_number_int(at + 1);
      else
	onset = 0;

      if(ac > 2 && fts_is_number(at + 2))
	n_read = fts_get_number_int(at + 2);
      else
	n_read = 0;

      if (fts_atomfile_check( file_name))
        size = fvec_read_atom_file(this, file_name);
      else
        size = fvec_load_audiofile(this, file_name, onset, n_read);
    
      if( this->editor)
	tabeditor_send( (tabeditor_t *)this->editor);

      data_object_set_dirty( o);
    }
  else
	fts_object_open_dialog(o, fts_s_load, sym_open_file);
}

static void
fvec_save_soundfile(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
    
  if(file_name)
    {
      int n_write = fts_get_int_arg(ac, at, 1, 0);
      int vec_size = fvec_get_size(this);
      fts_audiofile_t *sf = 0;
      int size = 0;
      
      sf = fts_audiofile_open_write(file_name, 1, (int)fts_dsp_get_sample_rate(), fts_s_int16);
    
      if( fts_audiofile_is_valid(sf))
	{
	  float *ptr = fvec_get_ptr(this);
    
	  if(n_write <= 0 || n_write > vec_size)
	    n_write = vec_size;
    
	  size = fts_audiofile_write(sf, &ptr, 1, n_write);
    
	  fts_audiofile_close(sf);
    
	  if(size <= 0)
	    fts_object_error(o, "cannot save to soundfile \"%s\"", fts_symbol_name(file_name));
	}
      else
	{
	  fts_object_error(o, "cannot open soundfile to write \"%s\"", fts_symbol_name(file_name));
	  fts_audiofile_close(sf);
	}
    }
}

/********************************************************************
 *
 *  system functions
 *
 */

static void
fvec_post(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
  int size = fvec_get_size(this);

  if(size != 1 && size <= FTS_POST_MAX_ELEMENTS)
    {
      int i;
      
      fts_spost(stream, "<fvec");
      
      for(i=0; i<size; i++)
	fts_spost(stream, " %.7g", fvec_get_element(this, i));

      fts_spost(stream, ">");      
    }
  else
    fts_spost(stream, "<fvec %d>", size);
}

static void
fvec_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
  int size = fvec_get_size(this);

  if(size == 0)
    fts_spost(stream, "<empty fvec>\n");
  else if(size == 1)
    fts_spost(stream, "<fvec of 1 element: %.7g>\n", fvec_get_element(this, 0));
  else if(size <= FTS_POST_MAX_ELEMENTS)
    {
      int i;
      
      fts_spost(stream, "<fvec of %d elements: ", size);
      
      for(i=0; i<size-1; i++)
	fts_spost(stream, "%.7g ", fvec_get_element(this, i));
      
      fts_spost(stream, "%.7g>\n", fvec_get_element(this, i));
    }
  else
    {
      int i, j;
      
      fts_spost(stream, "<fvec of %d elements>\n", size);
      fts_spost(stream, "{\n");
      
      for(i=0; i<size; i+=FTS_POST_MAX_ELEMENTS)
	{
	  int n = size - i;

	  if(n > FTS_POST_MAX_ELEMENTS)
	    n = FTS_POST_MAX_ELEMENTS;
	  
	  fts_spost(stream, "  ");
	  
	  for(j=0; j<n; j++)
	    fts_spost(stream, "%.7g ", fvec_get_element(this, i + j));
	  
	  fts_spost(stream, "\n");
	}
      
      fts_spost(stream, "}\n");
    }
}

static void
fvec_get_tuple(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float *values = fvec_get_ptr(this);
  int size = fvec_get_size(this);
  fts_tuple_t *tuple = (fts_tuple_t *)fts_object_create(fts_tuple_class, 0, 0);
  fts_atom_t *atoms;
  int i;

  fts_tuple_set_size(tuple, size);
  atoms = fts_tuple_get_atoms(tuple);
  
  for(i=0; i<size; i++)
    fts_set_float(atoms + i, values[i]);

  fts_return_object((fts_object_t *)tuple);
}

static void
fvec_set_from_instance(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fvec_t *in = (fvec_t *)fts_get_object(at);
  
  fvec_copy(in, this);

  data_object_set_dirty( o);
}

static void
fvec_dump_state(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);      
  int size = fvec_get_size(this);
  fts_message_t *mess;
  int i;
  
  /* send size message */
  mess = fts_dumper_message_new(dumper, fts_s_size);  
  fts_message_append_int(mess, size);
  fts_dumper_message_send(dumper, mess);
  
  /* get new set message and append onset 0 */
  mess = fts_dumper_message_new(dumper, fts_s_set);
  fts_message_append_int(mess, 0);
  
  for(i=0; i<size; i++)
    {
      fts_message_append_float(mess, this->values[i]);
      
      if(fts_message_get_ac(mess) >= 256)
	{
	  fts_dumper_message_send(dumper, mess);
	  
	  /* new set message and append onset i + 1 */
	  mess = fts_dumper_message_new(dumper, fts_s_set);
	  fts_message_append_int(mess, i + 1);
	}
    }
  
  if(fts_message_get_ac(mess) > 1) 
    fts_dumper_message_send(dumper, mess);
}

static void 
fvec_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(data_object_is_persistent(o))
    {
      fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
      fts_atom_t a;

      fvec_dump_state(o, 0, 0, ac, at);

      /* save persistence flag */
      fts_set_int(&a, 1);
      fts_dumper_send(dumper, fts_s_persistence, 1, &a);      
    }

  fts_name_dump_method(o, 0, 0, ac, at);
}

static void
fvec_assign_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  fvec_copy((fvec_t *)fts_get_object(at), this);
}

static void
fvec_assign_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  ivec_t *ivec = (ivec_t *)fts_get_object(at);
  int *p = ivec_get_ptr(ivec);
  int size = ivec_get_size(ivec);
  int i;

  fvec_set_size(this, size);

  for(i=0; i<size; i++)
    this->values[i] = (float)p[i];
}

static void
fvec_assign_cvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  cvec_t *cvec = (cvec_t *)fts_get_object(at);
  complex *c = cvec_get_ptr(cvec);
  int size = cvec_get_size(cvec);
  int i;

  fvec_set_size(this, size);

  for(i=0; i<size; i++)
    this->values[i] = c[i].re;
}

static void
fvec_return_element(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  int index = fts_get_int_arg(ac, at, 0, -1);

  if (index >= 0 && index < fvec_get_size(this))
    fts_return_float(fvec_get_element(this, index));
}

static void
fvec_return_interpolated(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  int size = fvec_get_size(this);
  double f = fts_get_number_float(at);
  double ret = 0.0;

  if(size < 2 || f <= 0.0)
    ret = fvec_get_element(this, 0);
  else if(f >= size - 1)
    ret = fvec_get_element(this, size - 1);
  else
  {
    int i = (int)f;
    double y0 = fvec_get_element(this, i);
    double y1 = fvec_get_element(this, i + 1);

    ret = y0 + (f - i) * (y1 - y0);
  }

  fts_return_float((float)ret);
}

static int
fvec_equals(const fts_atom_t *a, const fts_atom_t *b)
{
  fvec_t *o = (fvec_t *)fts_get_object(a);
  fvec_t *p = (fvec_t *)fts_get_object(b);
  int o_n = fvec_get_size(o);
  int p_n = fvec_get_size(p);

  if(o_n == p_n)
  {
    int i;

    for(i=0; i<o_n; i++)
      if(!data_float_equals(fvec_get_element(o, i), fvec_get_element(p, i)))
        return 0;

    return 1;
  }

  return 0;
}

/*********************************************************
 *
 *  class
 *
 */
static void
fvec_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  data_object_init(o);
  
  this->values = NULL;
  this->m = 0;
  this->n = 1;
  this->alloc = FVEC_NO_ALLOC;

  this->editor = 0;

  if(ac == 0)
    fvec_set_size(this, 0);
  else if(ac == 1 && fts_is_int(at))
    {
      fvec_set_size(this, fts_get_int(at));
      fvec_set_const(this, 0.0);
    }
  else if(ac == 1 && fts_is_tuple(at))
    {
      fts_tuple_t *tup = (fts_tuple_t *)fts_get_object(at);
      int size = fts_tuple_get_size(tup);
      
      fvec_set_size(this, size);
      fvec_set_with_onset_from_atoms(this, 0, size, fts_tuple_get_atoms(tup));

      data_object_persistence_args(o);
    }
  else if(ac == 1 && fts_is_symbol(at))
    {
      fts_symbol_t file_name = fts_get_symbol(at);
      int size = 0;
      
      if (fts_atomfile_check( file_name))
        size = fvec_read_atom_file(this, file_name);
      else
        size = fvec_load_audiofile(this, file_name, 0, 0);

      if(size == 0)
	fts_object_error(o, "cannot load fvec from file \"%s\"", fts_symbol_name(file_name));

      data_object_persistence_args(o);
    }
  else if(ac > 1)
    {
      fvec_set_size(this, ac);
      fvec_set_with_onset_from_atoms(this, 0, ac, at);

      data_object_persistence_args(o);
    }
  else
    fts_object_error(o, "bad arguments for fvec constructor");
}

static void
fvec_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  if(this->editor) 
    {  
      if(fts_object_has_id( this->editor))
	fts_client_send_message( (fts_object_t *)this->editor, fts_s_destroyEditor, 0, 0);
      
      fts_object_destroy((fts_object_t *)this->editor);
    }  

  if(this->values != NULL)
    fts_free(this->values - FTS_CUBIC_HEAD);
}

static void
fvec_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fvec_t), fvec_init, fvec_delete);
  
  fts_class_message_varargs(cl, fts_s_name, fts_name_set_method);
  fts_class_message_varargs(cl, fts_s_persistence, data_object_persistence);
  fts_class_message_varargs(cl, fts_s_update_gui, data_object_update_gui); 
  fts_class_message_varargs(cl, fts_s_dump_state, fvec_dump_state);
  fts_class_message_varargs(cl, fts_s_dump, fvec_dump);

  /* graphical editor */
  fts_class_message_varargs(cl, fts_s_openEditor, fvec_open_editor);
  fts_class_message_varargs(cl, fts_s_closeEditor, fvec_close_editor);
  fts_class_message_varargs(cl, fts_s_destroyEditor, fvec_destroy_editor);

  fts_class_message_varargs(cl, fts_s_post, fvec_post); 
  fts_class_message_varargs(cl, fts_s_print, fvec_print); 

  fts_class_set_equals_function(cl, fvec_equals);
  
  fts_class_message_varargs(cl, fts_s_set_from_instance, fvec_set_from_instance);

  fts_class_message_varargs(cl, fts_s_get_tuple, fvec_get_tuple);

  fts_class_message_number(cl, fts_s_fill, fvec_fill);
  fts_class_message_varargs(cl, fts_s_set, fvec_set_elements);

  fts_class_message(cl, fts_s_set, cl, fvec_assign_fvec);
  fts_class_message(cl, fts_s_set, ivec_type, fvec_assign_ivec);
  fts_class_message(cl, fts_s_set, cvec_type, fvec_assign_cvec);
  
  fts_class_message_varargs(cl, fts_new_symbol("reverse"), fvec_reverse);
  fts_class_message_varargs(cl, fts_new_symbol("rotate"), fvec_rotate);
  fts_class_message_varargs(cl, fts_new_symbol("sort"), fvec_sort);
  fts_class_message_varargs(cl, fts_new_symbol("scramble"), fvec_scramble);
  fts_class_message_varargs(cl, fts_new_symbol("normalize"), fvec_normalize);

  fts_class_message(cl, fts_new_symbol("add"), cl, fvec_add_fvec);
  fts_class_message(cl, fts_new_symbol("add"), ivec_type, fvec_add_ivec);
  fts_class_message_number(cl, fts_new_symbol("add"), fvec_add_number);
  
  fts_class_message(cl, fts_new_symbol("sub"), cl, fvec_sub_fvec);
  fts_class_message(cl, fts_new_symbol("sub"), ivec_type, fvec_sub_ivec);
  fts_class_message_number(cl, fts_new_symbol("sub"), fvec_sub_number);
  
  fts_class_message(cl, fts_new_symbol("mul"), cl, fvec_mul_fvec);
  fts_class_message(cl, fts_new_symbol("mul"), ivec_type, fvec_mul_ivec);
  fts_class_message_number(cl, fts_new_symbol("mul"), fvec_mul_number);
  
  fts_class_message(cl, fts_new_symbol("div"), cl, fvec_div_fvec);
  fts_class_message(cl, fts_new_symbol("div"), ivec_type, fvec_div_ivec);
  fts_class_message_number(cl, fts_new_symbol("div"), fvec_div_number);
  
  fts_class_message(cl, fts_new_symbol("bus"), cl, fvec_bus_fvec);
  fts_class_message(cl, fts_new_symbol("bus"), ivec_type, fvec_bus_ivec);
  fts_class_message_number(cl, fts_new_symbol("bus"), fvec_bus_number);
  
  fts_class_message(cl, fts_new_symbol("vid"), cl, fvec_vid_fvec);
  fts_class_message(cl, fts_new_symbol("vid"), ivec_type, fvec_vid_ivec);
  fts_class_message_number(cl, fts_new_symbol("vid"), fvec_vid_number);
  
  fts_class_message(cl, fts_new_symbol("ee"), cl, fvec_ee_fvec);
  fts_class_message_number(cl, fts_new_symbol("ee"), fvec_ee_number);
  
  fts_class_message(cl, fts_new_symbol("ne"), cl, fvec_ne_fvec);
  fts_class_message_number(cl, fts_new_symbol("ne"), fvec_ne_number);
  
  fts_class_message(cl, fts_new_symbol("gt"), cl, fvec_gt_fvec);
  fts_class_message_number(cl, fts_new_symbol("gt"), fvec_gt_number);
  
  fts_class_message(cl, fts_new_symbol("ge"), cl, fvec_ge_fvec);
  fts_class_message_number(cl, fts_new_symbol("ge"), fvec_ge_number);
  
  fts_class_message(cl, fts_new_symbol("lt"), cl, fvec_lt_fvec);
  fts_class_message_number(cl, fts_new_symbol("lt"), fvec_lt_number);
  
  fts_class_message(cl, fts_new_symbol("le"), cl, fvec_le_fvec);
  fts_class_message_number(cl, fts_new_symbol("le"), fvec_le_number);
  
  fts_class_message(cl, fts_new_symbol("min"), cl, fvec_min_fvec);
  fts_class_message_number(cl, fts_new_symbol("min"), fvec_min_number);
  
  fts_class_message(cl, fts_new_symbol("max"), cl, fvec_max_fvec);
  fts_class_message_number(cl, fts_new_symbol("max"), fvec_max_number);

  fts_class_message_void(cl, fts_new_symbol("getmax"),      fvec_getmax);
  fts_class_message_void(cl, fts_new_symbol("getmaxindex"), fvec_getmaxindex);
  fts_class_message_void(cl, fts_new_symbol("getmin"),      fvec_getmin);
  fts_class_message_void(cl, fts_new_symbol("getminindex"), fvec_getminindex);

  fts_class_message_void(cl, fts_new_symbol("abs"), fvec_abs);
  fts_class_message(cl, fts_new_symbol("abs"), cvec_type, fvec_abs);

  fts_class_message_void(cl, fts_new_symbol("log"), fvec_log);
  fts_class_message_void(cl, fts_new_symbol("exp"), fvec_exp);

  fts_class_message(cl, fts_new_symbol("ifft"), cvec_type, fvec_ifft_cvec);

  fts_class_message(cl, fts_new_symbol("pick"), cl, fvec_pick_fvec);
  fts_class_message_number(cl, fts_new_symbol("fade"), fvec_fade);

  fts_class_message_varargs(cl, fts_s_import, fvec_import);
  fts_class_message_varargs(cl, fts_s_export, fvec_export);
  
  fts_class_message_varargs(cl, fts_s_load, fvec_load);
  fts_class_message_varargs(cl, fts_s_save, fvec_save_soundfile);

  fts_class_message_void(cl, fts_s_size, fvec_return_size);
  fts_class_message_number(cl, fts_s_size, fvec_change_size);
  
  fts_class_message_int(cl, fts_s_get_element, fvec_return_element);
  fts_class_message_float(cl, fts_s_get_element, fvec_return_interpolated);

  fts_class_inlet_bang(cl, 0, data_object_output);

  fts_class_inlet_thru(cl, 0);
  fts_class_outlet_thru(cl, 0);
}

/********************************************************************
 *
 *  config
 *
 */

void 
fvec_config(void)
{
  sym_text = fts_new_symbol("text");
  sym_open_file = fts_new_symbol("open file");

  fvec_symbol = fts_new_symbol("fvec");

  fvec_type = fts_class_install(fvec_symbol, fvec_instantiate);
}
