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
 */

#include <fts/fts.h>
#include <utils.h>
#include "fvec.h"
#include "cvec.h"

#include <stdlib.h>
#include <ctype.h>
#include <float.h>

#define FVEC_NO_ALLOC -1

fts_symbol_t fvec_symbol = 0;
fts_metaclass_t *fvec_type = 0;

static fts_symbol_t sym_text = 0;
static fts_symbol_t sym_open_file = 0;
static fts_symbol_t sym_local = 0;

/* add something to beginning and and of float vector */
/* for four-point interpolation */
#define FVEC_MONSET 1
#define FVEC_MOVERHEAD 4

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
  int i;

  if(size < 0)
    size = 0;

  if(size > alloc)
    {
      if(vec->values != NULL)
	values = (float *)fts_realloc((vec->values - FVEC_MONSET), sizeof(float) * (size + FVEC_MOVERHEAD)) + FVEC_MONSET;
      else
	values = (float *)fts_malloc(sizeof(float) * (size + FVEC_MOVERHEAD)) + FVEC_MONSET;	

      values[-1] = 0.0;
      values[size] = 0.0;
      values[size + 1] = 0.0;
      values[size + 2] = 0.0;

      vec->values = values;
      vec->alloc = size;
    }

  /* when shortening: zero old values */
  for(i=size; i<vec->size; i++)
    vec->values[i] = 0.0;

  vec->size = size;
}

void
fvec_set_const(fvec_t *vec, float c)
{
  float *values = vec->values;
  int i;
  
  for(i=0; i<vec->size; i++)
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
	vec->values[i + offset] = fts_get_number_float(at + i);
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

  for(i=0; i<vec->size; i++)
    sum += vec->values[i];

  return sum;
}

static float
fvec_get_min_value(fvec_t *vec)
{
  float min;
  int i;

  min = vec->values[0];

  for (i=1; i<vec->size; i++)
    if (vec->values[i] < min)
      min = vec->values[i];

  return min;
}


static float
fvec_get_max_value(fvec_t *vec)
{
  float max;
  int i;

  max = vec->values[0];

  for (i=1; i<vec->size; i++)
    if (vec->values[i] > max)
      max = vec->values[i];

  return max;
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
	    fvec_set_element(vec, n, fts_get_number_float(&a));
	  else
	    fvec_set_element(vec, n, 0.0f);
	  
	  n++;
	}
      
      if(n > 0)
	fvec_set_size(vec, n);
      else
	fts_object_signal_runtime_error((fts_object_t *)vec, "cannot load from file \"%s\"\n", file_name);
      
      fts_atom_file_close(file);
    }
  else
    fts_object_signal_runtime_error((fts_object_t *)vec, "cannot open file \"%s\"\n", file_name);
  
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
fvec_file_is_text( fts_symbol_t file_name)
{
  char full_path[1024];
  int n, i;
  char buff[256];
  FILE* fd;

  if (fts_file_find(file_name, full_path, 256) == NULL)
     return 0;

  if ( (fd = fopen( full_path, "rb")) == NULL)
    return 0;

  if ( (n = fread( buff, 1, 256, fd)) < 256)
    {
      fclose( fd);
      return 0;
    }

  for ( i = 0; i < n; i++)
    {
      if ( !isgraph(buff[i]) && !isspace(buff[i]))
	return 0;
    }

  fclose( fd);

  return 1;
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
	  fts_object_signal_runtime_error((fts_object_t *)vec, "cannot seek position in file \"%s\"\n", file_name);
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
	  fts_object_signal_runtime_error((fts_object_t *)vec, "cannot load from soundfile \"%s\"\n", file_name);
	  size = 0;
	}
    }
  else
    {
      fts_object_signal_runtime_error((fts_object_t *)vec, "cannot open file \"%s\"\n", file_name);
      fts_audiofile_close(sf);
    }

  return size;
}

/********************************************************************
 *
 *   user methods
 *
 */

static void
fvec_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_object(o, 0, o);
}

static void
fvec_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  fvec_set_const(this, 0.0f);
}

static void
fvec_fill(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  if(ac > 0 && fts_is_number(at))
    fvec_set_const(this, fts_get_number_float(at));
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
	fvec_set_with_onset_from_atoms(this, offset, ac - 1, at + 1);
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
    }
}

