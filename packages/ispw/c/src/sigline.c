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
 */

#include <fts/fts.h>
#include <ftsconfig.h>

static fts_symbol_t sigline_function = 0;
static fts_symbol_t sigline_64_function = 0;

typedef struct
{
  double value;
  double incr;
  float target;
  int steps;
} line_control_t;


typedef struct
{
  fts_object_t _o;
  float time;
  int set;
  int vecsize;
  float srate;
  ftl_data_t ftl_data;
} sigline_t;

static void ftl_line(fts_word_t *argv)
{
  float * restrict fp = (float *)fts_word_get_ptr(argv);
  line_control_t * restrict lctl = (line_control_t *)fts_word_get_ptr(argv+1);
  long int n = fts_word_get_int(argv+2);

  if (lctl->steps > 0)
    {
      int i;
      double value = lctl->value;
      double incr = lctl->incr;

      for (i=0; i<n; i++)
	{
	  value += incr;
	  fp[i] = value;
	}

      lctl->value = value;
      lctl->steps -= n;
    }
  else
    {
      int i;
      float target = lctl->target;

      for (i=0; i<n; i++)
	fp[i] = target;

      lctl->value = target;
    }
}


static void ftl_line_64(fts_word_t *argv)
{
  float * restrict fp = (float *)fts_word_get_ptr(argv);
  line_control_t * restrict lctl = (line_control_t *)fts_word_get_ptr(argv+1);
  
  if (lctl->steps > 0)
    {
      int i;
      double value = lctl->value;
      double incr = lctl->incr;

      for (i=0; i<64; i++) 
	{
	  value += incr;
	  fp[i] = value;
 	}

      lctl->value = value;
      lctl->steps -= 64;
    }
  else
    {
      int i;
      float target = lctl->target;

      for(i=0; i<64; i++)
	fp[i] = target;

      lctl->value = target;
    }
}

static void
sigline_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigline_t *this = (sigline_t *)o;
  line_control_t* ctl = ftl_data_get_ptr(this->ftl_data);
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  fts_atom_t argv[3];

  this->srate = fts_dsp_get_output_srate(dsp, 0);
  this->vecsize = fts_dsp_get_output_size(dsp, 0);

  if (this->vecsize == 64)
    {
      fts_set_symbol (argv, fts_dsp_get_output_name(dsp, 0));
      fts_set_ftl_data(argv+1, this->ftl_data);
      dsp_add_funcall(sigline_64_function, 2, argv);
    }
  else
    {
      fts_set_symbol (argv, fts_dsp_get_output_name(dsp, 0));
      fts_set_ftl_data(argv+1, this->ftl_data);
      fts_set_int   (argv+2, fts_dsp_get_output_size(dsp, 0));
      dsp_add_funcall(sigline_function, 3, argv);
    }
}


static void
sigline_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigline_t *this = (sigline_t *)o;
  int down = fts_get_int_arg(ac, at, 1, 0);
  double df;
  float f;
  int i;

  this->vecsize = fts_dsp_get_tick_size();

  this->set = 0;
  this->srate = fts_dsp_get_sample_rate();

  this->ftl_data = ftl_data_new(line_control_t);

  df = 0.0f;
  f = 0.0f;
  ftl_data_set(line_control_t, this->ftl_data, value, &df);
  ftl_data_set(line_control_t, this->ftl_data, target, &f);
  ftl_data_set(line_control_t, this->ftl_data, incr, &df);
  i = 0;
  ftl_data_set(line_control_t, this->ftl_data, steps, &i);

  if (down > 0)
    {
      fts_atom_t a;

      fts_set_int(&a, down);

      fts_object_put_prop(o, fts_s_dsp_downsampling, &a);
    }

  dsp_list_insert(o);
}

static void
sigline_set_target(sigline_t *this, float target)
{
  line_control_t* ctl = ftl_data_get_ptr(this->ftl_data);

  if (this->time > 0.0f)
    {
      int n_ticks = this->time * 0.001f * this->srate / this->vecsize + 0.5;
      int steps = n_ticks * this->vecsize;

      ctl->target = target;
      ctl->incr = (target - ctl->value) / steps; 
      ctl->steps = steps;
    }
  else
    {
      ctl->value = target;
      ctl->target = target;
      ctl->incr = 0.0;
      ctl->steps = 0;
    }

  this->time = 0.0;
}

static void
sigline_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigline_t *this = (sigline_t *)o;
  float target = fts_get_number_float(at);

  sigline_set_target((sigline_t *)o, target);
}


static void
sigline_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigline_t *this = (sigline_t *)o;

  this->time = fts_get_float_arg(ac, at, 0, 0.0f);
}

static void
sigline_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if ((ac >= 2) && fts_is_number(&at[1]))
    sigline_number_1(o, 0, 0, 1, at+1);

  if ((ac >= 1) && fts_is_number(&at[0]))
    sigline_set_target((sigline_t *)o, fts_get_float_arg(ac, at, 0, 0.0f));
}

static void
sigline_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigline_t *this = (sigline_t *)o;

  ftl_data_free(this->ftl_data);
  dsp_list_remove(o);
}


static fts_status_t
sigline_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(sigline_t), 2, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, sigline_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, sigline_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, sigline_put);
  
  fts_method_define_varargs(cl, 0, fts_s_list, sigline_list);

  fts_method_define_varargs(cl, 0, fts_s_int, sigline_number);
  fts_method_define_varargs(cl, 0, fts_s_float, sigline_number);
  
  fts_method_define_varargs(cl, 1, fts_s_float, sigline_number_1);
  fts_method_define_varargs(cl, 1, fts_s_int, sigline_number_1);
  
  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);
  
  sigline_function = fts_new_symbol("ftl_line");
  dsp_declare_function(sigline_function, ftl_line);

  sigline_64_function = fts_new_symbol("ftl_line64");
  dsp_declare_function(sigline_64_function, ftl_line_64);

  return fts_Success;
}

void
sigline_config(void)
{
  fts_class_install(fts_new_symbol("line~"),sigline_instantiate);
}
