/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and ditributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include <fts/fts.h>
#include "mt.h"

/**********************************************************
 *
 *  object
 *
 */

typedef struct
{
  fts_object_t o;
  message_table_t *tab;
} messtab_t;

static void
messtab_init_refer(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messtab_t *this = (messtab_t *)o;

  this->tab = (message_table_t *)fts_get_ptr(at + 1);
  message_table_refer(this->tab);
}

static void
messtab_init_define(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messtab_t *this = (messtab_t *)o;
  
  if(ac == 1)
    {
      this->tab = message_table_new(0);
    }
  else if(fts_is_number(at + 1))
    {
      int size = fts_get_number_int(at + 1);
      
      this->tab = message_table_new(size);
    }
  else
    {
      int size = ac - 1;
      int i;
      
      this->tab = message_table_new(size);

      for(i=0; i<size; i++)
	{
	  message_t *mess = &message_table_get_element(this->tab, i);

	  if(fts_is_list(at + 1 + i))
	    {
	      fts_list_t *aa = fts_get_list(at + 1 + i);
	      int aac = fts_list_get_size(aa);
	      fts_atom_t *aat = fts_list_get_ptr(aa);
	      
	      if(fts_is_symbol(aat))
		message_set(mess, fts_get_symbol(aat), aac - 1, aat + 1);
	      else
		message_set(mess, fts_s_list, aac, aat);
	    }
	}
    }
  
  message_table_refer(this->tab);
}

static void
messtab_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messtab_t *this = (messtab_t *)o;

  message_table_release(this->tab);
}

/**********************************************************
 *
 *  user methods
 *
 */

static void
messtab_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messtab_t *this = (messtab_t *)o;
  int index = fts_get_number_int(at);

  if(index >= 0 && index < message_table_get_size(this->tab))
    {
      message_t *mess = &message_table_get_element(this->tab, index);

      if(mess->s)
	fts_outlet_send(o, 0, mess->s, mess->ac, mess->at);
    }
}

static void
messtab_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messtab_t *this = (messtab_t *)o;

  if(ac && fts_is_number(at))
    {
      int index = fts_get_number_int(at);

      if(index >= 0 && index < message_table_get_size(this->tab))
	{
	  message_t *mess = &message_table_get_element(this->tab, index);
	  
	  if(fts_is_symbol(at + 1))
	    message_set(mess, fts_get_symbol(at + 1), ac - 2, (fts_atom_t *)(at + 2));
	  else
	    message_set(mess, fts_s_list, ac - 1, (fts_atom_t *)(at + 1));
	}
    }
}

static void
messtab_set_reference(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messtab_t *this = (messtab_t *)o;

  if(ac && fts_is_a(at, message_table_type))
    {
      message_table_t *tab = (message_table_t *)fts_get_ptr(at);
      
      message_table_release(this->tab);
      this->tab = tab;
      message_table_refer(tab);  
    }
}

static void
messtab_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messtab_t *this = (messtab_t *)o;

  if(ac == 0)
    {
      int i;

      for(i=0; i<message_table_get_size(this->tab); i++)
	{
	  message_t *mess = &message_table_get_element(this->tab, i);

	  message_clear(mess);
	}
    }
  else if(fts_is_number(at))
    {
      int i = fts_get_number_int(at);

      if(i >= 0 && i < message_table_get_size(this->tab))
	{
	  message_t *mess = &message_table_get_element(this->tab, i);
	  message_clear(mess);
	}
    }
}

static void
messtab_set_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messtab_t *this = (messtab_t *)o;

  if(ac && fts_is_number(at))
    {
      int size = fts_get_number_int(at);

      if(size >= 0)
	message_table_set_size(this->tab, size);
    }
}

static void
messtab_import(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messtab_t *this = (messtab_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 0, 0);
  int n;
  
  n = message_table_file_import_ascii(this->tab, name);
}

static void
messtab_export(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messtab_t *this = (messtab_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 0, 0);
  
  message_table_file_export_ascii(this->tab, name);
}

static void
messtab_assist(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_symbol_t cmd = fts_get_symbol_arg(ac, at, 0, 0);

  if (cmd == fts_s_object)
    fts_object_blip(o, "table of messages");
  else if (cmd == fts_s_inlet)
    {
      int n = fts_get_int_arg(ac, at, 1, 0);

      switch(n)
	{
	case 0:
	  fts_object_blip(o, "<number>: index to recall message from table");
	  break;
	case 1:
	  fts_object_blip(o, "<messtab>: set reference");
	  break;
	}
    }
  else if (cmd == fts_s_outlet)
    {
      int n = fts_get_int_arg(ac, at, 1, 0);

      switch(n)
	{
	case 0:
	  fts_object_blip(o, "recalled message", n);
	  break;
	}
    }
}

/**********************************************************
 *
 *  class
 *
 */

/* when defining a variable: daemon for getting the property "state". */
static void
messtab_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  messtab_t *this = (messtab_t *) obj;
  
  fts_word_set_ptr(fts_atom_value(value), (void *)this->tab);
  fts_set_type(value, message_table_type);
}

static fts_status_t
messtab_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[5];

  if(ac == 2 && fts_is_a(at + 1, message_table_type))
    {
      fts_class_init(cl, sizeof(messtab_t), 2, 1, 0);
  
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, messtab_init_refer);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, messtab_delete);

      fts_method_define_varargs(cl, 1, fts_s_anything, messtab_set_reference);
    }
  else if(ac == 1 || (ac > 1 && (fts_is_int(at + 1) || fts_is_list(at + 1))))
    {
      int i;

      for(i=2; i<ac; i++)
	{
	  if(!fts_is_list(at + i))
	    return &fts_CannotInstantiate;	    
	}

      /* define variable */
      fts_class_init(cl, sizeof(messtab_t), 1, 1, 0);
  
      fts_class_add_daemon(cl, obj_property_get, fts_s_state, messtab_get_state);

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, messtab_init_define);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, messtab_delete);

      fts_method_define_varargs(cl, 0, fts_new_symbol("import"), messtab_import);
      fts_method_define_varargs(cl, 0, fts_new_symbol("export"), messtab_export);

      fts_method_define_varargs(cl, 0, fts_new_symbol("size"), messtab_set_size);
    }
  else
    return &fts_CannotInstantiate;
  
  fts_method_define_varargs(cl, 0, fts_s_int, messtab_index);
  fts_method_define_varargs(cl, 0, fts_s_float, messtab_index);

  fts_method_define_varargs(cl, 0, fts_s_set, messtab_set);
  fts_method_define_varargs(cl, 0, fts_new_symbol("clear"), messtab_clear);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("assist"), messtab_assist); 

  return fts_Success;
}

void
messtab_config(void)
{
  fts_metaclass_install(fts_new_symbol("messtab"), messtab_instantiate, fts_arg_type_equiv);
}