static int 
fvec_element_compare(const void *left, const void *right)
{
  float l = *((const float *)left);
  float r = *((const float *)right);

  return l - r;
}

static void
fvec_sort(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float *ptr = fvec_get_ptr(this);

  qsort((void *)ptr, fvec_get_size(this), sizeof(float), fvec_element_compare);
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
}

static void
fvec_normalize(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float *ptr = fvec_get_ptr(this);
  int size = fvec_get_size(this);
  float scale = 1.0;

  if(ac > 0 && fts_is_number(at))
    scale = fts_get_number_float(at);
  
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
    }
}

static void
fvec_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  if(ac > 0 && fts_is_number(at))
    {
      int size = fts_get_number_int(at);
      
      if(size >= 0)
	{
	  int old_size = this->size;
	  int i;

	  fvec_set_size(this, size);

	  /* when extending: zero new values */
	  for(i=old_size; i<size; i++)
	    this->values[i] = 0.0;	  
	}
    }
}

/**************************************************************************************
 *
 *  arithmetics
 *
 */

static void
fvec_add(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fvec_type))
	{
	  fvec_t *right = fvec_atom_get(at);
	  int this_size = fvec_get_size(this);
	  int right_size = fvec_get_size(right);
	  int size = (this_size <= right_size)? this_size: right_size;
	  float *l = fvec_get_ptr(this);
	  float *r = fvec_get_ptr(right);
	  int i;
    
	  for(i=0; i<size; i++)
	    l[i] += r[i];
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fvec_get_size(this);
	  float *p = fvec_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    p[i] += r;
	}
      else
	fts_object_signal_runtime_error(o, "method not implemented for given arguments");
    }
}

static void
fvec_sub(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fvec_type))
	{
	  fvec_t *right = fvec_atom_get(at);
	  int this_size = fvec_get_size(this);
	  int right_size = fvec_get_size(right);
	  int size = (this_size <= right_size)? this_size: right_size;
	  float *l = fvec_get_ptr(this);
	  float *r = fvec_get_ptr(right);
	  int i;
    
	  for(i=0; i<size; i++)
	    l[i] -= r[i];
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fvec_get_size(this);
	  float *p = fvec_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    p[i] -= r;
	}
      else
	fts_object_signal_runtime_error(o, "method not implemented for given arguments");
    }
}

static void
fvec_mul(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fvec_type))
	{
	  fvec_t *right = fvec_atom_get(at);
	  int this_size = fvec_get_size(this);
	  int right_size = fvec_get_size(right);
	  int size = (this_size <= right_size)? this_size: right_size;
	  float *l = fvec_get_ptr(this);
	  float *r = fvec_get_ptr(right);
	  int i;
    
	  for(i=0; i<size; i++)
	    l[i] *= r[i];
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fvec_get_size(this);
	  float *p = fvec_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    p[i] *= r;
	}
      else
	fts_object_signal_runtime_error(o, "method not implemented for given arguments");
    }
}

static void
fvec_div(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fvec_type))
	{
	  fvec_t *right = fvec_atom_get(at);
	  int this_size = fvec_get_size(this);
	  int right_size = fvec_get_size(right);
	  int size = (this_size <= right_size)? this_size: right_size;
	  float *l = fvec_get_ptr(this);
	  float *r = fvec_get_ptr(right);
	  int i;
    
	  for(i=0; i<size; i++)
	    {
	      if(r[i] != 0)
		l[i] /= r[i];
	      else
		l[i] = 0;
	    }
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fvec_get_size(this);
	  float *p = fvec_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    p[i] /= r;
	}
    }
}

static void
fvec_bus(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fvec_type))
	{
	  fvec_t *right = fvec_atom_get(at);
	  int this_size = fvec_get_size(this);
	  int right_size = fvec_get_size(right);
	  int size = (this_size <= right_size)? this_size: right_size;
	  float *l = fvec_get_ptr(this);
	  float *r = fvec_get_ptr(right);
	  int i;
    
	  for(i=0; i<size; i++)
	    l[i] = r[i] - l[i];
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fvec_get_size(this);
	  float *p = fvec_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    p[i] = r - p[i];
	}
      else
	fts_object_signal_runtime_error(o, "method not implemented for given arguments");
    }
}

