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

#ifdef WIN32
#define fabsf fabs
#endif

#define CLIENT_BLOCK_SIZE 256
#define MAX_BLOCK_SIZE 8192
#define TABEDITOR_DEFAULT_MIN  -1.0
#define TABEDITOR_DEFAULT_MAX  1.0

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
static fts_symbol_t sym_reset_editor = 0;

/********************************************************
*
*  utility functions
*
*/
#define tabeditor_is_ivec(t) ((t)->type)
#define CUT_TO_BOUNDS(val, min, max) ((val > max) ? max : ((val < min) ? min : val))

/*********************************************************
*
*  client utils
*
*/
static void
fvec_get_min_max_in_range(float *ptr, int size, int stride, int i_min, int i_max, float *min, float *max)
{
  int i = i_min * stride;
  
  *min = *max = ptr[i];
  i+=stride;
  
  for(; i<i_max*stride; i+=stride)
  {
    float f = ptr[i];
    
    if(f < *min)
      *min = f;
    
    if(f > *max)
      *max = f;
  }
}

static void
tabeditor_send_visibles(tabeditor_t *tabeditor)
{
  int i = 0;
  fts_atom_t a[CLIENT_BLOCK_SIZE];
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
    send = (n > CLIENT_BLOCK_SIZE-veconset)? CLIENT_BLOCK_SIZE-veconset: n;
    
    if( tabeditor_is_ivec( tabeditor))
      for(i = 0; ((i < send)&&( current+i < vecsize)); i++)
        fts_set_int(&a[i+veconset], ((ivec_t *)tabeditor->vec)->values[current+i]);
    else
    {
      float *ptr;
      int size;
      int stride;
      int i, j;
      
      fvec_vector(tabeditor->vec, &ptr, &size, &stride);
      
      for(i=0, j=current*stride; i < send && current+i < size; i++, j+=stride)
        fts_set_float(a + i + veconset, ptr[j]);
    }
    if(i+veconset < send) send = i+veconset;
    
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
  int i, j;
  fts_atom_t a[CLIENT_BLOCK_SIZE];
  int vecsize = tabeditor_get_size( tabeditor);
  int n = tabeditor->pixsize;
  int num_val = n*2;
  float k = (float)(1/tabeditor->zoom);
  int append = 0;
  int count = 0;
  int send = 0;
  int current = tabeditor->vindex;
    
  while(num_val > 0)
  {
    if(!append)
      fts_set_int(&a[0], n);
    else
      fts_set_int(&a[0], count);
    
    fts_set_int(&a[1], vecsize);
    
    send = (num_val > CLIENT_BLOCK_SIZE-2)? (CLIENT_BLOCK_SIZE-2): num_val;
    
    j = 0;
    if( tabeditor_is_ivec( tabeditor))
      for(i = 0; ((i < send-1)&&((int)(current+j*k)<vecsize)); i+=2)
      {	  
        fts_set_int(&a[i+2], ivec_get_max_value_in_range((ivec_t *)tabeditor->vec, (int)(current+k*j), (int)(current+k*(j+1))));
        fts_set_int(&a[i+3], ivec_get_min_value_in_range((ivec_t *)tabeditor->vec, (int)(current+k*j), (int)(current+k*(j+1))));
        j++;
      }      
    else
    {
      float *ptr;
      int size;
      int stride;
      
      fvec_vector(tabeditor->vec, &ptr, &size, &stride);
      
      for(i = 0, j = 0; (i < send-1 && ((int)(current+j*k)<vecsize)); i+=2)
      {
        float min, max;
        
        fvec_get_min_max_in_range(ptr, size, stride, (int)(current + k * j), (int)(current + k * (j + 1)), &min, &max);
        
        fts_set_float(a + i + 2, max);
        fts_set_float(a + i + 3, min);
        j++;
      }
    }
    if(i < send-1) send = i; 
    
    if(!append)
    {
      fts_client_send_message((fts_object_t *)tabeditor, sym_set_pixels, send+2, a);
      append = 1;
    }
    else
      fts_client_send_message((fts_object_t *)tabeditor, sym_append_pixels, send+2, a);

    current += (int)k*j;
    count+=j;
    num_val -= send;
  }
}

