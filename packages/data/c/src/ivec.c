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
#include "ivec.h"

static fts_symbol_t sym_text = 0;

fts_symbol_t ivec_symbol = 0;
fts_type_t ivec_type = 0;
fts_class_t *ivec_class = 0;

static fts_symbol_t sym_local = 0;

static fts_symbol_t sym_openEditor = 0;
static fts_symbol_t sym_destroyEditor = 0;
static fts_symbol_t sym_set = 0;
static fts_symbol_t sym_append = 0;
static fts_symbol_t sym_set_visibles = 0;
static fts_symbol_t sym_append_visibles = 0;
static fts_symbol_t sym_set_pixels = 0;
static fts_symbol_t sym_add_pixels = 0;
static fts_symbol_t sym_append_pixels = 0;
static fts_symbol_t sym_set_size = 0;
static fts_symbol_t sym_end_edit = 0;
static fts_symbol_t sym_start_edit = 0;
static fts_symbol_t sym_set_visible_size = 0;
static fts_symbol_t sym_copy_client = 0;
static fts_symbol_t sym_paste_client = 0;
static fts_symbol_t sym_cut_client = 0;
static fts_symbol_t sym_insert_client = 0;

/********************************************************
 *
 *  utility functions
 *
 */

#define ivec_set_editor_open(b) ((b)->opened = 1)
#define ivec_set_editor_close(b) ((b)->opened = 0)
#define ivec_editor_is_open(b) ((b)->opened)

/* local */
static void
set_size(ivec_t *vec, int size)
{
  int i;

  if(size > vec->alloc)
    {
      if(vec->alloc)
	vec->values = (int *)fts_realloc((void *)vec->values, sizeof(int) * size);
      else
	vec->values = (int *)fts_malloc(sizeof(int) * size);

      vec->alloc = size;
    }

  /* when shortening: zero old values */
  for(i=size; i<vec->size; i++)
    vec->values[i] = 0;

  vec->size = size;
}

void
ivec_set_const(ivec_t *vec, int c)
{
  int *values = vec->values;
  int i;
  
  for(i=0; i<vec->size; i++)
    values[i] = c;
}

void
ivec_set_size(ivec_t *vec, int size)
{
  int old_size = vec->size;
  int i;

  set_size(vec, size);

  /* when extending: zero new values */
  for(i=old_size; i<size; i++)
    vec->values[i] = 0;
}

void
ivec_set_from_atom_list(ivec_t *vec, int offset, int ac, const fts_atom_t *at)
{
  int size = ivec_get_size(vec);
  int i;
 
  if(offset + ac > size)
    ac = size - offset;
  
  for(i=0; i<ac; i++)
    {
      if(fts_is_number(at + i))
	vec->values[i + offset] = fts_get_number_int(at + i);
      else
	vec->values[i + offset] = 0;
    }
}

/* sum, min, max */
int 
ivec_get_sum(ivec_t *vec)
{
  int sum = 0;
  int i;

  for(i=0; i<vec->size; i++)
    sum += vec->values[i];

  return sum;
}

int
ivec_get_sub_sum(ivec_t *vec, int from, int to)
{
  int sum = 0;
  int i;
  
  if(from < 0)
    from = 0;

  if(to >= vec->size)
    to = vec->size - 1;

  for(i=from; i<=to; i++)
    sum += vec->values[i];

  return sum;
}

int
ivec_get_min_value(ivec_t *vec)
{
  int min;
  int i;

  min = vec->values[0];

  for (i=1; i<vec->size; i++)
    if (vec->values[i] < min)
      min = vec->values[i];

  return min;
}


int
ivec_get_max_value(ivec_t *vec)
{
  int max;
  int i;

  max = vec->values[0];

  for (i=1; i<vec->size; i++)
    if (vec->values[i] > max)
      max = vec->values[i];

  return max;
}

/********************************************************
 *
 *  files
 *
 */

#define IVEC_BLOCK_SIZE 256

static void
ivec_grow(ivec_t *vec, int size)
{
  int alloc = vec->alloc;

  while(!alloc || size > alloc)
    alloc += IVEC_BLOCK_SIZE;

  set_size(vec, alloc);
}