static void
fvec_vid(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fvec_type))
	{
	  fvec_t *right = fvec_atom_get(at);
	  int this_size = fvec_get_size(this);
	  int right_size = fvec_get_size(right);
	  int size = (this_size <= right_size)? this_size: right_size;
	  float *l = fvec_get_ptr(this);
	  float *r = fvec_get_ptr(right);
	  int i;
    
	  for(i=0; i<size; i++)
	    {
	      if(l[i] != 0)
		l[i] = r[i] / l[i];
	      else
		l[i] = 0.0;
	    }
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
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
      else
	fts_object_signal_runtime_error(o, "method not implemented for given arguments");
    }
}

/**************************************************************************************
 *
 *  comparison
 *
 */

static void
fvec_ee(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fvec_type))
	{
	  fvec_t *right = fvec_atom_get(at);
	  int this_size = fvec_get_size(this);
	  int right_size = fvec_get_size(right);
	  int size = (this_size <= right_size)? this_size: right_size;
	  float *l = fvec_get_ptr(this);
	  float *r = fvec_get_ptr(right);
	  int i;
    
	  for(i=0; i<size; i++)
	    l[i] = l[i] == r[i];
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fvec_get_size(this);
	  float *p = fvec_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    p[i] = p[i] == r;
	}
      else
	fts_object_signal_runtime_error(o, "method not implemented for given arguments");
    }
}

static void
fvec_ne(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fvec_type))
	{
	  fvec_t *right = fvec_atom_get(at);
	  int this_size = fvec_get_size(this);
	  int right_size = fvec_get_size(right);
	  int size = (this_size <= right_size)? this_size: right_size;
	  float *l = fvec_get_ptr(this);
	  float *r = fvec_get_ptr(right);
	  int i;
    
	  for(i=0; i<size; i++)
	    l[i] = l[i] != r[i];
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fvec_get_size(this);
	  float *p = fvec_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    p[i] = p[i] != r;
	}
    }
}

static void
fvec_gt(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fvec_type))
	{
	  fvec_t *right = fvec_atom_get(at);
	  int this_size = fvec_get_size(this);
	  int right_size = fvec_get_size(right);
	  int size = (this_size <= right_size)? this_size: right_size;
	  float *l = fvec_get_ptr(this);
	  float *r = fvec_get_ptr(right);
	  int i;
    
	  for(i=0; i<size; i++)
	    l[i] = l[i] > r[i];
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fvec_get_size(this);
	  float *p = fvec_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    p[i] = p[i] > r;
	}
      else
	fts_object_signal_runtime_error(o, "method not implemented for given arguments");
    }
}

static void
fvec_ge(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fvec_type))
	{
	  fvec_t *right = fvec_atom_get(at);
	  int this_size = fvec_get_size(this);
	  int right_size = fvec_get_size(right);
	  int size = (this_size <= right_size)? this_size: right_size;
	  float *l = fvec_get_ptr(this);
	  float *r = fvec_get_ptr(right);
	  int i;
    
	  for(i=0; i<size; i++)
	    l[i] = l[i] >= r[i];
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fvec_get_size(this);
	  float *p = fvec_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    p[i] = p[i] >= r;
	}
      else
	fts_object_signal_runtime_error(o, "method not implemented for given arguments");
    }
}

static void
fvec_lt(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fvec_type))
	{
	  fvec_t *right = fvec_atom_get(at);
	  int this_size = fvec_get_size(this);
	  int right_size = fvec_get_size(right);
	  int size = (this_size <= right_size)? this_size: right_size;
	  float *l = fvec_get_ptr(this);
	  float *r = fvec_get_ptr(right);
	  int i;
    
	  for(i=0; i<size; i++)
	    l[i] = l[i] < r[i];
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fvec_get_size(this);
	  float *p = fvec_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    p[i] = p[i] < r;
	}
      else
	fts_object_signal_runtime_error(o, "method not implemented for given arguments");
    }
}

static void
fvec_le(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fvec_type))
	{
	  fvec_t *right = fvec_atom_get(at);
	  int this_size = fvec_get_size(this);
	  int right_size = fvec_get_size(right);
	  int size = (this_size <= right_size)? this_size: right_size;
	  float *l = fvec_get_ptr(this);
	  float *r = fvec_get_ptr(right);
	  int i;
    
	  for(i=0; i<size; i++)
	    l[i] = l[i] <= r[i];
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fvec_get_size(this);
	  float *p = fvec_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    p[i] = p[i] <= r;
	}
      else
	fts_object_signal_runtime_error(o, "method not implemented for given arguments");
    }
}