static void
tabeditor_insert_pixels(tabeditor_t *tabeditor, int startId, int val_size)
{
  int i, j, n, current, send, current_pix;
  fts_atom_t a[CLIENT_BLOCK_SIZE];
  int vecsize = tabeditor_get_size( tabeditor);
  float k = (float)(1.0 / tabeditor->zoom);
  
  n = ((int)(val_size * tabeditor->zoom) + 1)*2;
  current = startId;
  current_pix = (int)((current - tabeditor->vindex) * tabeditor->zoom);
  send = 0;
  
  while(n > 0)
  {
    send = (n > CLIENT_BLOCK_SIZE-2) ? CLIENT_BLOCK_SIZE-2 : n;
        
    fts_set_int(&a[0], current_pix);
    fts_set_int(&a[1], vecsize);
    j = 0;
    if( tabeditor_is_ivec( tabeditor))
      for(i = 0; ((i < send-1) && ((int)(current + j * k) < vecsize)); i+=2)
      {
        fts_set_int(&a[i+2], ivec_get_max_value_in_range((ivec_t *)tabeditor->vec, (int)(current+k*j), (int)(current+k*(j+1))));
        fts_set_int(&a[i+3], ivec_get_min_value_in_range((ivec_t *)tabeditor->vec, (int)(current+k*j), (int)(current+k*(j+1))));
        j++;
      }
    else
    {
      float *ptr;
      int size;
      int stride;
    
      fvec_vector(tabeditor->vec, &ptr, &size, &stride);
    
      for(i = 0, j = 0; (i < send-1 && ((int)(current + j * k) < size)); i+=2)
      {    
        float min, max;
        
        fvec_get_min_max_in_range(ptr, size, stride, (int)(current + k * j), (int)(current + k * (j + 1)), &min, &max);
        
        fts_set_float(a + i + 2, max);
        fts_set_float(a + i + 3, min);
        j++;
      }
    }
    if(i < send-1) 
    {
      send = i;
      n = 0;
    }
    fts_client_send_message((fts_object_t *)tabeditor, sym_append_pixels, send+2, a);
    
    current+=(int)((send/2)*k);
    current_pix+=send/2;
    n -= send;
  }
}

static void
tabeditor_append_visibles(tabeditor_t *tabeditor, int first, int last)
{
  int i;
  fts_atom_t a[CLIENT_BLOCK_SIZE];
  
  int n = (last-first);
  
  int current = first;
  
  while(n > 0)
  {
    int send = (n > CLIENT_BLOCK_SIZE-1)? CLIENT_BLOCK_SIZE-1: n;
    
    fts_set_int(&a[0], current);
    
    if( tabeditor_is_ivec( tabeditor))
      for(i = 0; i < send; i++)
        fts_set_int(&a[i+1], ((ivec_t *)tabeditor->vec)->values[current+i]);
    else
    {
      float *ptr;
      int size;
      int stride;
      int i, j;
      
      fvec_vector(tabeditor->vec, &ptr, &size, &stride);
      
      for(i=0, j= current * stride; i < send && current+i < size; i++, j+=stride)
        fts_set_float(a + i + 1, ptr[j]);      
      
      if(i+1 < send) send = i+1;
    }
    fts_client_send_message((fts_object_t *)tabeditor, sym_append_visibles, send+1, a);
    
    current+=send;
    n -= send;
  }
}