int 
ivec_read_atom_file(ivec_t *vec, fts_symbol_t file_name)
{
  fts_atom_file_t *file = fts_atom_file_open(fts_symbol_name(file_name), "r");
  int n = 0;
  fts_atom_t a;
  char c;

  if(!file)
    return -1;
  
  while(fts_atom_file_read(file, &a, &c))
    {
      if(n >= vec->alloc)
	ivec_grow(vec, n);

      if(fts_is_number(&a))
	ivec_set_element(vec, n, fts_get_number_int(&a));
      else
	ivec_set_element(vec, n, 0);
	
      n++;
    }

  ivec_set_size(vec, n);
  
  fts_atom_file_close(file);

  return (n);
}

int
ivec_write_atom_file(ivec_t *vec, fts_symbol_t file_name)
{
  fts_atom_file_t *file;
  int size = ivec_get_size(vec);
  int i;

  file = fts_atom_file_open(fts_symbol_name(file_name), "w");

  if(!file)
    return -1;

  /* write the content of the vec */
  for(i=0; i<size; i++)     
    {
      fts_atom_t a;
      
      fts_set_int(&a, ivec_get_element(vec, i));
      fts_atom_file_write(file, &a, '\n');
    }

  fts_atom_file_close(file);

  return (i);
}

/********************************************************************
 *
 *   user methods
 *
 */
#define IVEC_CLIENT_BLOCK_SIZE 256

static void
ivec_send_visibles(ivec_t *ivec)
{
  int i;
  fts_atom_t a[IVEC_CLIENT_BLOCK_SIZE];
  int vecsize = ivec_get_size(ivec);
  int n = (ivec->vindex+ivec->vsize <= vecsize)? (ivec->vindex + ivec->vsize) : vecsize;

  int append = 0;
  int count = 0; 
  int send = 0;
  int current = 0;
  int vecoffset = 2;

  while(n > 0)
    {
      if(!append)
	{
	  fts_set_int(&a[0], vecsize);
	  fts_set_int(&a[1], n);
	}
      else
	{
	  fts_set_int(&a[0], count);
	  vecoffset = 1;
	}
      send = (n > IVEC_CLIENT_BLOCK_SIZE-vecoffset)? IVEC_CLIENT_BLOCK_SIZE-vecoffset: n;

      for(i = 0; ((i < send)&&(current+i<vecsize)); i++)
	  fts_set_int(&a[i+vecoffset], ivec->values[current+i]);

      if(!append)
	{
	  fts_client_send_message((fts_object_t *)ivec, sym_set_visibles, send+2, a);
	  append = 1;
	}
      else
	fts_client_send_message((fts_object_t *)ivec, sym_append_visibles, send+1, a);

      current+=send;
      count+=send;
      n -= send;
    }
}

static void
ivec_send_pixels(ivec_t *ivec)
{
  int i;
  fts_atom_t a[IVEC_CLIENT_BLOCK_SIZE];
  int vecsize = ivec_get_size(ivec);
  int n = ivec->pixsize;
  float k = (1/ivec->zoom);

  int append = 0;
  int count = 0; 
  int send = 0;
  /*int current = 0;*/
  int current = ivec->vindex;

  while(n > 0)
    {
      if(!append)
	  fts_set_int(&a[0], n);
      else
	  fts_set_int(&a[0], count);

      send = (n > IVEC_CLIENT_BLOCK_SIZE-1)? (IVEC_CLIENT_BLOCK_SIZE-1): n;

      for(i = 0; ((i < send)&&((int)(current+i*k)<vecsize)); i++)
	  fts_set_int(&a[i+1], ivec->values[(int)(current+k*i)]);
      
      if(!append)
	  {
	      fts_client_send_message((fts_object_t *)ivec, sym_set_pixels, send+1, a);
	      append = 1;
	  }
      else
	  fts_client_send_message((fts_object_t *)ivec, sym_append_pixels, send+1, a);

      current+=k*send;
      count+=send;
      n -= send;
    }
}