static void
fvec_min(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fvec_type))
	{
	  fvec_t *right = fvec_atom_get(at);
	  int this_size = fvec_get_size(this);
	  int right_size = fvec_get_size(right);
	  int size = (this_size <= right_size)? this_size: right_size;
	  float *l = fvec_get_ptr(this);
	  float *r = fvec_get_ptr(right);
	  int i;
    
	  for(i=0; i<size; i++)
	    l[i] = (r[i] <= l[i])? r[i]: l[i];
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fvec_get_size(this);
	  float *p = fvec_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    p[i] = (r <= p[i])? r: p[i];
	}
    }
}

static void
fvec_max(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fvec_type))
	{
	  fvec_t *right = fvec_atom_get(at);
	  int this_size = fvec_get_size(this);
	  int right_size = fvec_get_size(right);
	  int size = (this_size <= right_size)? this_size: right_size;
	  float *l = fvec_get_ptr(this);
	  float *r = fvec_get_ptr(right);
	  int i;
    
	  for(i=0; i<size; i++)
	    l[i] = (r[i] >= l[i])? r[i]: l[i];
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fvec_get_size(this);
	  float *p = fvec_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    p[i] = (r >= p[i])? r: p[i];
	}
      else
	fts_object_signal_runtime_error(o, "method not implemented for given arguments");
    }
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
fvec_ifft(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  if(ac > 0 && fts_is_a(at, cvec_type))
    {
      /* complex FFT */
      cvec_t *in = cvec_atom_get(at);
      int in_size = cvec_get_size(in);
      unsigned int fft_size = fts_get_fft_size(2 * in_size);
      float *fft_ptr;

      fvec_set_size(this, fft_size);
      fft_ptr = fvec_get_ptr(this);

      fts_rifft_inplc(fft_ptr, fft_size);
    }
  else
    fts_object_signal_runtime_error(o, "method not implemented for given arguments");
}

/**************************************************************************************
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
      
      if(size > 0)
	fvec_output(o, 0, 0, 0, 0);
      else
	fts_object_signal_runtime_error(o, "cannot import from text file \"%s\"\n", file_name);
    }
  else
    fts_object_signal_runtime_error(o, "unknown import file format \"%s\"\n", file_format);
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
	fts_object_signal_runtime_error(o, "cannot export to text file \"%s\"\n", file_name);
    }
  else
    fts_object_signal_runtime_error(o, "export file format \"%s\"\n", file_format);
}

static void
fvec_load(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  if(ac > 0 && fts_is_symbol(at))
    {
      fts_symbol_t file_name = fts_get_symbol(at);
      int size = 0;
      float sr = 0.0;
      int onset, n_read;

      if(ac > 1 && fts_is_number(at + 1))
	onset = fts_get_number_int(at + 1);
      else
	onset = 0;

      if(ac > 2 && fts_is_number(at + 2))
	n_read = fts_get_number_int(at + 2);
      else
	n_read = 0;

      if(ac > 3 && fts_is_number(at + 3))
	sr = fts_get_number_float(at + 3);
      else if(this->sr > 0.0)
	/* force sampling rate to given property */
	sr = this->sr;

      if (fvec_file_is_text( file_name))
	size = fvec_read_atom_file(this, file_name);
      else
	size = fvec_load_audiofile(this, file_name, onset, n_read);

      if(size > 0)
	fvec_output(o, 0, 0, 0, 0);
    }
  else
    {
      fts_atom_t a[4];
      
      fts_set_symbol(a, fts_s_load);
      fts_set_symbol(a + 1, sym_open_file);
      fts_set_symbol(a + 2, fts_project_get_dir());
      fts_set_symbol(a + 3, fts_new_symbol(" "));
      fts_client_send_message(o, fts_s_openFileDialog, 4, a);
    }
}