static void
tabeditor_append_pixels(tabeditor_t *tabeditor, int deltax, int deltap)
{
  int i, j;
  fts_atom_t a[CLIENT_BLOCK_SIZE];
  int vecsize = tabeditor_get_size( tabeditor);
  float k = (float)(1.0/tabeditor->zoom);
  int n = (deltax > 0)? deltax : -deltax;
  int num_val = n*2;
  
  int current = (deltax < 0)? tabeditor->vindex : (tabeditor->vindex + tabeditor->vsize-deltap);
  int start = (deltax < 0)? 0 : tabeditor->pixsize-deltax;
  
  if(current >= vecsize) current = vecsize-1;
  
  while(num_val > 0)
  {
    int send = (num_val > CLIENT_BLOCK_SIZE-2)? CLIENT_BLOCK_SIZE-2: num_val;
    
    fts_set_int(&a[0], start);
    
    if( tabeditor_is_ivec( tabeditor))
      for(i = 0, j=0; ((i < send-1)&&((int)(current+j*k) < vecsize)); i+=2)
      {
        fts_set_int(&a[i+1], ivec_get_max_value_in_range((ivec_t *)tabeditor->vec, (int)(current+k*j), (int)(current+k*(j+1))));
        fts_set_int(&a[i+2], ivec_get_min_value_in_range((ivec_t *)tabeditor->vec, (int)(current+k*j), (int)(current+k*(j+1))));
        j++;
      }     
    else
    {
      float *ptr;
      int size;
      int stride;
      
      fvec_vector(tabeditor->vec, &ptr, &size, &stride);
      
      for(i = 0, j = 0; (i < send-1 && ((int)(current + j * k) < size)); i+=2)
      {
        float min, max;
        fvec_get_min_max_in_range(ptr, size, stride, (int)(current + k * j), (int)(current + k * (j + 1)), &min, &max);
        
        fts_set_float(a + i + 1, max);	
        fts_set_float(a + i + 2, min);	
        j++;
      }
    }
    if(i < send-1) send = i;
    
    fts_client_send_message((fts_object_t *)tabeditor, sym_add_pixels, send+1, a);
    
    current+= (int)k*j;
    start+=j;
    num_val -= send;
  }
}

/*********************************************************
*
*  client methods
*
*/
void
tabeditor_upload_gui(tabeditor_t *this)
{
  if(this)
  {
    if(this->min_val != TABEDITOR_DEFAULT_MIN || this->max_val != TABEDITOR_DEFAULT_MAX)
    {
      fts_atom_t a[2];    
      fts_set_float(a, this->min_val);
      fts_set_float(a+1, this->max_val);
      fts_client_send_message((fts_object_t *)this, fts_s_range, 2, a);
    }
  }
}

void
tabeditor_upload_interval(tabeditor_t *this, int start, int end)
{
  int vecsize = tabeditor_get_size( this);
  if(vecsize > this->vsize)
    if(vecsize > this->win_size)
      this->vsize = this->win_size;
    else
      this->vsize = vecsize;
  else
    this->vsize = vecsize;
  
  fts_client_send_message((fts_object_t *)this, sym_reset_editor, 0, 0);
  
  if(this->zoom > 0.5)
    tabeditor_append_visibles(this, start, end);
  else

    tabeditor_insert_pixels(this, start, (end-start));
}

static fts_method_status_t
tabeditor_end_edit(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_client_send_message(o, sym_end_edit, 0, 0);
  
  return fts_ok;
}

static fts_method_status_t
tabeditor_get_to_client(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  tabeditor_t *this = (tabeditor_t *)o;
  
  tabeditor_upload_gui(this);
  
  if(ac > 1 && fts_is_number(at))
  {
    int first =  fts_get_number_int(at);
    int last = fts_get_number_int(at+1);
    tabeditor_append_visibles( this, first, last);
  }
  else
    tabeditor_send_visibles(this);
  
  return fts_ok;
}

static fts_method_status_t
tabeditor_get_pixels_to_client(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  tabeditor_t *this = (tabeditor_t *)o;  
  
  tabeditor_upload_gui(this);
  
  if(ac > 1 && fts_is_number(at))
  {
    int deltax = fts_get_number_int(at);
    int deltap = fts_get_number_int(at+1);
    tabeditor_append_pixels(this, deltax, deltap);
  }
  else
    tabeditor_send_pixels(this);
  
  return fts_ok;
}

static fts_method_status_t
tabeditor_set_visible_window(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  tabeditor_t *this = (tabeditor_t *)o;
  
  if(ac > 1 && fts_is_number(at))
  {
    this->vsize = fts_get_number_int(at);
    this->vindex = fts_get_number_int(at+1);	  
    this->win_size = fts_get_number_int(at+2);
    this->zoom = (float)fts_get_number_float(at+3);	  
    this->pixsize = fts_get_number_int(at+4);	  
  }
  
  return fts_ok;
}

