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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include <fts/fts.h>
#include "keyserver.h"

static fts_symbol_t sym_f1 = 0;
static fts_symbol_t sym_f2 = 0;
static fts_symbol_t sym_f3 = 0;
static fts_symbol_t sym_f4 = 0;
static fts_symbol_t sym_f5 = 0;
static fts_symbol_t sym_f6 = 0;
static fts_symbol_t sym_f7 = 0;
static fts_symbol_t sym_f8 = 0;
static fts_symbol_t sym_f9 = 0;
static fts_symbol_t sym_f10 = 0;
static fts_symbol_t sym_f11 = 0;
static fts_symbol_t sym_f12 = 0;

static fts_symbol_t sym_enter = 0;
static fts_symbol_t sym_esc = 0;

static fts_symbol_t sym_shift = 0;
static fts_symbol_t sym_alt = 0;

static fts_symbol_t sym_space = 0;

static int 
get_code(const fts_atom_t *a)
{
  if(fts_is_int(a))
    {
      int code = fts_get_int(a);

      if(code > 0 && code < 128)
	return code;
    }
  else if(fts_is_symbol(a))
    {
      fts_symbol_t sym = fts_get_symbol(a);

      if(sym == sym_f1)
	return code_f1;
      else if(sym == sym_f2)
	return code_f2;
      else if(sym == sym_f3)
	return code_f3;
      else if(sym == sym_f4)
	return code_f4;
      else if(sym == sym_f5)
	return code_f5;
      else if(sym == sym_f6)
	return code_f6;
      else if(sym == sym_f7)
	return code_f7;
      else if(sym == sym_f8)
	return code_f8;
      else if(sym == sym_f9)
	return code_f9;
      else if(sym == sym_f10)
	return code_f10;
      else if(sym == sym_f11)
	return code_f11;
      else if(sym == sym_f12)
	return code_f12;
      else if(sym == sym_enter)
	return code_enter;
      else if(sym == sym_esc)
	return code_esc;
      else if(sym == sym_shift)
	return code_shift;
      else if(sym == sym_alt)
	return code_alt;
      else if(sym == sym_space)
	return 32;
      else
	{
	  const char *str = sym;
	  
	  if(str[1] == '\0')
	    {
	      int code = (int)str[0];
	      
	      if(code >= 32 && code < 127)
		return code;
	    }
	}
    }

  return 0;
}

/*******************************************************************
 *
 *  key
 *
 */

typedef struct 
{
  fts_object_t o;
  int code;
} fts_key_t;

static void
key_multi_action(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int code = fts_get_int(at + 0);
  int status = fts_get_int(at + 1);

  if(status == 0)
    fts_outlet_int(o, 1, code);
  else
    fts_outlet_int(o, 0, code);  
}

static void
key_action(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int code = fts_get_int(at + 0);
  int status = fts_get_int(at + 1);

  if(status == 0)
    fts_outlet_bang(o, 1);
  else
    fts_outlet_bang(o, 0);  
}

static void
key_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_key_t *this = (fts_key_t *)o;

  if(ac == 1)
    {
      keyserver_add_listener(0, o, key_multi_action);
      this->code = 0;
    }
  else
    {
      int code = get_code(at + 1);

      if(code > 0)
	keyserver_add_listener(code, o, key_action);
      else
	{
	  post("key: unknown key specifier: ");
	  post_atoms(1, at + 1);
	  post("\n");
	}

      this->code = code;
    }
}

static void
key_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_key_t *this = (fts_key_t *)o;

  keyserver_remove_listener(this->code, o);
}

static fts_status_t
key_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac == 1)
    {
      fts_class_init(cl, sizeof(fts_key_t), 0, 2, 0);
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, key_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, key_delete);
    }

  else if(ac == 2)
    {
      fts_class_init(cl, sizeof(fts_key_t), 0, 2, 0);
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, key_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, key_delete);
    }
  else
    return &fts_CannotInstantiate;

  return fts_Success;
}

/*******************************************************************
 *
 *  keystat
 *
 */

static void
keystat_multi_action(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  int code = fts_get_int(at + 0);
  int status = fts_get_int(at + 1);

  fts_outlet_int(o, 1, status);
  fts_outlet_int(o, 0, code);
}

static void
keystat_action(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  int code = fts_get_int(at + 0);
  int status = fts_get_int(at + 1);

  fts_outlet_int(o, 0, status);
}

static void
keystat_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_key_t *this = (fts_key_t *)o;

  if(ac == 1)
    {
      keyserver_add_listener(0, o, keystat_multi_action);
      this->code = 0;
    }
  else
    {
      int code = get_code(at + 1);

      if(code > 0)
	keyserver_add_listener(code, o, keystat_action);
      else
	{
	  post("keystat: unknown key specifier: ");
	  post_atoms(1, at + 1);
	  post("\n");
	}

      this->code = code;
    }
}

static void
keystat_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_key_t *this = (fts_key_t *)o;

  keyserver_remove_listener(this->code, o);
}

static fts_status_t
keystat_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac == 1)
    {
      fts_class_init(cl, sizeof(fts_key_t), 0, 2, 0);
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, keystat_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, keystat_delete);
    }

  else if(ac == 2)
    {
      fts_class_init(cl, sizeof(fts_key_t), 0, 1, 0);
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, keystat_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, keystat_delete);
    }
  else
    return &fts_CannotInstantiate;

  return fts_Success;
}

void
key_config(void)
{
  sym_f1 = fts_new_symbol("f1");
  sym_f2 = fts_new_symbol("f2");
  sym_f3 = fts_new_symbol("f3");
  sym_f4 = fts_new_symbol("f4");
  sym_f5 = fts_new_symbol("f5");
  sym_f6 = fts_new_symbol("f6");
  sym_f7 = fts_new_symbol("f7");
  sym_f8 = fts_new_symbol("f8");
  sym_f9 = fts_new_symbol("f9");
  sym_f10 = fts_new_symbol("f10");
  sym_f11 = fts_new_symbol("f11");
  sym_f12 = fts_new_symbol("f12");

  sym_enter = fts_new_symbol("enter");
  sym_esc = fts_new_symbol("esc");

  sym_shift = fts_new_symbol("shift");
  sym_alt = fts_new_symbol("alt");

  sym_space = fts_new_symbol("space");

  fts_metaclass_install(fts_new_symbol("key"), key_instantiate, fts_narg_equiv);
  fts_metaclass_install(fts_new_symbol("keystat"), keystat_instantiate, fts_narg_equiv);
}