static void
fvec_save_soundfile(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
    
  if(file_name)
    {
      int n_write = fts_get_int_arg(ac, at, 1, 0);
      float sr = fts_get_float_arg(ac, at, 2, 0.0f);
      int vec_size = fvec_get_size(this);
      fts_audiofile_t *sf = 0;
      int size = 0;
      
      if(sr <= 0.0)
	sr = fts_dsp_get_sample_rate();
    
      sf = fts_audiofile_open_write(file_name, 1, sr, fts_s_int16);
    
      if( fts_audiofile_is_valid(sf))
	{
	  float *ptr = fvec_get_ptr(this);
    
	  if(n_write <= 0 || n_write > vec_size)
	    n_write = vec_size;
    
	  size = fts_audiofile_write(sf, &ptr, 1, n_write);
    
	  fts_audiofile_close(sf);
    
	  if(size <= 0)
	    fts_object_signal_runtime_error(o, "cannot save to soundfile \"%s\"\n", file_name);
	}
      else
	{
	  fts_object_signal_runtime_error(o, "cannot open soundfile to write \"%s\"\n", file_name);
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
fvec_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  int size = fvec_get_size(this);
  int i;

  post("{");

  if(size > 8)
    {
      int size8 = (size / 8) * 8;
      int i, j;

      for(i=0; i<size8; i+=8)
	{
	  /* print one line of 8 with indent */
	  post("\n  ");
	  for(j=0; j<8; j++)
	    post("%f ", fvec_get_element(this, i + j));
	}
	  
      /* print last line with indent */
      if(i < size)
	{
	  post("\n  ");
	  for(; i<size; i++)
	    post("%f ", fvec_get_element(this, i));
	}

      post("\n}\n");
    }
  else if(size)
    {
      for(i=0; i<size-1; i++)
	post("%f ", fvec_get_element(this, i));

      post("%f}\n", fvec_get_element(this, size - 1));
    }
  else
    post("}\n");
}

static void
fvec_get_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float *values = fvec_get_ptr(this);
  int size = fvec_get_size(this);
  fts_array_t *array = (fts_array_t *)fts_get_pointer(at);
  fts_atom_t *atoms;
  int i;
  
  fts_array_set_size(array, size);  
  atoms = fts_array_get_atoms(array);

  for(i=0; i<size; i++)
    fts_set_float(atoms + i, values[i]);
}

static void
fvec_set_from_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  fvec_set_size(this, ac);
  fvec_set_with_onset_from_atoms(this, 0, ac, at);
}

static void
fvec_set_from_instance(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fvec_t *in = fvec_atom_get(at);
  
  fvec_copy(in, this);
}

static void
fvec_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
fvec_assign(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fvec_type))
	fvec_copy(fvec_atom_get(at), this);
      else if(fts_is_a(at, cvec_type))
	{
	  cvec_t *cvec = cvec_atom_get(at);
	  complex *c = cvec_get_ptr(cvec);
	  int size = cvec_get_size(cvec);
	  int i;
	  
	  fvec_set_size(this, size);
	  
	  for(i=0; i<size; i++)
	    this->values[i] = c[i].re;
	}
    }
}

static void
fvec_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object(value, obj);
}

static void
fvec_set_sr(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fvec_t *this = (fvec_t *)obj;

  if(fts_is_number(value))
    {
      float sr = fts_get_number_float(value);
      
      if(sr > 0.0)
	this->sr = sr;
    }
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
  
  this->values = NULL;
  this->size = 0;
  this->alloc = FVEC_NO_ALLOC;
  data_object_set_keep((data_object_t *)o, fts_s_no);

  this->sr = 0.0;

  if(ac == 0)
    fvec_set_size(this, 0);
  else if(ac == 1 && fts_is_int(at))
    {
      fvec_set_size(this, fts_get_int(at));
      fvec_zero(this);
    }
  else if(ac == 1 && fts_is_tuple(at))
    {
      fts_tuple_t *tup = (fts_tuple_t *)fts_get_object(at);
      int size = fts_tuple_get_size(tup);
      
      fvec_set_size(this, size);
      fvec_set_with_onset_from_atoms(this, 0, size, fts_tuple_get_atoms(tup));
      data_object_set_keep((data_object_t *)o, fts_s_args);
    }
  else if(ac == 1 && fts_is_symbol(at))
    {
      fts_symbol_t file_name = fts_get_symbol(at);
      int size = 0;
      
      if (fvec_file_is_text( file_name))
	size = fvec_read_atom_file(this, file_name);
      else
	size = fvec_load_audiofile(this, file_name, 0, 0);

      if(size == 0)
	fts_object_set_error(o, "Cannot load fvec from file \"%s\"", file_name);

      data_object_set_keep((data_object_t *)o, fts_s_args);
    }
  else if(ac > 1)
    {
      fvec_set_size(this, ac);
      fvec_set_with_onset_from_atoms(this, 0, ac, at);

      data_object_set_keep((data_object_t *)o, fts_s_args);
    }
  else
    fts_object_set_error(o, "Wrong arguments for fvec constructor");
}