static fts_method_status_t
tabeditor_upload(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  tabeditor_t *this = (tabeditor_t *)o;
  /* if size changed (ex. changing from col to unwrap) update vsize */
  int vecsize = tabeditor_get_size( this);
  if(vecsize > this->vsize)
    if(vecsize > this->win_size)
      this->vsize = this->win_size;
    else
      this->vsize = vecsize;
  else
    this->vsize = vecsize;
    
  fts_client_send_message(o, sym_reset_editor, 0, 0);
  
  if(this->zoom > 0.5)
    tabeditor_send_visibles(this);
  else
  {
    this->pixsize = (int) (this->vsize * this->zoom);      
    tabeditor_send_pixels(this);
  }
  return fts_ok;
}



static fts_method_status_t
tabeditor_copy_by_client_request(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
        this->copy = fts_object_create(ivec_class, 1, at + 1);
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
        this->copy = fts_object_create(fmat_class, 1, at + 1);
      else
        fmat_set_m((fmat_t *)this->copy, size);
      
      src = fmat_get_ptr( ((fvec_t *)this->vec)->fmat);
      dst = fmat_get_ptr( (fmat_t *)this->copy);
      
      if(start + size > this_size)
        size = this_size - start;
      
      for(i=0; i<size; i++)
        dst[i] = src[start + i];
    }
  }
  
  return fts_ok;
}

static fts_method_status_t
tabeditor_cut_by_client_request(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  tabeditor_t *this = (tabeditor_t *)o;
  int v_size = fts_get_int(at);
  int pix_size = fts_get_int(at + 1);
  int start = fts_get_int(at + 2);
  int copy_size, size;
  int i;
  
  tabeditor_copy_by_client_request(o, NULL, ac - 2, at + 2, fts_nix);
  if( tabeditor_is_ivec( this))
  {
    int *ptr;
    copy_size = ivec_get_size( (ivec_t *)this->copy);
    size = ivec_get_size( (ivec_t *)this->vec);
    ptr = ivec_get_ptr( (ivec_t *)this->vec);
    
    for(i = start; i < size-copy_size; i++)
      ptr[i] = ptr[i + copy_size];
    
    for(i = size-copy_size; i < size; i++)
      ptr[i] = 0; 
  }  
  else
  {
    float *ptr;
    int size;
    int stride;
    
    fvec_vector(this->vec, &ptr, &size, &stride);    
    copy_size = fmat_get_m( (fmat_t *)this->copy);
    
    for(i = start; i < size-copy_size; i++)
      ptr[i] = ptr[i + copy_size];
    
    for(i = size-copy_size; i < size; i++)
      ptr[i] = 0.0;    
  }
  
  this->vsize = v_size;
  this->pixsize = pix_size;
  
  tabeditor_upload_interval(this, start, size);
  
  return fts_ok;
}

static fts_method_status_t
tabeditor_paste_by_client_request(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
      this_size = fmat_get_m((fmat_t *)this->vec);
      copy_size = fmat_get_m((fmat_t *)this->copy);
      
      src = fmat_get_ptr((fmat_t *)this->copy);
      dst = fmat_get_ptr((fmat_t *)this->vec);
      
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
    
    fts_object_set_state_dirty( this->vec);
  }  
  
  return fts_ok;
}

static fts_method_status_t
tabeditor_insert_by_client_request(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
      copy_size = fmat_get_m( (fmat_t *)this->copy);
      fmat_set_m( (fmat_t *)this->vec, fmat_get_m( (fmat_t *)this->vec) + copy_size);
      this_size = fmat_get_m( (fmat_t *)this->vec);
      
      src = fmat_get_ptr((fmat_t *)this->copy);
      dst = fmat_get_ptr((fmat_t *)this->vec);
      
      for(i=this_size - 1; i>=start; i--)
        dst[i] = dst[i - copy_size];
      
      for(i=0; i < copy_size; i++)
        dst[start + i] = src[i];	  
    }
    
    this->vsize = v_size;
    this->pixsize = pix_size;
    
    if( this->zoom < 0.5) tabeditor_send_pixels( this);
    tabeditor_send_visibles(this);
    
    fts_object_set_state_dirty( this->vec);
  }
  
  return fts_ok;
}

