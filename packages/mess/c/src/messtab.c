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
#include "fts.h"
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
  message_t buf;
} messtab_t;

static void
messtab_init_refer(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messtab_t *this = (messtab_t *)o;

  this->tab = (message_table_t *)fts_get_ptr(at + 1);
  message_init(&this->buf, 0, 0, 0);
}

static void
messtab_init_define(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messtab_t *this = (messtab_t *)o;
  int size = fts_get_int_arg(ac, at, 1, 0);

  this->tab = message_table_new(size);
  message_init(&this->buf, 0, 0, 0);
}

static void
messtab_delete_define(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messtab_t *this = (messtab_t *)o;

  message_table_delete(this->tab);
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
  int index = fts_get_int_arg(ac, at, 0, 0);

  if(index >= 0 && index < this->tab->size)
    {      
      message_t *mess = &message_table_get_element(this->tab, index);

      if(this->buf.s)
	message_move(&this->buf, mess);
      else if(mess->s)
	fts_outlet_send(o, 0, mess->s, mess->ac, mess->at);
    }
  else
    message_clear(&this->buf);
}

static void
messtab_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messtab_t *this = (messtab_t *)o;
  int index = fts_get_int_arg(ac, at, 0, 0);

  if(ac == 0)
    message_table_clear(this->tab);

  if(index > 0 && index < this->tab->size)
    {
      message_t *mess = &message_table_get_element(this->tab, index);
      message_clear(mess);
    }
}

static void
messtab_set_buffer(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messtab_t *this = (messtab_t *)o;

  message_set(&this->buf, s, ac, (fts_atom_t *)at);
}

static void
messtab_import(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messtab_t *this = (messtab_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 0, 0);
  int n;
  
  n = message_table_file_import_ascii(this->tab, name);
  
  if(n)
    fts_outlet_int(o, 1, n);
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
	  fts_object_blip(o, "<number>: index to recall/store message from/to table");
	  break;
	case 1:
	  fts_object_blip(o, "set message to be stored");
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
	case 1:
	  fts_object_blip(o, "<int>: # of messages read from file");
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
  fts_type_t type = (ac)? fts_get_type(at): 0;

  fts_class_init(cl, sizeof(messtab_t), 2, 2, 0);

  if(ac == 2 && fts_is_a(at + 1, message_table_type))
    fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, messtab_init_refer);
  else if(ac > 1 && fts_is_int(at + 1))
    {
      /* define variable */
      fts_class_add_daemon(cl, obj_property_get, fts_s_state, messtab_get_state);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, messtab_init_define);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, messtab_delete_define);
    }
  else
    return &fts_CannotInstantiate;
  
  /* set and get */
  fts_method_define_varargs(cl, 0, fts_s_int, messtab_index);

  /* message in */
  fts_method_define_varargs(cl, 1, fts_s_anything, messtab_set_buffer);

  fts_method_define_varargs(cl, 0, fts_new_symbol("import"), messtab_import);
  fts_method_define_varargs(cl, 0, fts_new_symbol("export"), messtab_export);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("assist"), messtab_assist); 

  return fts_Success;
}

void
messtab_config(void)
{
  fts_metaclass_install(fts_new_symbol("messtab"), messtab_instantiate, fts_arg_type_equiv);
}
