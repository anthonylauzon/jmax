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
#include <utils/c/include/utils.h>
#include <data/c/include/ivec.h>
#include <data/c/include/fvec.h>
#include <data/c/include/tabeditor.h>

#include <stdlib.h>

#define IVEC_CLIENT_BLOCK_SIZE 256

static fts_symbol_t sym_text = 0;

fts_symbol_t tabeditor_symbol = 0;
fts_class_t *tabeditor_type = 0;

static fts_symbol_t sym_set_visibles = 0;
static fts_symbol_t sym_append_visibles = 0;
static fts_symbol_t sym_set_pixels = 0;
static fts_symbol_t sym_add_pixels = 0;
static fts_symbol_t sym_append_pixels = 0;
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
#define tabeditor_is_ivec(t) ((t)->type)

/*********************************************************
*
*  client utils
*
*/
static void
tabeditor_send_visibles(tabeditor_t *tabeditor)
{
  int i;
  fts_atom_t a[IVEC_CLIENT_BLOCK_SIZE];
  int vecsize = tabeditor_get_size( tabeditor);
  int n = (tabeditor->vindex + tabeditor->vsize <= vecsize)? (tabeditor->vindex + tabeditor->vsize) : vecsize;

  int append = 0;
  int count = 0;
  int send = 0;
  int current = 0;
  int veconset = 2;

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
	  veconset = 1;
	}
      send = (n > IVEC_CLIENT_BLOCK_SIZE-veconset)? IVEC_CLIENT_BLOCK_SIZE-veconset: n;
  
      if( tabeditor_is_ivec( tabeditor))
	for(i = 0; ((i < send)&&( current+i < vecsize)); i++)
	  fts_set_int(&a[i+veconset], ((ivec_t *)tabeditor->vec)->values[current+i]);
      else
	for(i = 0; ((i < send)&&( current+i < vecsize)); i++)
	  fts_set_float(&a[i+veconset], ((fvec_t *)tabeditor->vec)->values[current+i]);

      if(!append)
	{
	  fts_client_send_message((fts_object_t *)tabeditor, sym_set_visibles, send+2, a);
	  append = 1;
	}
      else
	fts_client_send_message((fts_object_t *)tabeditor, sym_append_visibles, send+1, a);
  
      current+=send;
      count+=send;
      n -= send;
    }
}

static void
tabeditor_send_pixels(tabeditor_t *tabeditor)
{
  int i;
  fts_atom_t a[IVEC_CLIENT_BLOCK_SIZE];
  int vecsize = tabeditor_get_size( tabeditor);
  int n = tabeditor->pixsize;
  float k = (1/tabeditor->zoom);

  int append = 0;
  int count = 0;
  int send = 0;
  int current = tabeditor->vindex;
  
  while(n > 0)
    {
      if(!append)
	fts_set_int(&a[0], n);
      else
	fts_set_int(&a[0], count);
  
      send = (n > IVEC_CLIENT_BLOCK_SIZE-1)? (IVEC_CLIENT_BLOCK_SIZE-1): n;
  
      if( tabeditor_is_ivec( tabeditor))
	for(i = 0; ((i < send)&&((int)(current+i*k)<vecsize)); i++)
            fts_set_int(&a[i+1], ivec_get_max_abs_value_in_range((ivec_t *)tabeditor->vec, (int)(current+k*i), (int)(current+k*(i+1))));
      else
	for(i = 0; ((i < send)&&((int)(current+i*k)<vecsize)); i++)
            fts_set_float(&a[i+1], fvec_get_max_abs_value_in_range((fvec_t *)tabeditor->vec, (int)(current+k*i), (int)(current+k*(i+1))));
      
      if(!append)
	{
	  fts_client_send_message((fts_object_t *)tabeditor, sym_set_pixels, send+1, a);
	  append = 1;
	}
      else
	fts_client_send_message((fts_object_t *)tabeditor, sym_append_pixels, send+1, a);
  
      current+=k*send;
      count+=send;
      n -= send;
    }
}

