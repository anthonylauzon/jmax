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

#define IVEC_CLIENT_BLOCK_SIZE 256

static fts_symbol_t sym_text = 0;

fts_symbol_t ivec_symbol = 0;
fts_class_t *ivec_class = 0;

/********************************************************
 *
 *  utility functions
 *
 */

void
ivec_set_size(ivec_t *vec, int size)
{
  int i;

  if(size > vec->alloc)
    {
      vec->values = (int *)fts_realloc(vec->values, sizeof(int) * size);
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
ivec_set_with_onset_from_atoms(ivec_t *vec, int onset, int ac, const fts_atom_t *at)
{
  int size = ivec_get_size(vec);
  int i;
 
  if(onset + ac > size)
    ac = size - onset;
  
  for(i=0; i<ac; i++)
    {
      if(fts_is_number(at + i))
	vec->values[i + onset] = fts_get_number_int(at + i);
      else
	vec->values[i + onset] = 0;
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

int
ivec_get_max_abs_value_in_range(ivec_t *vec, int a, int b)
{
  int max;
  int i;	

  max = vec->values[a];

  for (i=a+1; i<vec->size && i < b; i++)
    if ( abs( vec->values[i]) > abs( max))
      max = vec->values[i];

  return max;
}

int
ivec_get_max_value_in_range(ivec_t *vec, int a, int b)
{
  int max;
  int i;	

  max = vec->values[a];

  for (i=a+1; i<vec->size && i < b; i++)
    if ( vec->values[i] > max)
      max = vec->values[i];

  return max;
}

int
ivec_get_min_value_in_range(ivec_t *vec, int a, int b)
{
  int min;
  int i;	

  min = vec->values[a];

  for (i=a+1; i<vec->size && i < b; i++)
    if ( vec->values[i] < min)
      min = vec->values[i];

  return min;
}

void
ivec_copy(ivec_t *org, ivec_t *copy)
{
  int i;
  
  ivec_set_size(copy, org->size);

  for(i=0; i<org->size; i++)
    copy->values[i] = org->values[i];
}

static void
ivec_copy_function(const fts_object_t *from, fts_object_t *to)
{
  ivec_copy((ivec_t *)from, (ivec_t *)to);
}

static int
ivec_equals(const ivec_t *a, const ivec_t *b)
{
  int a_n = ivec_get_size(a);
  int b_n = ivec_get_size(b);
  
  if(a_n == b_n)
  {
    int i;
    
    for(i=0; i<a_n; i++)
      if(ivec_get_element(a, i) != ivec_get_element(b, i))
        return 0;
    
    return 1;
  }
  
  return 0;
}

static void
ivec_array_function(fts_object_t *o, fts_array_t *array)
{
  ivec_t *this = (ivec_t *)o;
  int *values = ivec_get_ptr(this);
  int size = ivec_get_size(this);
  int onset = fts_array_get_size(array);
  fts_atom_t *atoms;
  int i;
  
  fts_array_set_size(array, onset + size);
  atoms = fts_array_get_atoms(array) + onset;
  
  for(i=0; i<size; i++)
    fts_set_int(atoms + i, values[i]);
}

/*********************************************************
*
*  client methods
*
*
*********************************************************/

static fts_method_status_t 
ivec_open_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *this = (ivec_t *)o;
  fts_atom_t a;
  
  if(this->editor == NULL)
  {
    fts_set_object(&a, o);
    this->editor = fts_object_create( tabeditor_type, 1, &a);
    fts_object_refer( this->editor);
  }
  
  if(fts_object_has_client( (fts_object_t *)this->editor) == 0)
  {
    fts_client_register_object( (fts_object_t *)this->editor, fts_object_get_client_id( o));
	  
    fts_set_int(&a, fts_object_get_id( (fts_object_t *)this->editor));
    fts_client_send_message( o, fts_s_editor, 1, &a);
    
    fts_send_message( (fts_object_t *)this->editor, fts_s_upload, 0, 0, fts_nix);
  }
  
  ivec_set_editor_open( this);
  fts_client_send_message(o, fts_s_openEditor, 0, 0);
  
  return fts_ok;
}

static fts_method_status_t
ivec_destroy_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *this = (ivec_t *)o;

  ivec_set_editor_close( this);
  
  return fts_ok;
}

static fts_method_status_t
ivec_close_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *this = (ivec_t *) o;

  if(ivec_editor_is_open(this))
    {
      ivec_set_editor_close(this);
      fts_client_send_message((fts_object_t *)this, fts_s_closeEditor, 0, 0);  
    }
  
  return fts_ok;
}

/********************************************************************
 *
 *   user methods
 *
 */
static fts_method_status_t
ivec_fill(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *this = (ivec_t *)o;

  ivec_set_const( this, fts_get_int_arg(ac, at, 0, 0));

  if( this->editor)
    tabeditor_send( (tabeditor_t *)this->editor);

  fts_object_set_state_dirty( o);
  
  return fts_ok;
}

static fts_method_status_t
ivec_set_elements(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *this = (ivec_t *)o;
    
  if(ac > 1 && fts_is_number(at))
    {
      int size = ivec_get_size(this);
      int onset = fts_get_number_int(at);
  
      if(onset >= 0 && onset < size)
	{
	  ivec_set_with_onset_from_atoms(this, onset, ac - 1, at + 1);
	  
	  if( this->editor)
	    tabeditor_insert_append( (tabeditor_t *)this->editor, onset, ac, at);
	
	  fts_object_set_state_dirty( o);
	}
    }
  
  return fts_ok;
}

static fts_method_status_t
ivec_reverse(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *this = (ivec_t *)o;
  int *ptr = ivec_get_ptr(this);
  int size = ivec_get_size(this); 
  int i, j;

  for(i=0, j=size-1; i<size/2; i++, j--)
    {
      int f = ptr[i];

      ptr[i] = ptr[j];
      ptr[j] = f;
    }

  fts_object_set_state_dirty( o);
  
  return fts_ok;
}

static int 
ivec_element_compare(const void *left, const void *right)
{
  int l = *((const int *)left);
  int r = *((const int *)right);

  return l - r;
}

static fts_method_status_t
ivec_sort(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *this = (ivec_t *)o;
  int *ptr = ivec_get_ptr(this);

  qsort((void *)ptr, ivec_get_size(this), sizeof(int), ivec_element_compare);

  fts_object_set_state_dirty( o);
  
  return fts_ok;
}

static fts_method_status_t
ivec_scramble(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *this = (ivec_t *)o;
  int *ptr = ivec_get_ptr(this);
  int size = ivec_get_size(this);
  double range = size;
  int i;

  for(i=0; i<size-1; i++)
    {
      int random = (int)(range * fts_random() / FTS_RANDOM_RANGE);
      int f = ptr[i];

      ptr[i] = ptr[i + random];
      ptr[i + random] = f;

      range -= 1.0;
    }

  fts_object_set_state_dirty( o);
  
  return fts_ok;
}

static fts_method_status_t
ivec_rotate(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *this = (ivec_t *)o;
  int *ptr = ivec_get_ptr(this);
  int size = ivec_get_size(this);

  if(size > 1)
    {
      int shift = 1;
      
      if(ac && fts_is_number(at))
	shift = fts_get_number_int(at);
      
      if(shift == 1)
	{
	  int f = ptr[size - 1];
	  int i;

	  for(i=size-2; i>=0; i--)
	    ptr[i + 1] = ptr[i];

	  ptr[0] = f;
	}
      else if(shift == -1)
	{
	  int f = ptr[0];
	  int i;

	  for(i=0; i<size-1; i++)
	    ptr[i] = ptr[i + 1];

	  ptr[size - 1] = f;
	}
      else
	{
	  int forward;
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
		  int swap = ptr[next];

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

      fts_object_set_state_dirty( o);
    }
  
  return fts_ok;
}

static fts_method_status_t
ivec_return_element(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *this = (ivec_t *)o;
  int index = fts_get_int_arg(ac, at, 0, -1);

  if (index >= 0 && index < ivec_get_size(this))
    fts_set_int(ret, ivec_get_element(this, index));
  
  return fts_ok;
}

static fts_method_status_t
ivec_return_interpolated(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *this = (ivec_t *)o;
  int size = ivec_get_size(this);
  double f = fts_get_number_float(at);
  double r = 0.0;

  if(size < 2 || f <= 0.0)
    r = ivec_get_element(this, 0);
  else if(f >= size - 1)
    r = ivec_get_element(this, size - 1);
  else
  {
    int i = (int)f;
    double y0 = ivec_get_element(this, i);
    double y1 = ivec_get_element(this, i + 1);

    r = y0 + (f - i) * (y1 - y0);
  }

  fts_set_float(ret, (float)r);
  
  return fts_ok;
}

static fts_method_status_t
ivec_return_size(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *this = (ivec_t *)o;
  fts_atom_t a;

  fts_set_int(&a, ivec_get_size(this));
  *ret = a;
  
  return fts_ok;
}

static fts_method_status_t
ivec_change_size(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *this = (ivec_t *)o;
  int size = fts_get_number_int(at);
  
  if(size >= 0)
  {
    int old_size = this->size;
    int i;
    
    ivec_set_size( this, size);
    
    /* when extending: zero new values*/
    for(i=old_size; i<size; i++)	
      this->values[i] = 0;		
    
    if( this->editor)
    {
      fts_client_send_message( this->editor, fts_s_size, ac, at);
      fts_object_set_state_dirty( o);
    }
  }
  
  return fts_ok;
}

/********************************************************************
 *
 *  system functions
 *
 */

static fts_method_status_t
ivec_print(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *this = (ivec_t *)o;
  int size = ivec_get_size(this);
  fts_bytestream_t* stream = fts_get_default_console_stream();
  
  if(ac > 0 && fts_is_object(at))
    stream = (fts_bytestream_t *)fts_get_object(at);
  
  if(size == 0)
    fts_spost(stream, "<empty ivec>\n");
  else if(size == 1)
    fts_spost(stream, "<ivec of 1 element: %d>\n", ivec_get_element(this, 0));
  else if(size <= FTS_POST_MAX_ELEMENTS)
    {
      int i;
      
      fts_spost(stream, "<ivec of %d elements: ", size);
      
      for(i=0; i<size-1; i++)
	fts_spost(stream, "%d ", ivec_get_element(this, i));
      
      fts_spost(stream, "%d>\n", ivec_get_element(this, i));
    }
  else
    {
      int i, j;
      
      fts_spost(stream, "<ivec of %d elements>\n", size);
      fts_spost(stream, "{\n");
      
      for(i=0; i<size; i+=FTS_POST_MAX_ELEMENTS)
	{
	  int n = size - i;

	  if(n > FTS_POST_MAX_ELEMENTS)
	    n = FTS_POST_MAX_ELEMENTS;
	  
	  fts_spost(stream, "  ");
	  
	  for(j=0; j<n; j++)
	    fts_spost(stream, "%d ", ivec_get_element(this, i + j));
	  
	  fts_spost(stream, "\n");
	}
      
      fts_spost(stream, "}\n");
    }
  
  return fts_ok;
}

static fts_method_status_t
ivec_set_from_instance(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *this = (ivec_t *)o;
  ivec_t *set = (ivec_t *)fts_get_object(at);

  ivec_copy(set, this);

  fts_object_set_state_dirty( o);
  
  return fts_ok;
}

static fts_method_status_t
ivec_dump_state(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *this = (ivec_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);      
  int size = ivec_get_size(this);
  fts_message_t *mess;
  int i;
  
  /* send size message */
  mess = fts_dumper_message_get(dumper, fts_s_size);  
  fts_message_append_int(mess, size);
  fts_dumper_message_send(dumper, mess);
  
  /* get new set message and append onset 0 */
  mess = fts_dumper_message_get(dumper, fts_s_set);
  fts_message_append_int(mess, 0);
  
  for(i=0; i<size; i++)
    {
      fts_message_append_int(mess, this->values[i]);
      
      if(fts_message_get_ac(mess) >= 256)
	{
	  fts_dumper_message_send(dumper, mess);
	  
	  /* new set message and append onset i + 1 */
	  mess = fts_dumper_message_get(dumper, fts_s_set);
	  fts_message_append_int(mess, i + 1);
	}
    }
  
  if(fts_message_get_ac(mess) > 1) 
    fts_dumper_message_send(dumper, mess);
  
  return fts_ok;
}

/*********************************************************
 *
 *  class
 *
 */
static fts_method_status_t
ivec_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *this = (ivec_t *)o;
  
  this->values = 0;
  this->size = 0;
  this->alloc = 0;
  
  this->editor = 0;
  
  if(ac == 0)
    ivec_set_size(this, 0);
  else if(ac == 1 && fts_is_int(at))
  {
    ivec_set_size(this, fts_get_int(at));
    ivec_set_const(this, 0);
  }
  else if(ac == 1 && fts_is_tuple(at))
  {
    fts_tuple_t *tup = (fts_tuple_t *)fts_get_object(at);
    int size = fts_tuple_get_size(tup);
    
    ivec_set_size(this, size);
    ivec_set_with_onset_from_atoms(this, 0, size, fts_tuple_get_atoms(tup));
  }
  else if(ac > 1)
  {
    ivec_set_size(this, ac);
    ivec_set_with_onset_from_atoms(this, 0, ac, at);
  }
  else
    fts_object_error(o, "bad arguments for ivec constructor");
  
  return fts_ok;
}

static fts_method_status_t
ivec_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *this = (ivec_t *)o;
  
  if(this->editor) 
  {  
    fts_client_send_message( (fts_object_t *)this->editor, fts_s_destroyEditor, 0, 0);
    fts_object_destroy((fts_object_t *)this->editor);
  }  
  
  if(this->values)
    fts_free(this->values);

  return fts_ok;
}

