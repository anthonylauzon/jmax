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
#include <float.h>
#include <fts/packages/data/bpf.h>

fts_symbol_t bpf_symbol = 0;
fts_class_t *bpf_type = 0;

static fts_symbol_t sym_addPoint = 0;
static fts_symbol_t sym_removePoints = 0;
static fts_symbol_t sym_setPoints = 0;

/************************************************************
*
*  bpf
*
*/

#define BPF_BLOCK_SIZE 16

#define bpf_set_editor_open(b) ((b)->opened = 1)
#define bpf_set_editor_close(b) ((b)->opened = 0)
#define bpf_editor_is_open(b) ((b)->opened)

static void
set_size(bpf_t *bpf, int size)
{
  int alloc = bpf->alloc;
  
  if(size > alloc)
  {
    alloc += BPF_BLOCK_SIZE;
    
    while(alloc < size)
      alloc += BPF_BLOCK_SIZE;
    
    bpf->points = (bp_t *)fts_realloc(bpf->points, sizeof(bp_t) * alloc);
    bpf->alloc = alloc;
  }
  
  bpf->size = size;
}

void
bpf_clear(bpf_t *bpf)
{
  bpf->size = 0;
}

void
bpf_append_point(bpf_t *bpf, double time, double value)
{
  int size = bpf->size;
  
  set_size(bpf, size + 1);
  
  if(size > 0)
  {
    double duration = bpf_get_time(bpf, size - 1);
    
    if(time <= duration)
    {
      time = duration;
      
      /* set jump for previous point */
      bpf->points[size - 1].slope = DBL_MAX;
    }
    else
    {
      /* set slope for previous point */
      bpf->points[size - 1].slope = 
      (value - bpf->points[size - 1].value) / 
      (time - bpf->points[size - 1].time);
    }
  }
  
  /* set point */
  bpf->points[size].time = time;
  bpf->points[size].value = value;
  bpf->points[size].slope = 0.0;
}

void
bpf_set_point(bpf_t *bpf, int index, double time, double value)
{
  int size = bpf->size;
  
  if(index < size)
  {
    double dt;
    
    /* set slope of point before */
    if(index > 0)
    {
      dt = time - bpf->points[index - 1].time;
      
      if(dt == 0.0)
        bpf->points[index - 1].slope = DBL_MAX;
      else
        bpf->points[index - 1].slope = (value - bpf->points[index - 1].value) / dt;
    }
    
    /* set point */
    bpf->points[index].time = time;
    bpf->points[index].value = value;
    
    if(index < size - 1)
    {
      dt = bpf->points[index + 1].time - bpf->points[index].time;
      
      if(dt == 0.0)
        bpf->points[index].slope = DBL_MAX;
      else
        bpf->points[index].slope = (bpf->points[index + 1].value - value) / dt;
    }
    else
      bpf->points[index].slope = 0.0;
  }
}

void
bpf_insert_point(bpf_t *bpf, double time, double value)
{
  int size = bpf->size;
  
  if(time > bpf->points[size - 1].time)
    bpf_append_point(bpf, time, value);
  else
  {
    int i, j;
    
    for(i=0; i<size; i++)
    {
      if(bpf->points[i].time > time)
        break;
    }
    
    set_size(bpf, size + 1);
    
    /* kick points to the back */
    for(j=size; j>i; j--)
      bpf->points[j] = bpf->points[j - 1];
    
    /* set point */
    bpf_set_point(bpf, i, time, value);
  }
}

void
bpf_remove_points(bpf_t *bpf, int index, int n)
{
  int size = bpf->size;
  
  if(index < size)
  {
    if(n > size - index)
      n = size - index;
    
    if(size <= n)
      bpf->size = 0;
    else
    {
      int i;
      
      size -= n;
      
      if(index == size)
        bpf->points[size - 1].slope = 0.0; /* erase last point */
      else
      {
        double dt;
        
        /* fill the hole */
        for(i=index; i<size; i++)
          bpf->points[i] = bpf->points[i + n];
        
        dt = bpf->points[index].time - bpf->points[index - 1].time;
        
        /* set slope of previous point */
        if(dt == 0.0)
          bpf->points[index - 1].slope = DBL_MAX;
        else
          bpf->points[index - 1].slope = (bpf->points[index].value - bpf->points[index - 1].value) / dt;
      }
      
      bpf->size = size;
    }
  }
}