static void
tabeditor_insert_pixels(tabeditor_t *tabeditor, int startId, int size)
{
  int i;
  fts_atom_t a[IVEC_CLIENT_BLOCK_SIZE];
  int vecsize = tabeditor_get_size( tabeditor);
  float k = (1 / tabeditor->zoom);
  int send = (int)(size * tabeditor->zoom) + 1;
  int current = startId;

  fts_set_int(&a[0], (int)((startId - tabeditor->vindex) * tabeditor->zoom));

  if( tabeditor_is_ivec( tabeditor))
    for(i = 0; ((i < send) && ((int)(current + i * k) < vecsize)); i++)
      fts_set_int(&a[i+1], ivec_get_max_abs_value_in_range((ivec_t *)tabeditor->vec, (int)(current+k*i), (int)(current+k*(i+1))));
  else
    for(i = 0; ((i < send) && ((int)(current + i * k) < vecsize)); i++)
      fts_set_float(&a[i+1], fvec_get_max_abs_value_in_range((fvec_t *)tabeditor->vec, (int)(current+k*i), (int)(current+k*(i+1))));

  fts_client_send_message((fts_object_t *)tabeditor, sym_append_pixels, send + 1, a);
}

static void
tabeditor_append_visibles(tabeditor_t *tabeditor, int first, int last)
{
  int i;
  fts_atom_t a[IVEC_CLIENT_BLOCK_SIZE];

  int n = (last-first);

  int current = first;

  while(n > 0)
    {
      int send = (n > IVEC_CLIENT_BLOCK_SIZE-1)? IVEC_CLIENT_BLOCK_SIZE-1: n;
  
      fts_set_int(&a[0], current);
  
      if( tabeditor_is_ivec( tabeditor))
	for(i = 0; i < send; i++)
	  fts_set_int(&a[i+1], ((ivec_t *)tabeditor->vec)->values[current+i]);
      else
	for(i = 0; i < send; i++)
	  fts_set_float(&a[i+1], ((fvec_t *)tabeditor->vec)->values[current+i]);

      fts_client_send_message((fts_object_t *)tabeditor, sym_append_visibles, send+1, a);
  
      current+=send;
      n -= send;
    }
}

static void
tabeditor_append_pixels(tabeditor_t *tabeditor, int deltax, int deltap)
{
  int i;
  fts_atom_t a[IVEC_CLIENT_BLOCK_SIZE];
  int vecsize = tabeditor_get_size( tabeditor);
  float k = (1/tabeditor->zoom);
  int n = (deltax > 0)? deltax : -deltax;

  int current = (deltax < 0)? tabeditor->vindex : (tabeditor->vindex + tabeditor->vsize-deltap);
  int start = (deltax < 0)? 0 : tabeditor->pixsize-deltax;
  
  while(n > 0)
    {
      int send = (n > IVEC_CLIENT_BLOCK_SIZE-1)? IVEC_CLIENT_BLOCK_SIZE-1: n;
      
      fts_set_int(&a[0], start);
      
      if( tabeditor_is_ivec( tabeditor))
	for(i = 0; ((i < send)&&((int)(current+i*k) < vecsize)); i++)
          fts_set_int(&a[i+1], ivec_get_max_abs_value_in_range((ivec_t *)tabeditor->vec, (int)(current+k*i), (int)(current+k*(i+1))));
      else
	for(i = 0; ((i < send)&&((int)(current+i*k) < vecsize)); i++)
          fts_set_float(&a[i+1], fvec_get_max_abs_value_in_range((fvec_t *)tabeditor->vec, (int)(current+k*i), (int)(current+k*(i+1))));	
      fts_client_send_message((fts_object_t *)tabeditor, sym_add_pixels, send+1, a);
  
      current+=k*i;
      start+=send;
      n -= send;
    }
}

/*********************************************************
*
*  client methods
*
*/
static void
tabeditor_end_edit(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_client_send_message(o, sym_end_edit, 0, 0);
}

