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
#include <ftsprivate/fpe.h>

fts_symbol_t dspcontrol_symbol = 0;
fts_symbol_t sym_dsp_on = 0;
fts_symbol_t sym_dsp_print = 0;
fts_symbol_t sym_dac_slip = 0;
fts_symbol_t sym_invalid_fpe = 0;
fts_symbol_t sym_divide_by_zero_fpe = 0;
fts_symbol_t sym_overflow_fpe = 0;
fts_symbol_t sym_client_dsp_on = 0;
fts_symbol_t sym_fifo_size = 0;
fts_symbol_t sym_sampling_rate = 0;
fts_symbol_t sym_start_collect = 0;
fts_symbol_t sym_stop_collect = 0;
fts_symbol_t sym_clear_collect = 0;
fts_symbol_t sym_check_nan = 0;

typedef struct dsp_control
{
  fts_object_t o;
  int poll_interval;
  int prev_dac_slip;
  int prev_invalid_fpe;
  int prev_divide0_fpe;
  int prev_overflow_fpe;
  int prev_denormalized_fpe;
} dsp_control_t;


static void 
dsp_control_poll(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_control_t *this = (dsp_control_t *)o;
  fts_atom_t a[1];
  int dac_slip;
  int invalid_fpe;
  int divide0_fpe;
  int overflow_fpe;
  unsigned int fpe_state;

  if (fts_dsp_is_running() && fts_audioport_report_xrun())
    dac_slip = 1;
  else
    dac_slip = 0;

  if (dac_slip != this->prev_dac_slip)
    {
      this->prev_dac_slip = dac_slip;
      fts_set_int(a, dac_slip);
      fts_client_send_message((fts_object_t *)this, sym_dac_slip, 1, a);
    }

  fpe_state = fts_check_fpe();

  invalid_fpe  = ((fpe_state & FTS_INVALID_FPE) ? 1 : 0);

  if (invalid_fpe != this->prev_invalid_fpe)
    {
      this->prev_invalid_fpe = invalid_fpe;

      fts_set_int(a, invalid_fpe);
      fts_client_send_message((fts_object_t *)this, sym_invalid_fpe, 1, a);
    }

  divide0_fpe  = ((fpe_state & FTS_DIVIDE0_FPE) ? 1 : 0);

  if (divide0_fpe != this->prev_divide0_fpe)
    {
      this->prev_divide0_fpe = divide0_fpe;

      fts_set_int(a, divide0_fpe);
      fts_client_send_message((fts_object_t *)this, sym_divide_by_zero_fpe, 1, a);
    }

  overflow_fpe = ((fpe_state & FTS_OVERFLOW_FPE) ? 1 : 0);

  if (overflow_fpe != this->prev_overflow_fpe)
    {
      this->prev_overflow_fpe = overflow_fpe;

      fts_set_int(a, overflow_fpe);
      fts_client_send_message((fts_object_t *)this, sym_overflow_fpe, 1, a);
    }

/*    denormalized_fpe = ((fpe_state & FTS_DENORMALIZED_FPE) ? 1 : 0); */

/*    if (denormalized_fpe != this->prev_denormalized_fpe) */
/*      { */
/*        this->prev_denormalized_fpe = denormalized_fpe; */

/*        fts_set_int(&a, denormalized_fpe); */
/*        fts_data_remote_call((fts_data_t *)this, DSP_CONTROL_DENORMALIZED_FPE_STATE, 1, &a); */
/*      } */

  fts_timebase_add_call(fts_get_timebase(), o, dsp_control_poll, 0, this->poll_interval);
}


static void 
dsp_control_dsp_active(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_t *this = (fts_object_t *)o;
  fts_client_send_message(this, sym_client_dsp_on, 1, at);
}

static void 
dsp_control_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_control_t *this = (dsp_control_t *)o;

  if (ac > 0 && fts_is_number(at))
    this->poll_interval = fts_get_number_int(at + 0);
  else
    this->poll_interval = 1000;

  this->prev_dac_slip = 0;
  this->prev_invalid_fpe = 0;
  this->prev_divide0_fpe = 0;
  this->prev_overflow_fpe = 0;
  this->prev_denormalized_fpe = 0;

  fts_timebase_add_call(fts_get_timebase(), o, dsp_control_poll, 0, 0.0);

  /* listen to dsp active param */
  fts_dsp_active_add_listener(o, dsp_control_dsp_active);
}