void
bpf_simplify(bpf_t *bpf, double time, double value)
{
  int size = bpf->size;
  int i = 0;
  
  while(i < size-1)
  {
    if((bpf->points[i].time == bpf->points[i + 1].time && bpf->points[i].value == bpf->points[i + 1].value) || 
       (bpf->points[i].time == bpf->points[i + 2].time && bpf->points[i].value == bpf->points[i + 2].value) ||
       (bpf->points[i].slope == bpf->points[i + 1].slope))
      bpf_remove_points(bpf, i + 1, 1);
    else
      i++;
  }
}

void
bpf_copy(bpf_t *org, bpf_t *copy)
{
  int size = bpf_get_size(org);
  int i;
  
  set_size(copy, size);
  
  for(i=0; i<size; i++)
    copy->points[i] = org->points[i];
}

static void
bpf_copy_function(const fts_object_t *from, fts_object_t *to)
{
  bpf_copy((bpf_t *)from, (bpf_t *)to);
}

static int
bpf_equals(bpf_t *a, bpf_t *b)
{
  int a_n = bpf_get_size(a);
  int b_n = bpf_get_size(b);
  
  if(a_n == b_n)
  {
    int i;
    
    for(i=0; i<a_n; i++)
    {
      if(!data_float_equals(bpf_get_time(a, i), bpf_get_time(b, i)) ||
         !data_float_equals(bpf_get_value(a, i), bpf_get_time(b, i)))
        return 0;
    }
    
    return 1;
  }
  
  return 0;
}

static void
bpf_array_function(fts_object_t *o, fts_array_t *array)
{
  bpf_t *this = (bpf_t *)o;
  int size = bpf_get_size(this);
  int onset = fts_array_get_size(array);
  fts_atom_t *atoms;
  int i;
  
  fts_array_set_size(array, onset + size * 2);
  atoms = fts_array_get_atoms(array) + onset;
  
  for(i=0; i<size; i++)
  {
    fts_set_float(atoms + 2 * i, bpf_get_time(this, i));
    fts_set_float(atoms + 2 * i + 1, bpf_get_value(this, i));
  }
}

double
bpf_get_interpolated(bpf_t *this, double time)
{
  int size = bpf_get_size(this);
  
  if(time < bpf_get_time(this, 0))
  {
    this->index = 0;
    return bpf_get_value(this, 0);
  }
  else if(time >= bpf_get_duration(this))
  {
    this->index = size - 1;
    return bpf_get_target(this);
  }
  else
  {
    int index = this->index;
    
    if(index > size - 2)
      index = size - 2;
    
    /* search index */
    if(time >= bpf_get_time(this, index + 1))
    {
      index++;
      
      while(time >= bpf_get_time(this, index + 1))
        index++;
    }
    else if(time < bpf_get_time(this, index))
    {
      index--;
      
      while(time < bpf_get_time(this, index))
        index--;
    }
    else if(bpf_get_slope(this, index) == DBL_MAX)
    {
      index++;
      
      while(bpf_get_slope(this, index) == DBL_MAX)
        index++;
    }
    
    /* remember new index */
    this->index = index;
    
    /* return interpolated value */
    return bpf_get_value(this, index) + (time - bpf_get_time(this, index)) * bpf_get_slope(this, index);
  }
}

#define BPF_CLIENT_BLOCK_SIZE 64

static void
bpf_set_client(bpf_t *bpf)
{
  if(bpf_editor_is_open(bpf))
  {
    fts_atom_t a[2 * BPF_CLIENT_BLOCK_SIZE];
    int size = bpf_get_size(bpf);
    int sent = 0;
    int i;
    
    while(size > 0)
    {
      int n = (size > BPF_CLIENT_BLOCK_SIZE)? BPF_CLIENT_BLOCK_SIZE: size;
      
      for(i=0; i<n; i++)
      {
        fts_set_float(a + 2 * i, bpf_get_time(bpf, sent + i));
        fts_set_float(a + 2 * i + 1, bpf_get_value(bpf, sent + i));
      }
      
      if(!sent)
        fts_client_send_message((fts_object_t *)bpf, fts_s_set, 2 * n, a);
      else
        fts_client_send_message((fts_object_t *)bpf, fts_s_append, 2 * n, a);
      
      sent += n;
      size -= n;
    }
  }
}

/************************************************************
*
*  user methods
*
*/