static void
tabeditor_get_to_client(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tabeditor_t *this = (tabeditor_t *)o;
  if(ac > 1 && fts_is_number(at))
    {
      int first =  fts_get_number_int(at);
      int last = fts_get_number_int(at+1);
      tabeditor_append_visibles( this, first, last);
    }
  else
    tabeditor_send_visibles(this);
}
static void
tabeditor_get_pixels_to_client(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tabeditor_t *this = (tabeditor_t *)o;  
  if(ac > 1 && fts_is_number(at))
    {
      int deltax = fts_get_number_int(at);
      int deltap = fts_get_number_int(at+1);
      tabeditor_append_pixels(this, deltax, deltap);
    }
  else
    tabeditor_send_pixels(this);
}

static void
tabeditor_set_visible_window(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tabeditor_t *this = (tabeditor_t *)o;
  
  if(ac > 1 && fts_is_number(at))
    {
      this->vsize = fts_get_number_int(at);
      this->vindex = fts_get_number_int(at+1);	  
      this->zoom = fts_get_number_float(at+2);	  
      this->pixsize = fts_get_number_int(at+3);	  
    }
}

static void
tabeditor_copy_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tabeditor_t *this = (tabeditor_t *)o;
  int start = fts_get_int(at);
  int size = fts_get_int(at + 1);
  int this_size;
  
  if(size > 0)
    {
      int i;
  
      if( tabeditor_is_ivec( this))
	{
	  int *src, *dst;

	  this_size = ivec_get_size( (ivec_t *)this->vec);

	  if(!this->copy)
	    this->copy = fts_object_create(ivec_type, 1, at + 1);
	  else
	    ivec_set_size( (ivec_t *)this->copy, size);
	
	  src = ivec_get_ptr( (ivec_t *)this->vec);
	  dst = ivec_get_ptr( (ivec_t *)this->copy);
	
	  if(start + size > this_size)
	    size = this_size - start;
	  
	  for(i=0; i<size; i++)
	    dst[i] = src[start + i];
	}
      else
	{
	  float *src, *dst;
		
	  this_size = fvec_get_size( (fvec_t *)this->vec);

	  if(!this->copy)
	    this->copy = fts_object_create(fvec_type, 1, at + 1);
	  else
	    fvec_set_size( (fvec_t *)this->copy, size);
	
	  src = fvec_get_ptr( (fvec_t *)this->vec);
	  dst = fvec_get_ptr( (fvec_t *)this->copy);
	  
	  if(start + size > this_size)
	    size = this_size - start;
	  
	  for(i=0; i<size; i++)
	    dst[i] = src[start + i];
	}
    }
}

static void
tabeditor_cut_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tabeditor_t *this = (tabeditor_t *)o;
  int v_size = fts_get_int(at);
  int pix_size = fts_get_int(at + 1);
  int start = fts_get_int(at + 2);
  int copy_size;
  int i;

  tabeditor_copy_by_client_request(o, 0, 0, ac - 2, at + 2);
  if( tabeditor_is_ivec( this))
    {
      int *ptr;
      copy_size = ivec_get_size( (ivec_t *)this->copy);
      ptr = ivec_get_ptr( (ivec_t *)this->vec);
    
      for(i=0; i<ivec_get_size( (ivec_t *)this->vec); i++)
	ptr[start + i] = ptr[i + start + copy_size];

      ivec_set_size((ivec_t *)this->vec, ivec_get_size( (ivec_t *)this->vec) - copy_size);
    }  
  else
    {
      float *ptr;
      copy_size = fvec_get_size( (fvec_t *)this->copy);
      ptr = fvec_get_ptr( (fvec_t *)this->vec);
    
      for(i = 0; i < fvec_get_size( (fvec_t *)this->vec); i++)
	ptr[start + i] = ptr[i + start + copy_size];

      fvec_set_size((fvec_t *)this->vec, fvec_get_size( (fvec_t *)this->vec) - copy_size);
    }
    
  this->vsize = v_size;
  this->pixsize = pix_size;

  if(this->zoom < 0.5)
    tabeditor_send_pixels( this);
  
  tabeditor_send_visibles( this);
}

