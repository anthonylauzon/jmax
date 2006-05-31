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
#include "mateditor.h"
#include <limits.h>

#include <stdlib.h>

#define CLIENT_BLOCK_SIZE 256
#define MAX_BLOCK_SIZE 8192

static fts_symbol_t sym_text = 0;

fts_symbol_t mateditor_symbol = 0;
fts_class_t *mateditor_type = 0;

/********************************************************
*
*  utility functions
*
*/
#define mateditor_is_fmat(t) ((t)->type)

int mateditor_get_m( mateditor_t *mateditor)
{
  if( mateditor_is_fmat( mateditor))
    return fmat_get_m((fmat_t *)mateditor->mat);
  else
    return mat_get_m((mat_t *)mateditor->mat);
}

int mateditor_get_n( mateditor_t *mateditor)
{
  if( mateditor_is_fmat( mateditor))
    return fmat_get_n((fmat_t *)mateditor->mat);
  else
    return mat_get_n((mat_t *)mateditor->mat);
}

/*********************************************************
*
*  client utils
*
*/


/*void
mateditor_upload_gui(mateditor_t *this)
{
}

static fts_method_status_t
mateditor_get_to_client(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mateditor_t *this = (mateditor_t *)o;
  
  mateditor_upload_gui(this);
  
  if(ac > 1 && fts_is_number(at))
  {
    int first =  fts_get_number_int(at);
    int last = fts_get_number_int(at+1);
    tabeditor_append_visibles( this, first, last);
  }
  else
    tabeditor_send_visibles(this);
  
  return fts_ok;
}*/

#define MATEDITOR_CLIENT_BLOCK_SIZE 256

static void 
mateditor_upload_size(mateditor_t *self)
{
  fts_atom_t a[2];
  int m = mateditor_get_m(self);
  int n = mateditor_get_n(self);
  
  fts_set_int(a, m);
  fts_set_int(a+1, n);
  fts_client_send_message((fts_object_t *)self, fts_s_size, 2, a);
}

static void 
mateditor_upload_from_to_row(mateditor_t *self, int start_row, int end_row)
{
  fts_atom_t a[MATEDITOR_CLIENT_BLOCK_SIZE];  
  int n_cols = mateditor_get_n(self);
  int sent = 0;
  int data_size = (end_row-start_row)*n_cols;
  int ms = start_row;
  int ns = 0;
  int start_id = (ms*n_cols + ns);
  
  //post("mateditor_upload_from_to_row %d %d", start_row, end_row);
  
  fts_client_send_message((fts_object_t *)self, fts_s_start_upload, 0, 0);
  
  if(mateditor_is_fmat(self))
  {
    while( data_size > 0)
    {
      int i = 0;
      int n = (data_size > MATEDITOR_CLIENT_BLOCK_SIZE-2)? MATEDITOR_CLIENT_BLOCK_SIZE-2: data_size;
      
      /* starting row and column index */
      if( sent)
      {
        ms = start_row + sent/n_cols;
        ns = sent - (ms-start_row)*n_cols;
      }
      fts_set_int(a, ms);
      fts_set_int(a+1, ns);
      
      for(i=0; i < n ; i++)
        fts_set_float(&a[2+i], ((fmat_t *)self->mat)->values[start_id  + sent + i]);      
      
      fts_client_send_message((fts_object_t *)self, fts_s_set, n+2, a);
      
      sent += n;
      data_size -= n;
    }
  }
  fts_client_send_message((fts_object_t *)self, fts_s_end_upload, 0, 0);
}

static void 
mateditor_upload_from_index(mateditor_t *self, int row_id, int col_id, int size)
{
  fts_atom_t a[MATEDITOR_CLIENT_BLOCK_SIZE];  
  int n_cols = mateditor_get_n(self);
  int sent = 0;
  int data_size = size;
  int ms = row_id;
  int ns = col_id;
  int start_id = (ms*n_cols + ns);
    
  if(mateditor_is_fmat(self))
  {
    while( data_size > 0)
    {
      int i = 0;
      int n = (data_size > MATEDITOR_CLIENT_BLOCK_SIZE-2)? MATEDITOR_CLIENT_BLOCK_SIZE-2: data_size;
    
      /* starting row and column index */
      if( sent)
      {
        ms = sent/n_cols;
        ns = sent - ms*n_cols;
      }
      fts_set_int(a, ms);
      fts_set_int(a+1, ns);
      
      for(i=0; i < n ; i++)
        fts_set_float(&a[2+i], ((fmat_t *)self->mat)->values[start_id  + sent + i]);      
    
      fts_client_send_message((fts_object_t *)self, fts_s_set, n+2, a);
      
      sent += n;
      data_size -= n;
    }
  }
}