static void
ivec_insert_pixels(ivec_t *ivec, int startId, int size)
{
  int i;
  fts_atom_t a[IVEC_CLIENT_BLOCK_SIZE];
  int vecsize = ivec_get_size(ivec);
  float k = (1/ivec->zoom);
  int send = (int)(size/k)+1;
  int current = startId;

  fts_set_int(&a[0], (int)((startId - ivec->vindex)/k));
  
  for(i = 0; ((i < send)&&((int)(current+i*k)<vecsize)); i++)
    fts_set_int(&a[i+1], ivec->values[(int)(current+k*i)]);
      
  fts_client_send_message((fts_object_t *)ivec, sym_append_pixels, send+1, a);
}

static void
ivec_append_visibles(ivec_t *ivec, int first, int last)
{
  int i;
  fts_atom_t a[IVEC_CLIENT_BLOCK_SIZE];
  int vecsize = ivec_get_size(ivec);

  int n = (last-first);

  int append = 0;
  int current = first;

  while(n > 0)
    {
	int send = (n > IVEC_CLIENT_BLOCK_SIZE-1)? IVEC_CLIENT_BLOCK_SIZE-1: n;

	fts_set_int(&a[0], current);
	
	for(i = 0; i < send; i++)
	    fts_set_int(&a[i+1], ivec->values[current+i]);
      
	fts_client_send_message((fts_object_t *)ivec, sym_append_visibles, send+1, a);

	current+=send;
	n -= send;
    }
}

static void
ivec_append_pixels(ivec_t *ivec, int deltax, int deltap)
{
  int i;
  fts_atom_t a[IVEC_CLIENT_BLOCK_SIZE];
  int vecsize = ivec_get_size(ivec);
  float k = (1/ivec->zoom);
  int n = (deltax > 0)? deltax : -deltax;

  int append = 0;
  int current = (deltax < 0)? ivec->vindex : (ivec->vindex+ivec->vsize-deltap);
  int start = (deltax < 0)? 0 : ivec->pixsize-deltax;

  while(n > 0)
    {
	int send = (n > IVEC_CLIENT_BLOCK_SIZE-1)? IVEC_CLIENT_BLOCK_SIZE-1: n;
	
	fts_set_int(&a[0], start);

	for(i = 0; ((i < send)&&((int)(current+i*k) < vecsize)); i++)
	    fts_set_int(&a[i+1], ivec->values[(int)(current+k*i)]);
      
	fts_client_send_message((fts_object_t *)ivec, sym_add_pixels, send+1, a);

	current+=k*i;
	start+=send;
	n -= send;
    }
}

static void
ivec_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;
  fts_atom_t a[1];

  ivec_atom_set(a, this);
  fts_outlet_send(o, 0, ivec_symbol, 1, a);
}

static void
ivec_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;

  ivec_set_const(this, 0);

  if(ivec_editor_is_open(this))
    {
      if(this->zoom<0.5) ivec_send_pixels(this);
      ivec_send_visibles(this);
    }
}

static void
ivec_fill(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;
  int constant = fts_get_int_arg(ac, at, 0, 0);

  ivec_set_const(this, constant);

  if(ivec_editor_is_open(this))
      {
	if(this->zoom<0.5) ivec_send_pixels(this);
	ivec_send_visibles(this);
      }
}

static void
ivec_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;

  if(ac > 1 && fts_is_number(at))
    {
      int size = ivec_get_size(this);
      int offset = fts_get_number_int(at);

      if(offset >= 0 && offset < size)
	{
	  ivec_set_from_atom_list(this, offset, ac - 1, at + 1);
	  
	  if(ivec_editor_is_open(this))
	    {
	      if(this->zoom<0.5) 
		ivec_insert_pixels(this, offset, ac - 1);
	      
	      fts_client_send_message((fts_object_t *)this, sym_append_visibles, ac, at);
	    }
	}
    }
}

static void
ivec_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;

  if(ac == 1 && fts_is_number(at))
    {
      int size = fts_get_number_int(at);
      
      if(size >= 0)
	  {
	      ivec_set_size(this, size);
	      if(ivec_editor_is_open(this))
		  fts_client_send_message((fts_object_t *)this, sym_set_size, ac, at);
	  }
    }
}

static void
ivec_import(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_format = fts_get_symbol_arg(ac, at, 1, sym_text);

  if(!file_name)
    return;

  if(file_format == sym_text)
    {
      int size = ivec_read_atom_file(this, file_name);
      
      if(size <= 0)
	post("ivec: can't import from text file \"%s\"\n", fts_symbol_name(file_name));
    }
  else
    post("ivec: unknown import file format \"%s\"\n", fts_symbol_name(file_format));
}