static void
tabeditor_paste_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tabeditor_t *this = (tabeditor_t *)o;

  if( this->copy)
    {
      int this_size, copy_size;
      int i;
      int start = fts_get_int(at);
      int size = fts_get_int(at + 1);
      
      if( tabeditor_is_ivec( this))
	{
	  int *src, *dst;
	  this_size = ivec_get_size((ivec_t *)this->vec);
	  copy_size = ivec_get_size((ivec_t *)this->copy);
	  
	  src = ivec_get_ptr((ivec_t *)this->copy);
	  dst = ivec_get_ptr((ivec_t *)this->vec);

	  if(size == 0)
	    size = copy_size;
	  else if(size > copy_size)
	    size = copy_size;
      
	  if(start + size > this_size)
	    size = this_size - start;
	  
	  for(i=0; i<size; i++)
	    dst[start+ i] = src[i];
	}
      else
	{
	  float *src, *dst;
	  this_size = fvec_get_size((fvec_t *)this->vec);
	  copy_size = fvec_get_size((fvec_t *)this->copy);
	
	  src = fvec_get_ptr((fvec_t *)this->copy);
	  dst = fvec_get_ptr((fvec_t *)this->vec);
	  
	  if(size == 0)
	    size = copy_size;
	  else if(size > copy_size)
	    size = copy_size;
	  
	  if(start + size > this_size)
	    size = this_size - start;
	  
	  for(i=0; i<size; i++)
	    dst[start+ i] = src[i];
	}
  
      if(this->zoom < 0.5) tabeditor_send_pixels( this);
      tabeditor_send_visibles( this);
    
      data_object_set_dirty( this->vec);
    }  
}

static void
tabeditor_insert_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tabeditor_t *this = (tabeditor_t *)o;

  if(this->copy)
    {
      int v_size = fts_get_int(at);
      int pix_size = fts_get_int(at+1);
      int start = fts_get_int(at+2);
      int copy_size, this_size;
      int i;
      
      if(tabeditor_is_ivec(this))
	{
	  int *src, *dst;
	  copy_size = ivec_get_size( (ivec_t *)this->copy);
	  ivec_set_size( (ivec_t *)this->vec, ivec_get_size( (ivec_t *)this->vec) + copy_size);
	  this_size = ivec_get_size( (ivec_t *)this->vec);
	    
	  src = ivec_get_ptr((ivec_t *)this->copy);
	  dst = ivec_get_ptr((ivec_t *)this->vec);

	  for(i=this_size - 1; i>=start; i--)
	    dst[i] = dst[i - copy_size];
  
	  for(i=0; i < copy_size; i++)
	    dst[start + i] = src[i];	  
	}
      else
	{
	  float *src, *dst;		
	  copy_size = fvec_get_size( (fvec_t *)this->copy);
	  fvec_set_size( (fvec_t *)this->vec, fvec_get_size( (fvec_t *)this->vec) + copy_size);
	  this_size = fvec_get_size( (fvec_t *)this->vec);
	
	  src = fvec_get_ptr((fvec_t *)this->copy);
	  dst = fvec_get_ptr((fvec_t *)this->vec);
	
	  for(i=this_size - 1; i>=start; i--)
	    dst[i] = dst[i - copy_size];
	  
	  for(i=0; i < copy_size; i++)
	    dst[start + i] = src[i];	  
	}
  
      this->vsize = v_size;
      this->pixsize = pix_size;
  
      if( this->zoom < 0.5) tabeditor_send_pixels( this);
      tabeditor_send_visibles(this);
      
      data_object_set_dirty( this->vec);
    }
}

/********************************************************************
 *
 *   user methods
 *
 */

