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

#define CLASS_NAME "delread~"

static fts_symbol_t delread_function_symbol = 0;
extern void ftl_delread(fts_word_t *a);

/**************************************************
 *
 *    object
 *
 */

typedef struct
{
  fts_object_t  obj;
  fts_symbol_t name;
  fts_object_t *next; /* pointer to the other delreader for the same delay line */
  float conv;
  fts_symbol_t unit;
  float time; /* nominal delay time */
  float write_advance; /* for compensation of computation order of write on read */
  del_buf_t *buf; /* pointer to delay buffer */
  ftl_data_t ftl_deltime; /* real backward index to current phase of delayline */
} delread_t;

static void
delread_set_delay(delread_t *this)
{
  int mindel = delbuf_get_tick_size(this->buf);
  int maxdel = delbuf_get_size_in_samples(this->buf) + this->write_advance;
  int del = this->time * this->conv + this->write_advance;
  
  if (del < mindel)
    del = mindel; /* either the delwrite wrote already one tick or the min delay is one tick */
  else if (del > maxdel)
    del = maxdel;
  
  ftl_data_copy(long, this->ftl_deltime, &del);
}

static void
delread_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delread_t *this = (delread_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 1, 0);
  fts_symbol_t unit = samples_unit_get_arg(ac, at, 2);
  float time = (unit ? fts_get_float_arg(ac, at, 3, 0.0f): fts_get_float_arg(ac, at, 2, 0.0f));
  float conv;

  if(!unit) 
    unit = samples_unit_get_default();

  conv = samples_unit_convert(unit, 1.0f, fts_dsp_get_sample_rate());
  
  this->name = name;
  this->time = time;
  this->conv = conv;
  this->unit = unit;
  this->write_advance = 0;
  this->buf = 0;
  this->next = 0;

  this->ftl_deltime = ftl_data_new(long);

  delay_table_add_delreader(o, this->name);
  dsp_list_insert(o); /* just put object in list */
}


static void
delread_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delread_t *this = (delread_t *)o;

  ftl_data_free(this->ftl_deltime);

  delay_table_remove_delreader(o, this->name);
  dsp_list_remove(o);
}


/**************************************************
 *
 *    dsp
 *
 */

static void
delread_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delread_t *this = (delread_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  long n_tick = fts_dsp_get_input_size(dsp, 0);
  float sr = fts_dsp_get_input_srate(dsp, 0);
  del_buf_t *buf;
  fts_atom_t argv[5];

  buf = delay_table_get_delbuf(this->name);
  if(!buf){
    post("delread~: can't find delay line: %s\n", fts_symbol_name(this->name));
    return;
  }
   
  if(delay_table_is_delwrite_scheduled(this->name))
    this->write_advance = n_tick; /* write before read (data is at least one tick old) */
  else
    this->write_advance = 0; /* read before write */ 

  if(delbuf_is_init(buf))
    {
      if(delbuf_get_tick_size(buf) != n_tick){ /* check if n_tick of delread and delwrite matches */
	post("error: delread~: %s: sample rate does not match with delay line\n", fts_symbol_name(this->name));
	return;
      }
    }
  else
    {
      /* first delwrite~ or delread~ scheduled for this delayline inits buffer */
      int success;
      success = delbuf_init(buf, sr, n_tick);
      if(!success) return;
    }

  this->conv = samples_unit_convert(this->unit, 1.0f, sr);
  this->buf = buf;      
  
  delread_set_delay(this);
  
  fts_set_symbol(argv, fts_dsp_get_output_name(dsp, 0));
  fts_set_ptr(argv + 1, buf);
  fts_set_int(argv + 2, n_tick);
  fts_set_ftl_data(argv + 3, this->ftl_deltime);
  dsp_add_funcall(delread_function_symbol, 4, argv);

  delay_table_delreader_scheduled(this->name);
}

/**************************************************
 *
 *    user methods
 *
 */

static void
delread_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delread_t *this = (delread_t *)o;
  del_buf_t *buf = this->buf;

  this->time = fts_get_float_arg(ac, at, 0, 0.0f);

  if(this->buf)
    delread_set_delay(this);
}

/**************************************************
 *
 *    class
 *
 */

static fts_status_t
delread_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[4];

  fts_class_init(cl, sizeof(delread_t), 1, 1, 0);

  /* System methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  a[2] = fts_s_anything;
  a[3] = fts_s_number;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, delread_init, 4, a, 2);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, delread_delete, 0, 0);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, delread_put, 1, a);

  /* User methods */
  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, delread_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, delread_number, 1, a);

  /* DSP declarations */

  dsp_sig_inlet(cl, 0); /* for order forcing (shadock) */
  dsp_sig_outlet(cl, 0);        
  
  delread_function_symbol = fts_new_symbol("delread");
  dsp_declare_function(delread_function_symbol, ftl_delread);

  return fts_Success;
}

void
delread_config(void)
{
  fts_class_install(fts_new_symbol(CLASS_NAME),delread_instantiate);
}