static void
ivec_export(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_format = fts_get_symbol_arg(ac, at, 1, sym_text);

  if(!file_name)
    return;

  if(file_format == sym_text)
    {
      int size = ivec_write_atom_file(this, file_name);
      
      if(size < 0)
	post("ivec: can't export to text file \"%s\"\n", fts_symbol_name(file_name));
    }
  else
    post("ivec: unknown export file format \"%s\"\n", fts_symbol_name(file_format));
}

static void
ivec_open_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;

  if(!fts_object_has_id(o))
    fts_client_upload(o, ivec_symbol, 0, 0);

  ivec_set_editor_open(this);
  fts_client_send_message(o, sym_openEditor, 0, 0);
}

static void
ivec_close_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;

  ivec_set_editor_close(this);
}

static void
ivec_end_edit(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_client_send_message(o, sym_end_edit, 0, 0);
}

static void
ivec_get_to_client(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;
  if(ac > 1 && fts_is_number(at))
      {
	  int first =  fts_get_number_int(at);
	  int last = fts_get_number_int(at+1);
	  ivec_append_visibles(this, first, last);
      }
  else
      ivec_send_visibles(this);
}
static void
ivec_get_pixels_to_client(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;  
  if(ac > 1 && fts_is_number(at))
      {
	  int deltax = fts_get_number_int(at);
	  int deltap = fts_get_number_int(at+1);
	  ivec_append_pixels(this, deltax, deltap);
      }
  else
      ivec_send_pixels(this);
}

static void
ivec_set_visible_window(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;
  
  if(ac > 1 && fts_is_number(at))
    {
      this->vsize = fts_get_number_int(at);
      this->vindex = fts_get_number_int(at+1);	  
      this->zoom = fts_get_number_float(at+2);	  
      this->pixsize = fts_get_number_int(at+3);	  
    }
}

static void
ivec_copy_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;
  int start = fts_get_int(at);
  int size = fts_get_int(at + 1);
  int this_size = ivec_get_size(this);
  
  if(size > 0)
    {
      int *src, *dst;
      int i;
      
      if(!this->copy)
	this->copy = (ivec_t *)fts_object_create(ivec_class, 1, at + 1);
      else
	ivec_set_size(this->copy, size);
      
      src = ivec_get_ptr(this);
      dst = ivec_get_ptr(this->copy);
      
      if(start + size > this_size)
	size = this_size - start;
      
      for(i=0; i<size; i++)
	dst[i] = src[start + i];
    }
}

static void
ivec_cut_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;
  int v_size = fts_get_int(at);
  int pix_size = fts_get_int(at+1);
  int start = fts_get_int(at+2);
  int copy_size;
  int *ptr;
  int i;

  ivec_copy_by_client_request(o, 0, 0, ac-2, at+2);
  copy_size = ivec_get_size(this->copy);

  ptr = ivec_get_ptr(this);
  
  for(i=0; i<ivec_get_size(this); i++)
    ptr[start + i] = ptr[i + start + copy_size];

  ivec_set_size(this, ivec_get_size(this) - copy_size);
  
  this->vsize = v_size;
  this->pixsize = pix_size;

  if(this->zoom<0.5) ivec_send_pixels(this);
  ivec_send_visibles(this);
}

static void
ivec_paste_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;

  if(this->copy)
    {
      int start = fts_get_int(at);
      int size = fts_get_int(at + 1);
      int this_size = ivec_get_size(this);
      int copy_size = ivec_get_size(this->copy);
      int *src, *dst;
      int i;
      
      if(size == 0)
	size = copy_size;
      else if(size > copy_size)
	size = copy_size;
      
      src = ivec_get_ptr(this->copy);
      dst = ivec_get_ptr(this);
      
      if(start + size > this_size)
	size = this_size - start;
      
      for(i=0; i<size; i++)
	  dst[start+ i] = src[i];
    
      if(this->zoom<0.5) ivec_send_pixels(this);
      ivec_send_visibles(this);
    }  
}

