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
#include "deltable.h"
#include "delbuf.h"
#include "vd.h"
#include "sampunit.h"

/* buf_ptr is incremented each sample and corresponds to zero delay;
   at the outset it points into the buffer proper but at the end it
   could point n-1 samps past the logical end.  Since the offset is
   bounded below by n, bp - offset does not point past the end;
   in fact you can read four consecutive samples starting from there
   (but bp-offset might point before the beginning; this is guarded
   against in the loop.) */

static fts_symbol_t vd_dsp_symbol = 0;
static fts_symbol_t vd_inplace_dsp_symbol = 0;
static fts_symbol_t vd_miller_dsp_symbol = 0;

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
  fts_symbol_t unit;
  float max_incr;
  float millers_fix_del;
  ftl_data_t vd_data;
} vd_t;


static void
vd_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vd_t *this = (vd_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t unit = samples_unit_get_arg(ac, at, 1);

  this->name = name;

  if(!name)
    return;
  
  this->vd_data = ftl_data_new(ftl_vd_t);

  if(unit)
    {
      this->unit = unit;
      at += 2;
      ac -= 2;
    }
  else
    {
      this->unit = samples_unit_get_default();
      at++; 
      ac--;
    }

  if(ac && fts_is_number(at))
    {
      float max_incr = fts_get_number_float(at);

      if(max_incr < 1.0f)
	this->max_incr = 4.0f;
      else
	this->max_incr = max_incr;

      at++;
      ac--;
    }
  else
    this->max_incr = 4.0f;

  /* up argument given -> ISPW bug compatibillity (ftl_vd_miller)
     if the argument is not given the up sampling from ratio regarding to delwrite
     is implictly detected and the bug free ftl_vd executed in DSP time */
     
  if(ac && fts_is_int(at))
    {
      int up = fts_get_int(at);

      if(up < 0)
	this->millers_fix_del = 1.0;
      else if (up > 15) 
	this->millers_fix_del = 1.0f / 32768.0f;
      else
	this->millers_fix_del = 1.0f / (1 << up);
    }
  else
    this->millers_fix_del = 0.0f;


  delay_table_add_delreader(o, this->name);
  fts_dsp_add_object(o);
}


static void
vd_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vd_t *this = (vd_t *)o;


  if(this->name)
    {
      delay_table_remove_delreader(o, this->name);
      
      ftl_data_free(this->vd_data);
      
      fts_dsp_remove_object(o);
    }
}

/**************************************************
 *
 *    dsp
 *
 */

static void
vd_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vd_t *this = (vd_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  long n_tick = fts_dsp_get_input_size(dsp, 0);
  float sr = fts_dsp_get_input_srate(dsp, 0);
  ftl_vd_t *ftl = (ftl_vd_t *)ftl_data_get_ptr(this->vd_data);
  float conv;
  del_buf_t *buf;
  fts_atom_t argv[6];
  
  if(this->name)
    {
      buf = delay_table_get_delbuf(this->name);
      if(!buf){
	post("vd~: can't find delay line: %s\n", this->name);
	return;
      }

      if(delay_table_is_delwrite_scheduled(this->name))
	ftl->write_advance = (float)buf->n_tick;
      else
	ftl->write_advance = 0.0f;

      ftl->conv = samples_unit_convert(this->unit, 1.0f, sr);
      ftl->max_span = this->max_incr * (float)n_tick;
  
      if(this->millers_fix_del == 0.0f)
	{
	  if (fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0))
	    {
	      /* Inplace call */

	      fts_set_symbol(argv, fts_dsp_get_input_name(dsp, 0));
	      fts_set_pointer(argv + 1, buf);
	      fts_set_ftl_data(argv + 2, this->vd_data);
	      fts_set_int(argv + 3, n_tick);
	      fts_dsp_add_function(vd_inplace_dsp_symbol, 4, argv);
	    }
	  else
	    {
	      /* Standard call */

	      fts_set_symbol(argv, fts_dsp_get_input_name(dsp, 0));
	      fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
	      fts_set_pointer(argv + 2, buf);
	      fts_set_ftl_data(argv + 3, this->vd_data);
	      fts_set_int(argv + 4, n_tick);
	      fts_dsp_add_function(vd_dsp_symbol, 5, argv);
	    }
	}
      else
	{
	  /* call millers version */

	  fts_set_symbol(argv, fts_dsp_get_input_name(dsp, 0));
	  fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
	  fts_set_pointer(argv + 2, buf);
	  fts_set_ftl_data(argv + 3, this->vd_data);
	  fts_set_int(argv + 4, n_tick);
	  fts_set_float(argv + 5, this->millers_fix_del * (float)n_tick); /* write tick size (hopefully) */
	  fts_dsp_add_function(vd_miller_dsp_symbol, 6, argv);
	}

      delay_table_delreader_scheduled(this->name);
    }
}

/**************************************************
 *
 *    class
 *
 */

static fts_status_t
vd_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(vd_t), 2, 1, 0);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, vd_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, vd_delete);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_put, vd_put);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_inlet(cl, 1); 
  fts_dsp_declare_outlet(cl, 0);        
  
  vd_dsp_symbol = fts_new_symbol("ftl_vd");
  fts_dsp_declare_function(vd_dsp_symbol, ftl_vd);

  vd_inplace_dsp_symbol = fts_new_symbol("ftl_vd_inplace");
  fts_dsp_declare_function(vd_inplace_dsp_symbol, ftl_vd_inplace);

  vd_miller_dsp_symbol = fts_new_symbol("vd_miller");
  fts_dsp_declare_function(vd_miller_dsp_symbol, ftl_vd_miller);

  return fts_ok;
}

void
vd_config(void)
{
  fts_class_install(fts_new_symbol("vd~"), vd_instantiate);
}