static void
fvec_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  if(this->values != NULL)
    fts_free(this->values - FVEC_MONSET);
}

static fts_status_t
fvec_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fvec_t), 1, 1, 0);
  
  /* init / delete */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fvec_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, fvec_delete);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_print, fvec_print); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, fvec_set_elements);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set_from_instance, fvec_set_from_instance);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set_from_array, fvec_set_from_array);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_size, fvec_size);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_get_array, fvec_get_array);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_dump, fvec_dump);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_save, fvec_save_soundfile); 
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_load, fvec_load);

  fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("sr"), fvec_set_sr);
  fts_class_add_daemon(cl, obj_property_put, fts_s_keep, data_object_daemon_set_keep);
  fts_class_add_daemon(cl, obj_property_get, fts_s_keep, data_object_daemon_get_keep);
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, fvec_get_state);
  
  fts_method_define_varargs(cl, 0, fts_s_bang, fvec_output);
  
  fts_method_define_varargs(cl, 0, fts_s_clear, fvec_clear);
  fts_method_define_varargs(cl, 0, fts_s_fill, fvec_fill);
  fts_method_define_varargs(cl, 0, fts_s_set, fvec_set_elements);

  fts_method_define_varargs(cl, 0, fts_s_assign, fvec_assign);
  
  fts_method_define_varargs(cl, 0, fts_new_symbol("reverse"), fvec_reverse);
  fts_method_define_varargs(cl, 0, fts_new_symbol("rotate"), fvec_rotate);
  fts_method_define_varargs(cl, 0, fts_new_symbol("sort"), fvec_sort);
  fts_method_define_varargs(cl, 0, fts_new_symbol("scramble"), fvec_scramble);
  fts_method_define_varargs(cl, 0, fts_new_symbol("normalize"), fvec_normalize);

  fts_method_define_varargs(cl, 0, fts_new_symbol("add"), fvec_add);
  fts_method_define_varargs(cl, 0, fts_new_symbol("sub"), fvec_sub);
  fts_method_define_varargs(cl, 0, fts_new_symbol("mul"), fvec_mul);
  fts_method_define_varargs(cl, 0, fts_new_symbol("div"), fvec_div);
  fts_method_define_varargs(cl, 0, fts_new_symbol("bus"), fvec_bus);
  fts_method_define_varargs(cl, 0, fts_new_symbol("vid"), fvec_vid);
  fts_method_define_varargs(cl, 0, fts_new_symbol("ee"), fvec_ee);
  fts_method_define_varargs(cl, 0, fts_new_symbol("ne"), fvec_ne);
  fts_method_define_varargs(cl, 0, fts_new_symbol("gt"), fvec_gt);
  fts_method_define_varargs(cl, 0, fts_new_symbol("ge"), fvec_ge);
  fts_method_define_varargs(cl, 0, fts_new_symbol("lt"), fvec_lt);
  fts_method_define_varargs(cl, 0, fts_new_symbol("le"), fvec_le);
  fts_method_define_varargs(cl, 0, fts_new_symbol("min"), fvec_min);
  fts_method_define_varargs(cl, 0, fts_new_symbol("max"), fvec_max);

  fts_method_define_varargs(cl, 0, fts_new_symbol("abs"), fvec_abs);
  fts_method_define_varargs(cl, 0, fts_new_symbol("log"), fvec_log);

  fts_method_define_varargs(cl, 0, fts_s_size, fvec_size);
  
  fts_method_define_varargs(cl, 0, fts_s_import, fvec_import);
  fts_method_define_varargs(cl, 0, fts_s_export, fvec_export);
  
  fts_method_define_varargs(cl, 0, fts_s_load, fvec_load);
  fts_method_define_varargs(cl, 0, fts_s_save, fvec_save_soundfile);
  
  return fts_Success;
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