static fts_method_status_t
_bpf_append(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  bpf_t *this = (bpf_t *)o;
  
  if(ac > 0)
  {
    double onset_time = bpf_get_duration(this);
    double time = 0.0;
    double value = 0.0;
    int i;
    
    if(ac & 1)
    {
      if(fts_is_number(at))
        value = fts_get_number_float(at);
      
      bpf_append_point(this, onset_time, value);
      
      /* skip first value */
      ac--;
      at++;
    }
    
    for(i=0; i<ac; i+=2)
    {
      if(fts_is_number(at + i))
        time = onset_time + fts_get_number_float(at + i);
      else
        time = onset_time;
      
      if(fts_is_number(at + i + 1))
        value = fts_get_number_float(at + i + 1);
      else
        value = 0.0;
      
      bpf_append_point(this, time, value);
    }      
    
    bpf_set_client(this);
    fts_object_set_state_dirty(o);
    
    fts_set_object(ret, o);
  }

  return fts_ok;
}

static fts_method_status_t
_bpf_set(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  bpf_t *this = (bpf_t *)o;
  
  this->size = 0;
  
  if(ac > 0)
  {
    double time = 0.0;
    double value = 0.0;
    int i;
    
    if(ac & 1)
    {
      set_size(this, 1);
      
      if(fts_is_number(at))
        value = fts_get_number_float(at);
      else
        value = 0.0;
      
      this->points[0].time = 0.0;
      this->points[0].value = value;
      this->points[0].slope = 0.0;
      
      /* skip value */
      ac--;
      at++;
    }
    
    for(i=0; i<ac; i+=2)
    {
      if(fts_is_number(at + i))
        time = fts_get_number_float(at + i);
      else
        time = 0.0;
      
      if(fts_is_number(at + i + 1))
        value = fts_get_number_float(at + i + 1);
      else
        value = 0.0;
      
      bpf_append_point(this, time, value);
    }
  }
  
  bpf_set_client(this);
  fts_object_set_state_dirty(o);
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
_bpf_insert(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  bpf_t *this = (bpf_t *)o;
  
  if(ac > 1)
  {
    int i;
    
    for(i=0; i<ac-1; i+=2)
    {
      double time = fts_get_float(at + i);
      double value = fts_get_float(at + i + 1);
      
      bpf_insert_point(this, time, value);
    }
    
    bpf_set_client(this);
    fts_object_set_state_dirty(o);
    fts_set_object(ret, o);
  }
  
  return fts_ok;
}

static fts_method_status_t
_bpf_clear(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  bpf_t *this = (bpf_t *)o;
  
  this->size = 1;
  
  this->points[0].time = 0.0;
  this->points[0].value = 0.0;
  
  bpf_set_client(this);
  fts_object_set_state_dirty(o);
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
_bpf_set_from_bpf(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  bpf_t *this = (bpf_t *)o;
  bpf_t *in = (bpf_t *)fts_get_object(at);
  
  bpf_copy(in, this);
  
  bpf_set_client(this);
  fts_object_set_state_dirty(o);
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
_bpf_set_from_fmat(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  bpf_t *this = (bpf_t *)o;
  fmat_t *fmat = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(fmat);
  int n = fmat_get_n(fmat);
  
  if(m > 0)
  {
    float *ptr = fmat_get_ptr(fmat);
    
    /* clear bpf */
    this->size = 0;
        
    if(n > 1)
    {
      int i;
      
      for(i=0; i<m*n; i+=n)
        bpf_append_point(this, ptr[i], ptr[i + 1]);
    }
    else
    {
      double time = 0.0;
      double last_value = ptr[0] - 1.0;
      int i;
      
      for(i=0; i<m; i++)
      {
        double value = ptr[i];
        
        if(value != last_value)
          bpf_append_point(this, time, value);
        
        time += 1.0;
        last_value = value;
      }
    }
  }
  else
    bpf_clear(this);
  
  bpf_set_client(this);
  fts_object_set_state_dirty(o);
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
_bpf_get_interpolated(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  bpf_t *this = (bpf_t *)o;
  double time = fts_get_number_float(at);
  
  fts_set_float(ret, bpf_get_interpolated(this, time));
  
  return fts_ok;
}

static fts_method_status_t
_bpf_get_duration(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  bpf_t *this = (bpf_t *)o;
  
  fts_set_float(ret, (float)bpf_get_duration(this));  
  
  return fts_ok;
}

static fts_method_status_t
_bpf_set_duration(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  bpf_t *this = (bpf_t *)o;
  double new = fts_get_number_float(at);
  double old = bpf_get_duration(this);
  
  if(new > 0.0 && old > 0.0)
  {
    double scale = new / old;
    int i;
    
    for(i=0; i<bpf_get_size(this); i++)
      this->points[i].time *= scale;
  }
  
  bpf_set_client(this);
  fts_object_set_state_dirty(o);
  
  return fts_ok;
}

static fts_method_status_t
_bpf_get_size(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  bpf_t *this = (bpf_t *)o;
  
  fts_set_int(ret, bpf_get_size(this));
  
  return fts_ok;
}

static fts_method_status_t
_bpf_print(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  bpf_t *this = (bpf_t *)o;
  int size = this->size;
  fts_bytestream_t* stream = fts_get_default_console_stream();
  
  if(ac > 0 && fts_is_object(at))
    stream = (fts_bytestream_t *)fts_get_object(at);
  
  if(size == 0)
    fts_spost(stream, "<empty bpf>\n", size);
  else if (size == 1)
    fts_spost(stream, "<bpf of 1 point: @%.7g, %.7g>\n", this->points[0].time, this->points[0].value);
  else
  {
    int i;
    
    fts_spost(stream, "<bpf of %d points, %.7g msec>\n", size, this->points[size - 1].time);
    fts_spost(stream, "{\n");
    
    for(i=0; i<this->size; i++)
      fts_spost(stream, "  @%.7g, %.7g\n", this->points[i].time, this->points[i].value);
    
    fts_spost(stream, "}\n");
  }
  
  return fts_ok;
}

/************************************************************
*
*  client methods
*
*/

static fts_method_status_t
bpf_add_point_by_client_request(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  bpf_t *this = (bpf_t *)o;
  double time = fts_get_float(at + 1);
  double value = fts_get_float(at + 2);
  
  bpf_insert_point(this, time, value);
  fts_client_send_message(o, sym_addPoint, ac, at);
  
  fts_object_set_state_dirty(o);
  
  return fts_ok;
}

static fts_method_status_t
bpf_remove_points_by_client_request(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  bpf_t *this = (bpf_t *)o;
  int index = fts_get_int(at + 0);
  int size = fts_get_int(at + 1);
  
  bpf_remove_points(this, index, size);
  fts_client_send_message(o, sym_removePoints, ac, at);
  
  fts_object_set_state_dirty(o);
  
  return fts_ok;
}

static fts_method_status_t
bpf_set_points_by_client_request(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  bpf_t *this = (bpf_t *)o;
  int index = fts_get_int(at);
  int n = (ac - 1) / 2;
  int i;
  
  for(i=0; i<n; i++)
  {
    double time = fts_get_float(at + 2 * i + 1);
    double value = fts_get_float(at + 2 * i + 2);
    
    bpf_set_point(this, index + i, time, value);
  }
  
  fts_client_send_message(o, sym_setPoints, ac, at);
  
  fts_object_set_state_dirty(o);
  
  return fts_ok;
}

static fts_method_status_t
bpf_open_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  bpf_t *this = (bpf_t *)o;
  
  bpf_set_editor_open(this);
  fts_client_send_message(o, fts_s_openEditor, 0, 0);
  
  bpf_set_client(this);
  
  return fts_ok;
}

static fts_method_status_t 
bpf_close_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  bpf_t *this = (bpf_t *) o;
  
  if(bpf_editor_is_open(this))
  {
    bpf_set_editor_close(this);
    fts_client_send_message((fts_object_t *)this, fts_s_closeEditor, 0, 0);  
  }
  
  return fts_ok;
}

static fts_method_status_t
bpf_destroy_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  bpf_t *this = (bpf_t *)o;
  
  bpf_set_editor_close(this);
  
  return fts_ok;
}

/************************************************************
*
*  system methods
*
*/

static fts_method_status_t
bpf_dump_state(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  bpf_t *this = (bpf_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  int size = bpf_get_size(this);
  fts_message_t *mess = fts_dumper_message_get(dumper, fts_s_set);
  int i;
  
  for(i=0; i<size; i++)
  {
    float time  = (float)this->points[i].time;
    float value = (float)this->points[i].value;
    
    fts_message_append_float(mess, time);
    fts_message_append_float(mess, value);
  }
  
  fts_dumper_message_send(dumper, mess);
  
  return fts_ok;
}

/************************************************************
*
*  class
*
*/

static fts_method_status_t
bpf_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{ 
  bpf_t *this = (bpf_t *)o;
  
  this->points = 0;
  this->alloc = 0;
  this->size = 0;
  this->opened = 0;
  
  if(ac > 0)
    _bpf_set(o, NULL, ac, at, fts_nix);
  else
  {
    set_size(this, 1);
    
    this->points[0].time = 0.0;
    this->points[0].value = 0.0;
  }
  
  return fts_ok;
}

static fts_method_status_t
bpf_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{ 
  bpf_t *this = (bpf_t *)o;
  
  fts_client_send_message(o, fts_s_destroyEditor, 0, 0);
  
  if(this->points)
    fts_free(this->points);
  
  return fts_ok;
}

static void
bpf_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(bpf_t), bpf_init, bpf_delete);  
  
  fts_class_set_copy_function(cl, bpf_copy_function);
  fts_class_set_array_function(cl, bpf_array_function);
  
  fts_class_message_varargs(cl, fts_s_name, fts_object_name);
  fts_class_message_varargs(cl, fts_s_persistence, fts_object_persistence);
  fts_class_message_varargs(cl, fts_s_dump_state, bpf_dump_state);
  
  fts_class_message_varargs(cl, fts_s_print, _bpf_print);
  
  fts_class_message_varargs(cl, fts_s_openEditor, bpf_open_editor);
  fts_class_message_varargs(cl, fts_s_closeEditor, bpf_close_editor); 
  fts_class_message_varargs(cl, fts_s_destroyEditor, bpf_destroy_editor);
  
  fts_class_message_varargs(cl, fts_new_symbol("add_point"), bpf_add_point_by_client_request);
  fts_class_message_varargs(cl, fts_new_symbol("remove_points"), bpf_remove_points_by_client_request);
  fts_class_message_varargs(cl, fts_new_symbol("set_points"), bpf_set_points_by_client_request);
  
  fts_class_message_varargs(cl, fts_s_clear, _bpf_clear);
  fts_class_message_varargs(cl, fts_s_set, _bpf_set);
  fts_class_message_varargs(cl, fts_s_append, _bpf_append);
  fts_class_message_varargs(cl, fts_s_insert, _bpf_insert);
  fts_class_message(cl, fts_s_set, bpf_type, _bpf_set_from_bpf);
  fts_class_message(cl, fts_s_set, fmat_class, _bpf_set_from_fmat);
  
  fts_class_message_number(cl, fts_s_get_element, _bpf_get_interpolated);
  
  fts_class_message_void(cl, fts_new_symbol("duration"), _bpf_get_duration);
  fts_class_message_number(cl, fts_new_symbol("duration"), _bpf_set_duration);
  
  fts_class_message_void(cl, fts_new_symbol("size"), _bpf_get_size);
  
  fts_class_inlet_bang(cl, 0, data_object_output);
  
  fts_class_inlet_thru(cl, 0);
  fts_class_outlet_thru(cl, 0);
  
  fts_class_doc(cl, bpf_symbol, "<num: time> <num: value> ...", "bpf of time tagged points");
  fts_class_doc(cl, fts_s_clear, NULL, "erase all points");
  fts_class_doc(cl, fts_s_set, "<num: time> <num: value> ...", "set bpf from a list of points");
  fts_class_doc(cl, fts_s_set, "<bpf: bpf>", "set bpf from another bpf");
  fts_class_doc(cl, fts_s_set, "[<num: value>] <fmat: fmat>", "set bpf from an fmat");
	fts_class_doc(cl, fts_s_append, "[<num: value>] <num: time> <num: value> ...", "append list of points to the bpf");
	fts_class_doc(cl, fts_s_insert, "<num: time> <num: value> ...", "insert list of points to the bpf");
	fts_class_doc(cl, fts_new_symbol("duration"), NULL, "get the duration of the bpf");
	fts_class_doc(cl, fts_new_symbol("duration"), "<num: duration>", "set the duration of the bpf by rescaling the time axis");
	fts_class_doc(cl, fts_s_size, NULL, "get number of points");
	fts_class_doc(cl, fts_s_print, NULL, "print list of points");
}

void
bpf_config(void)
{
  bpf_symbol = fts_new_symbol("bpf");
  
  sym_addPoint = fts_new_symbol("addPoint");
  sym_removePoints = fts_new_symbol("removePoints");
  sym_setPoints = fts_new_symbol("setPoints");
  
  bpf_type = fts_class_install(bpf_symbol, bpf_instantiate);
}

/** EMACS **
* Local variables:
* mode: c
* c-basic-offset:2
* End:
*/
