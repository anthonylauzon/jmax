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
static fts_symbol_t sym_set_size = 0;

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

int
ivec_get_atoms(ivec_t *vec, int ac, fts_atom_t *at)
{
  int i;
  int size = vec->size;

  if(ac > size)
    ac = size;

  for(i=0; i<ac; i++)
    fts_set_int(at + i, vec->values[i]);

  return size;
}

/* copy & zero */
void
ivec_copy(ivec_t *in, ivec_t *out)
{
  int i;

  set_size(out, in->size);

  for(i=0; i<in->size; i++)
      out->values[i] = in->values[i];
}

void
ivec_zero(ivec_t *vec)
{
  int i;

  for(i=0; i<vec->size; i++)
      vec->values[i] = 0;
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
ivec_set_from_ptr(ivec_t *vec, int *ptr, int size)
{
  int i;

  set_size(vec, size);

  for(i=0; i<vec->size; i++)
    vec->values[i] = ptr[i];
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

  while(size > alloc)
    alloc += IVEC_BLOCK_SIZE;

  ivec_set_size(vec, alloc);
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
#define IVEC_CLIENT_BLOCK_SIZE 128

static void
ivec_set_client(ivec_t *ivec)
{
  int i;
  fts_atom_t a[IVEC_CLIENT_BLOCK_SIZE];
  int n = ivec_get_size(ivec);
  int append = 0;
  int current = 0;

  while(n > 0)
    {
      int send = (n > IVEC_CLIENT_BLOCK_SIZE-1)? IVEC_CLIENT_BLOCK_SIZE-1: n;

      if(!append)
	fts_set_int(&a[0], n);
      else
	fts_set_int(&a[0], current);

      for(i = 0; i < send; i++)
	  fts_set_int(&a[i+1], ivec->values[current+i]);
      
      if(!append)
	{
	  fts_client_send_message((fts_object_t *)ivec, sym_set, send+1, a);
	  append = 1;
	}
      else
	fts_client_send_message((fts_object_t *)ivec, sym_append, send+1, a);

      current+=send;
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
}

static void
ivec_fill(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;
  int constant = fts_get_int_arg(ac, at, 0, 0);

  ivec_set_const(this, constant);

  if(ivec_editor_is_open(this))
    ivec_set_client(this);
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
	ivec_set_from_atom_list(this, offset, ac - 1, at + 1);
  
      if(ivec_editor_is_open(this))
	ivec_set_client(this);
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
	ivec_set_size(this, size);
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
	post("ivec: can not import from text file \"%s\"\n", fts_symbol_name(file_name));
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
	post("ivec: can not export to text file \"%s\"\n", fts_symbol_name(file_name));
    }
  else
    post("ivec: unknown export file format \"%s\"\n", fts_symbol_name(file_format));
}

static void
ivec_open_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;
  ivec_set_editor_open(this);
  fts_client_send_message(o, sym_openEditor, 0, 0);
  ivec_set_client(this);
}

static void
ivec_close_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;

  ivec_set_editor_close(this);
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

void 
ivec_save_bmax(ivec_t *vec, fts_bmax_file_t *f)
{
  fts_atom_t av[256];
  int ac = 0;
  int i;
  int offset = 0;

  for(i=0; i<vec->size; i++)
    {
      fts_set_int(&av[ac], vec->values[i]);

      ac++;

      if (ac == 256)
	{
	  /* Code a push of all the values */
	  fts_bmax_code_push_atoms(f, ac, av);

	  /* Code a push of the offset */
	  fts_bmax_code_push_int(f, offset);

	  /* Code a "set" message for 256 values plus offset */
	  fts_bmax_code_obj_mess(f, fts_SystemInlet, fts_s_set, ac + 1);
	  offset = offset + ac;

	  /* Code a pop of all the values  */
	  fts_bmax_code_pop_args(f, ac);

	  ac = 0;
	}
    }

  if(ac != 0)
    {
      /* Code a push of all the values */
      fts_bmax_code_push_atoms(f, ac, av);

      /* Code a push of the offset */
      fts_bmax_code_push_int(f, offset);

      /* Code an "append" message for the values left */
      fts_bmax_code_obj_mess(f, fts_SystemInlet, fts_s_set, ac + 1);

      /* Code a pop of all the values  */
      fts_bmax_code_pop_args(f, ac);
    }
}

static void
ivec_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;  
  fts_bmax_file_t *f = (fts_bmax_file_t *)fts_get_ptr(at);
      
  /*ivec_save_bmax(this, f);*/
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
ivec_set_file(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  ivec_t *this = (ivec_t *)obj;

  if(fts_is_symbol(value))
    {
      fts_symbol_t name = fts_get_symbol(value);

      this->file = name;
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
}

static void
ivec_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;
  
  fts_free(this->values);
  fts_client_send_message(o, sym_destroyEditor, 0, 0);
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
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_save_bmax, ivec_bmax); 
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, ivec_set);

      /* define variable */
      fts_class_add_daemon(cl, obj_property_get, fts_s_state, ivec_get_state);

      /* set file property */
      fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("file"), ivec_set_file);

      fts_method_define_varargs(cl, 0, fts_s_bang, ivec_output);

      fts_method_define_varargs(cl, 0, fts_new_symbol("clear"), ivec_clear);
      fts_method_define_varargs(cl, 0, fts_new_symbol("fill"), ivec_fill);
      fts_method_define_varargs(cl, 0, fts_new_symbol("set"), ivec_set);
      
      fts_method_define_varargs(cl, 0, fts_new_symbol("size"), ivec_size);

      fts_method_define_varargs(cl, 0, fts_new_symbol("import"), ivec_import);
      fts_method_define_varargs(cl, 0, fts_new_symbol("export"), ivec_export);

      /* graphical editor */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("open_editor"), ivec_open_editor);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("close_editor"), ivec_close_editor);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("set_from_client"), ivec_set);

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
  sym_set_size = fts_new_symbol("setSize");

  fts_metaclass_install(ivec_symbol, ivec_instantiate, ivec_equiv);
  ivec_class = fts_class_get_by_name(ivec_symbol);

  fts_atom_type_register(ivec_symbol, ivec_class);
}