static void
tabeditor_set_elements(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tabeditor_t *this = (tabeditor_t *)o;
  
  if(ac > 1 && fts_is_number(at))
    {
      int size = tabeditor_get_size( this);
      int onset = fts_get_number_int(at);
  
      if(onset >= 0 && onset < size)
	{
	  if( tabeditor_is_ivec(this))
	    {
	      ivec_set_with_onset_from_atoms( (ivec_t *)this->vec, onset, ac - 1, at + 1);
	  
	      if( ivec_editor_is_open( (ivec_t *)this->vec))
		tabeditor_insert_append( this, onset, ac, at);
	    }
	  else
	    {
	      fvec_set_with_onset_from_atoms( (fvec_t *)this->vec, onset, ac - 1, at + 1);
	  
	      if( fvec_editor_is_open( (fvec_t *)this->vec))
		tabeditor_insert_append( this, onset, ac, at);
	    }

	  data_object_set_dirty( this->vec);
	}
    }
}

void tabeditor_insert_append(tabeditor_t *tabeditor, int onset, int ac, const fts_atom_t *at)
{
  if( tabeditor->zoom < 0.5) 
    tabeditor_insert_pixels( tabeditor, onset, ac - 1);
	      
  fts_client_send_message((fts_object_t *)tabeditor, sym_append_visibles, ac, at);
}

void tabeditor_send( tabeditor_t *tabeditor)
{
  if( tabeditor->zoom < 0.5) 
    tabeditor_send_pixels( tabeditor);
      
  tabeditor_send_visibles( tabeditor);
}

int tabeditor_get_size( tabeditor_t *tabeditor)
{
  int size;
  if( tabeditor_is_ivec( tabeditor))
    size = ivec_get_size( ((ivec_t *)tabeditor->vec));
  else
    size = fvec_get_size( ((fvec_t *)tabeditor->vec));

  return size;
}

/*********************************************************
 *
 *  class
 *
 */
static void
tabeditor_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tabeditor_t *this = (tabeditor_t *)o;
  
  this->opened = 0; 
  this->vsize = 0; 
  this->vindex = 0;
  this->zoom = 1.0;
  this->pixsize = 1;
  
  if(ac == 1 && fts_is_object(at))
    {
      this->vec = fts_get_object( at);
      this->type = (fts_object_get_class(this->vec) == ivec_type);
    }
  else
    {
      this->vec = 0; 
      this->type = -1;
   }
  
  this->copy = 0;
}

static void
tabeditor_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tabeditor_t *this = (tabeditor_t *)o;
  
  if(fts_object_has_id(o))
    fts_client_send_message(o, fts_s_destroyEditor, 0, 0);

  if(this->copy)
    fts_object_destroy((fts_object_t *)this->copy);
}

static void
tabeditor_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(tabeditor_t), tabeditor_init, tabeditor_delete);
  
  fts_class_message_varargs(cl, fts_new_symbol("set_from_client"), tabeditor_set_elements);
  fts_class_message_varargs(cl, fts_new_symbol("get_from_client"), tabeditor_get_to_client);
  fts_class_message_varargs(cl, fts_new_symbol("get_pixels_from_client"), tabeditor_get_pixels_to_client);
  fts_class_message_varargs(cl, fts_new_symbol("set_visible_window"), tabeditor_set_visible_window);
  fts_class_message_varargs(cl, fts_new_symbol("end_edit"), tabeditor_end_edit);
  fts_class_message_varargs(cl, fts_new_symbol("copy_from_client"), tabeditor_copy_by_client_request);
  fts_class_message_varargs(cl, fts_new_symbol("paste_from_client"), tabeditor_paste_by_client_request);
  fts_class_message_varargs(cl, fts_new_symbol("cut_from_client"), tabeditor_cut_by_client_request);
  fts_class_message_varargs(cl, fts_new_symbol("insert_from_client"), tabeditor_insert_by_client_request);
}

/********************************************************************
 *
 *  config
 *
 */

void 
tabeditor_config(void)
{
  sym_text = fts_new_symbol("text");
  tabeditor_symbol = fts_new_symbol("tabeditor");

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

  sym_set_visible_size = fts_new_symbol("setVisibleSize");

  tabeditor_type = fts_class_install(tabeditor_symbol, tabeditor_instantiate);
}










