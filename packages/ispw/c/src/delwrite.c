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

#include "delbuf.h"
#include "deltable.h"
#include "sampunit.h"

static fts_symbol_t dsp_symbol = 0;
extern void ftl_delwrite(fts_word_t *a);

/**************************************************
 *
 *    object
 *
 */

typedef struct _delwrite_t
{
  fts_object_t o;
  fts_symbol_t name;
  fts_symbol_t unit;
  del_buf_t *buf;
  int rec_prot; /* housekeeping for dead code elimination */
} delwrite_t;

static void
delwrite_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delwrite_t *this = (delwrite_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t unit = samples_unit_get_arg(ac, at, 1);
  float size;

  if(unit)
    size = fts_get_float_arg(ac, at, 2, 0.0f);
  else{
    size = fts_get_float_arg(ac, at, 1, 0.0f);
    unit = samples_unit_get_default();

  }

  this->name = 0;
  
  if(delay_table_get_delbuf(name))
    {
      fts_object_set_error(o, "name multiply defined: %s\n", name);
      return;
    }

  this->name = name;
  this->unit = unit;
  this->buf = delbuf_new(size, unit);

  this->rec_prot = 0;

  delay_table_add_delwrite(o, this->name, this->buf);
  fts_dsp_add_object(o); /* just put object in list */
}

static void
delwrite_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delwrite_t *this = (delwrite_t *)o;

  if(this->name)
    {
      delbuf_delete_delayline(this->buf);
      delay_table_remove_delwrite(o, this->name);
      fts_dsp_remove_object(o);
    }
}


/**************************************************
 *
 *    dsp
 *
 */

static void
delwrite_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delwrite_t *this = (delwrite_t *)o;
  fts_atom_t argv[5];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  long n_tick = fts_dsp_get_input_size(dsp, 0);
  float sr = fts_dsp_get_input_srate(dsp, 0);
  
  if(!this->name)
    fts_object_signal_runtime_error(o, "unnamed delwrite~\n");

  if(delbuf_is_init(this->buf))
    {
      if(delbuf_get_tick_size(this->buf) != n_tick)
	{
	  fts_object_signal_runtime_error(o, "write sample rate does not match with delay line %s\n", this->name);
	  return;
	}
    }
  else
    {
      int success;
      success = delbuf_init(this->buf, sr, n_tick);
      if(!success) return;
    }

  fts_set_symbol(argv, fts_dsp_get_input_name(dsp, 0));
  fts_set_pointer(argv + 1, this->buf);
  fts_set_int(argv + 2, n_tick);
  fts_dsp_add_function(dsp_symbol, 3, argv);

  delay_table_delwrite_scheduled(this->name);
}

/**************************************************
 *
 *    user methods
 *
 */

static void
delwrite_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delwrite_t *this = (delwrite_t *)o;

  if(this->name) 
    delbuf_clear_delayline(this->buf);
}

static void
delwrite_realloc(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delwrite_t *this = (delwrite_t *)o;
  float size  = fts_get_float_arg(ac, at, 1, 0.0f);

  if(this->name)
    delbuf_set_size(this->buf, size, this->unit);
}


/**************************************************
 *
 *    class
 *
 */


static void
delwrite_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(delwrite_t), delwrite_init, delwrite_delete);

  fts_class_message_varargs(cl, fts_s_put, delwrite_put);

  fts_class_message_varargs(cl, fts_s_clear, delwrite_clear);
  fts_class_message_varargs(cl, fts_new_symbol("realloc"), delwrite_realloc);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
  
  dsp_symbol = fts_new_symbol("delwrite");
  fts_dsp_declare_function(dsp_symbol, ftl_delwrite);
}

void
delwrite_config(void)
{
  fts_class_install(fts_new_symbol("delwrite~"),delwrite_instantiate);
}