/********************************************************************
*
*   user methods
*
*/

static fts_method_status_t
tabeditor_set_elements(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
        fvec_set_from_atoms((fvec_t *)this->vec, onset, ac - 1, at + 1);
        
	      if( fvec_editor_is_open( (fvec_t *)this->vec))
          tabeditor_insert_append( this, onset, ac, at);
	    }
      
      fts_object_set_state_dirty( this->vec);
    }
  }
  
  return fts_ok;
}

static fts_method_status_t
tabeditor_interpolate_by_client_request(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  tabeditor_t *this = (tabeditor_t *)o;
  
  if(ac == 4 && fts_is_number(at))
  {
    int start = fts_get_number_int(at);
    int end = fts_get_number_int(at+1);
    int buffsize = end-start;
    fts_atom_t buffer[MAX_BLOCK_SIZE];
    int i;
    
    if( tabeditor_is_ivec( this))
    {
      int startVal = fts_get_number_int(at+2);
      int endVal = fts_get_number_int(at+3);
      double coeff;
      int n, current, setted, count;
      
      if (startVal != endVal) 
        coeff = ((double)(startVal - endVal))/(end - start);
      else coeff = 0;
      
      if(start < 0)
      {
        startVal = startVal+start*coeff;
        start = 0;
      }
      
      n = buffsize;
      current = start;
      count = 0;
      while(n > 0)
      {
        setted = (n > MAX_BLOCK_SIZE) ? MAX_BLOCK_SIZE : n;
        
        for (i = 0; i < setted; i+=1)
          fts_set_int(buffer+i, CUT_TO_BOUNDS( (int)(startVal-(count+i)*coeff), this->min_val, this->max_val));
        
        ivec_set_with_onset_from_atoms((ivec_t *)this->vec, current, setted, buffer);
        
        current+=setted;
        n-=setted;
        count+=setted;
      }
      tabeditor_upload_interval(this, start, end);
      
    }
    else
    {
      float startVal = (float)fts_get_number_float(at+2);
      float endVal = (float)fts_get_number_float(at+3);
      double coeff;
      int n, current, setted, count;
      
      if (startVal != endVal) 
        coeff = ((double)(startVal - endVal))/(end - start);
      else coeff = 0;
      
      if(start < 0)
      {
        startVal = startVal+start*coeff;
        start = 0;
      }
      
      n = buffsize;
      current = start;
      count = 0;
      while(n > 0)
      {
        setted = (n > MAX_BLOCK_SIZE) ? MAX_BLOCK_SIZE : n;
        
        for (i = 0; i < setted; i+=1)
          fts_set_float(buffer+i, CUT_TO_BOUNDS( (float)(startVal-(count+i)*coeff), this->min_val, this->max_val));
      
        fvec_set_from_atoms((fvec_t *)this->vec, current, setted, buffer);
        
        current+=setted;
        n-=setted;
        count+=setted;
      }
      tabeditor_upload_interval(this, start, end);
    }
  }
  return fts_ok;
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
  if( tabeditor_is_ivec( tabeditor))
    return ivec_get_size((ivec_t *)tabeditor->vec);
  else
    return fvec_get_size((fvec_t *)tabeditor->vec);
}

/* gui editor data */
static fts_method_status_t
tabeditor_set_range(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  tabeditor_t *this = (tabeditor_t *)o;
  
	if(ac == 2 && fts_is_float(at) && fts_is_float(at+1))
	{
		float min_val = fts_get_float(at);
		float max_val =  fts_get_float(at+1);
    if(this->min_val != min_val || this->max_val != max_val)
    {
      this->min_val = min_val;
			this->max_val = max_val;
    }
  }	
  
  return fts_ok;
}	