static void
ivec_insert_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;

  if(this->copy)
    {
      int v_size = fts_get_int(at);
      int pix_size = fts_get_int(at+1);
      int start = fts_get_int(at+2);
      int copy_size = ivec_get_size(this->copy);
      int this_size;
      int *src, *dst;
      int i;

      ivec_set_size(this, ivec_get_size(this) + copy_size);
      this_size = ivec_get_size(this);

      src = ivec_get_ptr(this->copy);
      dst = ivec_get_ptr(this);

      for(i=this_size - 1; i>=start; i--)
	dst[i] = dst[i - copy_size];

      for(i=0; i<copy_size; i++)
	dst[start + i] = src[i];

      this->vsize = v_size;
      this->pixsize = pix_size;

      if(this->zoom<0.5) ivec_send_pixels(this);
      ivec_send_visibles(this);
    }
}

/********************************************************************
 *
 *  system functions
 *
 */

static void
ivec_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;
  int size = ivec_get_size(this);

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
	    post("%d ", ivec_get_element(this, i + j));
	}
	  
      /* print last line with indent */
      if(i < size)
	{
	  post("\n  ");
	  for(; i<size; i++)
	    post("%d ", ivec_get_element(this, i));
	}

      post("\n}");
    }
  else if(size > 0)
    {
      int i;
      
      for(i=0; i<size-1; i++)
	post("%d ", ivec_get_element(this, i));

      post("%d}", ivec_get_element(this, size - 1));
    }
  else
    post("}");
}

static void
ivec_save_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;

  if(this->persistent)
    {
      fts_bmax_file_t *f = (fts_bmax_file_t *)fts_get_ptr(at);      
      int size = ivec_get_size(this);
      fts_atom_t av[257];
      int ac = 1;
      int i;
      
      fts_set_int(av, 0); /* set offset to 0 */
      
      for(i=0; i<size; i++)
	{
	  fts_set_int(av + ac, this->values[i]);
	  
	  if(ac == 256)
	    {
	      fts_bmax_save_message(f, fts_s_set, ac, av);
	      fts_set_int(av , i + 1); /* set next offset */
	      ac = 1;
	    }
	  
	  ac++;
	}
      
      if(ac > 1) 
	fts_bmax_save_message(f, fts_s_set, ac, av);
    }
}

static void
ivec_assist(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_symbol_t cmd = fts_get_symbol_arg(ac, at, 0, 0);

  if (cmd == fts_s_object)
    fts_object_blip(o, "vector of ints");
  else if (cmd == fts_s_inlet)
    {
      int n = fts_get_int_arg(ac, at, 1, 0);

      switch(n)
	{
	case 0:
	  /* fts_object_blip(o, "no comment"); */
	  break;
	}
    }
}

static void
ivec_set_persistent(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  ivec_t *this = (ivec_t *)obj;

  if(fts_is_symbol(value))
    {
      fts_symbol_t s = fts_get_symbol(value);

      if(s == fts_s_yes)
	this->persistent = 1;
      else
	this->persistent = 0;	
    }
}

static void
ivec_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  ivec_t *this = (ivec_t *)obj;

  ivec_atom_set(value, this);
}

/*********************************************************
 *
 *  class
 *
 */
/* new/delete */

static void
ivec_alloc(ivec_t *vec, int size)
{
  int i;

  if(size > 0)
    {
      vec->values = (int *) fts_malloc(size * sizeof(int));
      vec->size = size;

      /* init to zero */
      for(i=0; i<size; i++)
	vec->values[i] = 0;
    }
  else
    {
      vec->values = 0;
      vec->size = 0;
    }

  vec->alloc = size;
}

static void
ivec_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;
  
  /* skip class name */
  ac--;
  at++;

  if(ac == 0)
    ivec_alloc(this, 0);
  else if(ac == 1 && fts_is_int(at))
    ivec_alloc(this, fts_get_int(at));
  else if(ac == 1 && fts_is_list(at))
    {
      fts_list_t *aa = fts_get_list(at);
      int size = fts_list_get_size(aa);

      ivec_alloc(this, size);
      ivec_set_from_atom_list(this, 0, size, fts_list_get_ptr(aa));
    }
  else if(ac > 1)
    {
      ivec_alloc(this, ac);
      ivec_set_from_atom_list(this, 0, ac, at);
    }

  this->persistent = 0;
  this->opened = 0; 
  this->vsize = 0; 
  this->vindex = 0;
  this->zoom = 1.0;
  this->pixsize = 1;
  this->copy = 0;
}