static void
ivec_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(ivec_t), ivec_init, ivec_delete);
  
  fts_class_set_copy_function(cl, ivec_copy_function);
  fts_class_set_array_function(cl, ivec_array_function);
  
  fts_class_message_varargs(cl, fts_s_name, fts_object_name);
  fts_class_message_varargs(cl, fts_s_persistence, fts_object_persistence);
  fts_class_message_varargs(cl, fts_s_dump_state, ivec_dump_state);

  /* graphical editor */
  fts_class_message_varargs(cl, fts_s_openEditor, ivec_open_editor);
  fts_class_message_varargs(cl, fts_s_closeEditor, ivec_close_editor);
  fts_class_message_varargs(cl, fts_s_destroyEditor, ivec_destroy_editor);

  fts_class_message_varargs(cl, fts_s_print, ivec_print); 

  fts_class_message_varargs(cl, fts_s_set_from_instance, ivec_set_from_instance);

  fts_class_message_varargs(cl, fts_new_symbol("reverse"), ivec_reverse);
  fts_class_message_varargs(cl, fts_new_symbol("rotate"), ivec_rotate);
  fts_class_message_varargs(cl, fts_new_symbol("sort"), ivec_sort);
  fts_class_message_varargs(cl, fts_new_symbol("scramble"), ivec_scramble);

  fts_class_message_varargs(cl, fts_s_fill, ivec_fill);
  fts_class_message_varargs(cl, fts_s_set, ivec_set_elements);

  fts_class_message_void(cl, fts_s_size, ivec_return_size);
  fts_class_message_number(cl, fts_s_size, ivec_change_size);

  fts_class_message_int(cl, fts_s_get_element, ivec_return_element);
  fts_class_message_float(cl, fts_s_get_element, ivec_return_interpolated);
  
  fts_class_inlet_bang(cl, 0, data_object_output);

  fts_class_inlet_thru(cl, 0);
  fts_class_outlet_thru(cl, 0);
}

/********************************************************************
 *
 *  config
 *
 */

FTS_MODULE_INIT(ivec)
{
  sym_text = fts_new_symbol("text");
  ivec_symbol = fts_new_symbol("ivec");

  ivec_class = fts_class_install(ivec_symbol, ivec_instantiate);
}