static fts_method_status_t
tabeditor_set_range_from_client(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  tabeditor_t *this = (tabeditor_t *)o;
  
  if(ac == 2 && fts_is_float(at) && fts_is_float(at+1))
	{
		float min_val = fts_get_float(at);
		float max_val =  fts_get_float(at+1);
    if(this->min_val != min_val || this->max_val != max_val)
    {
      this->min_val = min_val;
			this->max_val = max_val;
      fts_object_set_dirty((fts_object_t *)this->vec);
      tabeditor_upload_gui(this);
    }
  }	  
  return fts_ok;
}

void 
tabeditor_dump_gui(tabeditor_t *this, fts_dumper_t *dumper)
{
  if(this->min_val != TABEDITOR_DEFAULT_MIN || this->max_val != TABEDITOR_DEFAULT_MAX)
  {
    fts_atom_t a[3];  
    fts_set_symbol(a, fts_s_range);
    fts_set_float(a + 1, this->min_val);
    fts_set_float(a + 2, this->max_val);
    fts_dumper_send(dumper, fts_s_editor, 3, a);
  }
}
/*********************************************************
*
*  class
*
*/
static fts_method_status_t
tabeditor_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  tabeditor_t *this = (tabeditor_t *)o;
  
  this->opened = 0; 
  this->vsize = 0; 
  this->win_size = 0;
  this->vindex = 0;
  this->zoom = 1.0;
  this->pixsize = 1;
  
  this->min_val = TABEDITOR_DEFAULT_MIN;
  this->max_val = TABEDITOR_DEFAULT_MAX;
  
  if(ac == 1 && fts_is_object(at))
  {
    this->vec = fts_get_object( at);
    this->type = (fts_object_get_class(this->vec) == ivec_class);
  }
  else
  {
    this->vec = 0; 
    this->type = -1;
  }
  
  this->copy = 0;
  
  return fts_ok;
}

static fts_method_status_t
tabeditor_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  tabeditor_t *this = (tabeditor_t *)o;
  
  fts_client_send_message(o, fts_s_destroyEditor, 0, 0);
  
  if(this->copy)
    fts_object_destroy((fts_object_t *)this->copy);
  
  return fts_ok;
}

static void
tabeditor_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(tabeditor_t), tabeditor_init, tabeditor_delete);
  
  fts_class_message_varargs(cl, fts_s_upload, tabeditor_upload);
  
  fts_class_message_varargs(cl, fts_new_symbol("set_from_client"), tabeditor_set_elements);
  fts_class_message_varargs(cl, fts_new_symbol("get_from_client"), tabeditor_get_to_client);
  fts_class_message_varargs(cl, fts_new_symbol("get_pixels_from_client"), tabeditor_get_pixels_to_client);
  fts_class_message_varargs(cl, fts_new_symbol("set_visible_window"), tabeditor_set_visible_window);
  fts_class_message_varargs(cl, fts_new_symbol("end_edit"), tabeditor_end_edit);
  fts_class_message_varargs(cl, fts_new_symbol("copy_from_client"), tabeditor_copy_by_client_request);
  fts_class_message_varargs(cl, fts_new_symbol("paste_from_client"), tabeditor_paste_by_client_request);
  fts_class_message_varargs(cl, fts_new_symbol("cut_from_client"), tabeditor_cut_by_client_request);
  fts_class_message_varargs(cl, fts_new_symbol("insert_from_client"), tabeditor_insert_by_client_request);
  fts_class_message_varargs(cl, fts_new_symbol("interpolate"), tabeditor_interpolate_by_client_request);
  
  fts_class_message_varargs(cl, fts_s_range, tabeditor_set_range);
  fts_class_message_varargs(cl, fts_new_symbol("change_range"), tabeditor_set_range_from_client);
}

/********************************************************************
*
*  config
*
*/

FTS_MODULE_INIT(tabeditor)
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
  sym_reset_editor = fts_new_symbol("resetEditor");
  
  sym_set_visible_size = fts_new_symbol("setVisibleSize");
  
  tabeditor_type = fts_class_install(tabeditor_symbol, tabeditor_instantiate);
}