static void
ivec_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;
  
  if(fts_object_has_id(o))
    fts_client_send_message(o, sym_destroyEditor, 0, 0);

  if(this->copy)
    fts_object_destroy((fts_object_t *)this->copy);

  if(this->values)
    fts_free(this->values);
}

static int
ivec_check(int ac, const fts_atom_t *at)
{
  return (ac == 0 || (ac == 1 && (fts_is_int(at) || fts_is_list(at))) || (ac > 1));
}

static fts_status_t
ivec_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ivec_check(ac - 1, at + 1))
    {
      fts_class_init(cl, sizeof(ivec_t), 1, 1, 0);
  
      /* init / delete */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, ivec_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, ivec_delete);
  
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_print, ivec_print); 
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("assist"), ivec_assist); 

      /* save and restore to/from bmax file */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_save_bmax, ivec_save_bmax); 
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, ivec_set);

      fts_class_add_daemon(cl, obj_property_get, fts_s_state, ivec_get_state);
      fts_class_add_daemon(cl, obj_property_put, fts_s_keep, ivec_set_persistent);

      fts_method_define_varargs(cl, 0, fts_s_bang, ivec_output);

      fts_method_define_varargs(cl, 0, fts_new_symbol("clear"), ivec_clear);
      fts_method_define_varargs(cl, 0, fts_new_symbol("fill"), ivec_fill);
      fts_method_define_varargs(cl, 0, fts_new_symbol("set"), ivec_set);
      
      fts_method_define_varargs(cl, 0, fts_new_symbol("size"), ivec_size);

      fts_method_define_varargs(cl, 0, fts_new_symbol("import"), ivec_import);
      fts_method_define_varargs(cl, 0, fts_new_symbol("export"), ivec_export);

      fts_method_define_varargs(cl, 0, fts_s_print, ivec_print); 

      /* graphical editor */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("open_editor"), ivec_open_editor);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("close_editor"), ivec_close_editor);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("set_from_client"), ivec_set);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("get_from_client"), ivec_get_to_client);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("get_pixels_from_client"), ivec_get_pixels_to_client);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("set_visible_window"), ivec_set_visible_window);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("end_edit"), ivec_end_edit);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("copy_from_client"), ivec_copy_by_client_request);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("paste_from_client"), ivec_paste_by_client_request);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("cut_from_client"), ivec_cut_by_client_request);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("insert_from_client"), ivec_insert_by_client_request);

      /* type outlet */
      fts_outlet_type_define(cl, 0, ivec_symbol, 1, &ivec_type);      

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
ivec_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  return ivec_check(ac1 - 1, at1 + 1);
}

void 
ivec_config(void)
{
  sym_text = fts_new_symbol("text");
  ivec_symbol = fts_new_symbol("ivec");
  ivec_type = ivec_symbol;

  sym_local = fts_new_symbol("local");

  sym_openEditor = fts_new_symbol("openEditor");
  sym_destroyEditor = fts_new_symbol("destroyEditor");
  sym_set = fts_new_symbol("set");
  sym_append = fts_new_symbol("append");
  sym_copy_client = fts_new_symbol("copy");
  sym_paste_client = fts_new_symbol("paste");
  sym_cut_client = fts_new_symbol("cut");
  sym_insert_client = fts_new_symbol("insert");

  sym_set_visibles = fts_new_symbol("setVisibles");
  sym_append_visibles = fts_new_symbol("appendVisibles");
  sym_set_pixels = fts_new_symbol("setPixels");
  sym_append_pixels = fts_new_symbol("appendPixels");
  sym_add_pixels = fts_new_symbol("addPixels");
  sym_end_edit = fts_new_symbol("endEdit");
  sym_start_edit = fts_new_symbol("startEdit");

  sym_set_size = fts_new_symbol("setSize");
  sym_set_visible_size = fts_new_symbol("setVisibleSize");

  fts_metaclass_install(ivec_symbol, ivec_instantiate, ivec_equiv);
  ivec_class = fts_class_get_by_name(ivec_symbol);

  fts_atom_type_register(ivec_symbol, ivec_class);
}
