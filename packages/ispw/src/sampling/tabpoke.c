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

#include "fts.h"
#include "sampbuf.h"

/******************************************************************
 *
 *    object
 *
 */
 
typedef struct
{
  fts_object_t obj; 

  fts_symbol_t tab_name;	/* symbol bound to table we'll use */
  float value;		/* value to write ot samptab */
  int state;		/* inlet state - table style */
} tabpoke_t;


static void
tabpoke_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tabpoke_t *this = (tabpoke_t *)o;
  fts_symbol_t tab_name = fts_get_symbol_arg(ac, at, 1, 0);

  this->tab_name = tab_name;
  this->state    = 0;
}


/******************************************************************
 *
 *    user methods
 *
 */

static void
tabpoke_set(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  tabpoke_t *this = (tabpoke_t *)o;
  char name_str[64];
  fts_symbol_t tab_name;
  
  if (fts_is_long(at))
    {
      gensampname(name_str, "sample", fts_get_long_arg(ac, at, 0, 0));
      tab_name = fts_new_symbol_copy(name_str);
    }
  else
    tab_name = fts_get_symbol_arg(ac, at, 0, 0);

  if (sampbuf_get(tab_name))
    this->tab_name = tab_name;
  else
    post("tabpoke: %s: can't find table~\n", fts_symbol_name(tab_name));
}

static void
tabpoke_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tabpoke_t *this = (tabpoke_t *)o;
  long n = fts_get_number_int(at);
  sampbuf_t *buf;

  buf = sampbuf_get(this->tab_name);

  if (buf)
    {
      if (n >= buf->size)
	n = buf->size - 1;
      else if(n < 0)
	n = 0;
      
      if (this->state)
	{
	  buf->samples[n] = this->value;
	  this->state = 0;
	}
      else
	fts_outlet_float(o, 0, (double)buf->samples[n]);
    }
  else
    post("tabpoke: %s: can't find table~\n", fts_symbol_name(this->tab_name));
}


static void
tabpoke_set_value(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tabpoke_t *this = (tabpoke_t *)o;

  this->value = fts_get_number_float(at);
  this->state = 1;
}

static void
tabpoke_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if ((ac >= 2) && fts_is_number(at+1))
    tabpoke_set_value(o, winlet, s, ac, at+1);

  if ((ac >= 1) && fts_is_number(at))
    tabpoke_number(o, winlet, s, ac, at);
}

static void
tabpoke_center(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tabpoke_t *this = (tabpoke_t *)o;
  long len = fts_get_number_int(at);
  sampbuf_t *buf;

  buf = sampbuf_get(this->tab_name);

  if (buf)
    {
      long i;
      double result, top = 0.0f, bottom = 0.0f;
    
      if(buf->size < len)
	post("tabpoke: center: table too short\n");
      else
	{
	  for(i=0; i<len;i++)
	    {
	      top += (i * buf->samples[i]);
	      bottom += buf->samples[i];
	    }

	  result = (bottom)? (top/bottom): 0.0f;
	  fts_outlet_float(o, 0, result);
	}		
    }
  else
    post("tabpoke: %s: can't find table~\n", fts_symbol_name(this->tab_name));
}

/******************************************************************
 *
 *    class
 *
 */
 
static fts_status_t
class_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  fts_class_init(cl, sizeof(tabpoke_t), 2, 1, 0); 

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, tabpoke_init, 2, a);
  
  a[0] = fts_s_float;
  fts_outlet_type_define(cl, 0,	fts_s_float, 1, a);

  /* user methods */

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, tabpoke_number, 1, a);
  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, tabpoke_number, 1, a);

  a[0] = fts_s_anything;
  fts_method_define(cl, 0, fts_new_symbol("set"), tabpoke_set, 1, a);

  fts_method_define_varargs(cl, 0, fts_s_list, tabpoke_list);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, tabpoke_set_value, 1, a);
  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, tabpoke_set_value, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_new_symbol("center"), tabpoke_center, 1, a);

  return fts_Success;
}

void
tabpoke_config(void)
{
  fts_class_install(fts_new_symbol("tabpoke"), class_instantiate);
}