static void 
dsp_control_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_active_remove_listener(o);
}

static void 
dsp_control_upload(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t a[1];
  dsp_control_t *this = (dsp_control_t *)o;
  float sr;

  fts_set_int(a, 9999999);
  fts_client_send_message((fts_object_t *)this, sym_fifo_size, 1, a);

  sr = fts_dsp_get_sample_rate();
  fts_set_int(a, (int)sr );

  fts_client_send_message((fts_object_t *)this, sym_sampling_rate, 1, a);
}

static void 
dsp_control_fpe_start_collect(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_objectset_t *set = (fts_objectset_t *) fts_get_object(at);      
  fts_fpe_start_collect(set);
}

static void 
dsp_control_fpe_stop_collect( fts_object_t *d, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_fpe_stop_collect();
}

static void 
dsp_control_fpe_clear_collect( fts_object_t *d, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_fpe_empty_collection();
}

static void 
dsp_control_remote_dsp_on( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (ac == 1 && fts_is_int(at))
    {
      int active = fts_get_int(at);
      
      if(active)
	fts_dsp_activate();
      else
	fts_dsp_desactivate();
    }
}

static void 
dsp_control_remote_dsp_print( fts_object_t *d, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_chain_post();
}

static void 
dsp_control_remote_set_poll_interval( fts_object_t *d, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if ( (ac == 1) && fts_is_int( at))
    {
      dsp_control_t *this = (dsp_control_t *)d;

      this->poll_interval = fts_get_int(&at[0]);
    }
}

static void 
dsp_control_set_check_nan( fts_object_t *d, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  /*if ( (ac == 1) && fts_is_int( at))
    ftl_program_set_check_nan( dsp_get_current_dsp_chain(), fts_get_int( at));
  */
}

static void 
dsp_control_restart( fts_object_t *d, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_restart();
}

static void
dsp_control_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(dsp_control_t), dsp_control_init, NULL);

  fts_class_method_varargs(cl, fts_s_upload, dsp_control_upload);

  fts_class_method_varargs(cl, sym_dsp_on, dsp_control_remote_dsp_on);
  fts_class_method_varargs(cl, sym_dsp_print, dsp_control_remote_dsp_print);
  fts_class_method_varargs(cl, sym_start_collect, dsp_control_fpe_start_collect);
  fts_class_method_varargs(cl, sym_stop_collect, dsp_control_fpe_stop_collect);
  fts_class_method_varargs(cl, sym_clear_collect, dsp_control_fpe_clear_collect);
  fts_class_method_varargs(cl, sym_check_nan, dsp_control_set_check_nan);
  fts_class_method_varargs(cl, fts_new_symbol("dsp_restart"), dsp_control_restart);
}

void dspcontrol_config( void)
{
  dspcontrol_symbol = fts_new_symbol("__dspcontrol");
  sym_dsp_on = fts_new_symbol("dsp_on");
  sym_dsp_print = fts_new_symbol("dsp_print");
  sym_dac_slip = fts_new_symbol("setDACSlipState");
  sym_invalid_fpe = fts_new_symbol("setInvalidFpeState");
  sym_divide_by_zero_fpe = fts_new_symbol("setDivideByZeroFpeState");
  sym_overflow_fpe = fts_new_symbol("setOverflowFpeState");
  sym_client_dsp_on = fts_new_symbol("setDspOnState");
  sym_fifo_size = fts_new_symbol("setFifoSize");
  sym_sampling_rate = fts_new_symbol("setSamplingRate");
  sym_start_collect = fts_new_symbol("fts_start_collect");
  sym_stop_collect = fts_new_symbol("fpe_stop_collecting");
  sym_clear_collect = fts_new_symbol("fpe_clear_collecting");
  sym_check_nan = fts_new_symbol("set_check_nan");

  fts_class_install(dspcontrol_symbol, dsp_control_instantiate);
}