static void 
mateditor_upload_data(mateditor_t *self)
{  
  fts_client_send_message((fts_object_t *)self, fts_s_start_upload, 0, 0);
  /*&&&&*/
  /*mateditor_upload_from_index(self, 0, 0, mateditor_get_m(self) * mateditor_get_n(self));*/
  if(self->last_vis_row > 0)
    mateditor_upload_from_to_row(self, self->first_vis_row, self->last_vis_row);
  //mateditor_upload_from_to_row(self, 0, self->last_vis_row);
  fts_client_send_message((fts_object_t *)self, fts_s_end_upload, 0, 0);
}

static fts_method_status_t
mateditor_upload(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mateditor_t *this = (mateditor_t *)o;  
  
  mateditor_upload_size(this);
  mateditor_upload_data(this);
  
  return fts_ok;
}


static fts_method_status_t
mateditor_set_visible_range(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mateditor_t *this = (mateditor_t *)o;
  
  /*if(ac > 1 && fts_is_number(at))
  {
    int old_last_row = this->last_vis_row;
    this->last_vis_row = fts_get_number_int(at);
    
    post("mateditor mateditor_set_last_visible_row last_vis_row %d", this->last_vis_row);
    
    if(old_last_row < this->last_vis_row)
      mateditor_upload_from_to_row(this, old_last_row, this->last_vis_row);
  }*/
  
  if(ac > 1 && fts_is_number(at) && fts_is_number(at+1))
  {
    int old_last_row = this->last_vis_row;
    this->first_vis_row = fts_get_number_int(at);
    this->last_vis_row = fts_get_number_int(at+1);
    
    //post("mateditor mateditor_set_last_visible_row last_vis_row %d", this->last_vis_row);
    
    mateditor_upload_from_to_row(this, this->first_vis_row, this->last_vis_row);
  }
  
  return fts_ok;
}

/********************************************************************
*
*   user methods
*
*/

static fts_method_status_t
mateditor_set_from_list(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mateditor_t *self = (mateditor_t *)o;
  
  if(ac > 2 && fts_is_number(at) && fts_is_number(at + 1))
  {
    int m = mateditor_get_m(self);
    int n = mateditor_get_n(self);
    int i = fts_get_number_int(at);
    int j = fts_get_number_int(at + 1);
    int size = m * n;
    int onset;
    
    if(i > m)
      i = m;
    
    if(j > n)
      j = n;
    
    while(i < 0)
      i += m;
    
    while(j < 0)
      j += n;
    
    onset = i * n + j;
    if(onset + ac-2 > size)
      ac = size - onset;
    
    if(mateditor_is_fmat(self))
      fmat_set_from_atoms(((fmat_t *)self->mat), onset, 1, ac-2, at+2);
    else
      mat_set_with_onset_from_atoms(((mat_t *)self->mat), onset, ac-2, at+2);
    
    mateditor_upload_from_index(self, i, j, ac-2);
    
    fts_object_set_state_dirty( self->mat);
    
    fts_set_object(ret, o);
  }
  
  return fts_ok;
}

/* gui editor data */
/*void 
mateditor_dump_gui(mateditor_t *this, fts_dumper_t *dumper)
{
  if(this->min_val != TABEDITOR_DEFAULT_MIN || this->max_val != TABEDITOR_DEFAULT_MAX)
  {
    fts_atom_t a[3];  
    fts_set_symbol(a, fts_s_range);
    fts_set_float(a + 1, this->min_val);
    fts_set_float(a + 2, this->max_val);
    fts_dumper_send(dumper, fts_s_editor, 3, a);
  }
}*/
/*********************************************************
*
*  class
*
*/
static fts_method_status_t
mateditor_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mateditor_t *this = (mateditor_t *)o;
  
  this->opened = 0; 
  this->first_vis_row = 0; 
  this->last_vis_row = 0; 
  
  if(ac == 1 && fts_is_object(at))
  {
    this->mat = fts_get_object( at);
    this->type = (fts_object_get_class(this->mat) == fmat_class);
  }
  else
  {
    this->mat = 0; 
    this->type = -1;
  }

  return fts_ok;
}

static fts_method_status_t
mateditor_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{  
  fts_client_send_message(o, fts_s_destroyEditor, 0, 0);
    
  return fts_ok;
}

static void
mateditor_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(mateditor_t), mateditor_init, mateditor_delete);
  
  fts_class_message_varargs(cl, fts_s_set, mateditor_set_from_list);
  fts_class_message_varargs(cl, fts_s_upload, mateditor_upload);
  fts_class_message_varargs(cl, fts_new_symbol("set_visible_range"), mateditor_set_visible_range);
}

/********************************************************************
*
*  config
*
*/

FTS_MODULE_INIT(mateditor)
{
  sym_text = fts_new_symbol("text");
  mateditor_symbol = fts_new_symbol("mateditor");
    
  /*sym_set_visibles = fts_new_symbol("setVisibles");
  sym_append_visibles = fts_new_symbol("appendVisibles");*/
  
  mateditor_type = fts_class_install(mateditor_symbol, mateditor_instantiate);
}










