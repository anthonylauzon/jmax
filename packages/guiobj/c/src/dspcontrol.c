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
/*#include "../mess/fpe.h"*/
#include <ftsprivate/fpe.h>

/*
 * An ftl_data_t object implementing a set of control functions for the DSP engine.
 * It provide access to dac slip and fpe monitoring.
 * Later, we may add sampling rate, vector size, and the pause factor (?).
 * dsp on/off functions.
 * 
 * Currently, it does not implement any remote calls, just send the result
 * of the poll; the poll interval is given at the creation of the object.
 *
 * Todo: install the dsp control instance as listener for param sampling rate, param fifo_size,
 * and DSP on/off (how ?).
 */

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

typedef struct fts_dsp_control
{
  fts_object_t o;

  int poll_interval;

  /* Old state */

  int prev_dac_slip;
  int prev_invalid_fpe;
  int prev_divide0_fpe;
  int prev_overflow_fpe;
  int prev_denormalized_fpe;
} fts_dsp_control_t;


static void 
fts_dsp_control_poll(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_control_t *this = (fts_dsp_control_t *)o;
  fts_atom_t a[1];
  int dac_slip;
  int invalid_fpe;
  int divide0_fpe;
  int overflow_fpe;
  int denormalized_fpe;
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

  fts_timebase_add_call(fts_get_timebase(), o, fts_dsp_control_poll, 0, this->poll_interval);
}


static void fts_dsp_control_dsp_on_listener(void *listener, fts_symbol_t name,  const fts_atom_t *value)
{
  fts_object_t *this = (fts_object_t *)listener;
  fts_client_send_message(this, sym_client_dsp_on, 1, value);
}

static void fts_dsp_control_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_control_t *this = (fts_dsp_control_t *)o;

  ac--;
  at++;

  if (ac > 0 && fts_is_number(at))
    this->poll_interval = fts_get_number_int(at + 0);
  else
    this->poll_interval = 1000;

  this->prev_dac_slip = 0;
  this->prev_invalid_fpe = 0;
  this->prev_divide0_fpe = 0;
  this->prev_overflow_fpe = 0;
  this->prev_denormalized_fpe = 0;

  fts_timebase_add_call(fts_get_timebase(), o, fts_dsp_control_poll, 0, 0.0);
}

static void fts_dsp_control_upload(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t a[1];
  fts_dsp_control_t *this = (fts_dsp_control_t *)o;
  float sr;

  fts_param_add_listener(fts_s_dsp_on, this, fts_dsp_control_dsp_on_listener);
  fts_set_int(a, fts_param_get_int(fts_s_fifo_size, 0));
  fts_client_send_message((fts_object_t *)this, sym_fifo_size, 1, a);

  sr = fts_dsp_get_sample_rate();
  fts_set_int(a, (int)sr );
  fts_client_send_message((fts_object_t *)this, sym_sampling_rate, 1, a);
}

static void fts_dsp_control_fpe_start_collect(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_objectset_t *set = (fts_objectset_t *) fts_get_object(at);      
  fts_fpe_start_collect(set);
}

static void fts_dsp_control_fpe_stop_collect( fts_object_t *d, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_fpe_stop_collect();
}

static void fts_dsp_control_fpe_clear_collect( fts_object_t *d, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_fpe_empty_collection();
}

static void fts_dsp_control_remote_dsp_on( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if ((ac == 1) && fts_is_int(at))
    {
      fts_dsp_control_t *this = (fts_dsp_control_t *)o;

      fts_param_set(fts_s_dsp_on, at);
    }
}

static void fts_dsp_control_remote_dsp_print( fts_object_t *d, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_chain_post();
}

static void fts_dsp_control_remote_set_poll_interval( fts_object_t *d, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if ( (ac == 1) && fts_is_int( at))
    {
      fts_dsp_control_t *this = (fts_dsp_control_t *)d;

      this->poll_interval = fts_get_int(&at[0]);
    }
}

static void fts_dsp_control_set_check_nan( fts_object_t *d, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  /*if ( (ac == 1) && fts_is_int( at))
    {
    ftl_program_set_check_nan( dsp_get_current_dsp_chain(), fts_get_int( at));
    }*/
}

static void fts_dsp_control_restart( fts_object_t *d, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_restart();
}

static fts_status_t
dsp_control_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_dsp_control_t), 0, 0, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fts_dsp_control_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_upload, fts_dsp_control_upload);

  fts_method_define_varargs(cl, fts_SystemInlet, sym_dsp_on, fts_dsp_control_remote_dsp_on);
  fts_method_define_varargs(cl, fts_SystemInlet, sym_dsp_print, fts_dsp_control_remote_dsp_print);
  fts_method_define_varargs(cl, fts_SystemInlet, sym_start_collect, fts_dsp_control_fpe_start_collect);
  fts_method_define_varargs(cl, fts_SystemInlet, sym_stop_collect, fts_dsp_control_fpe_stop_collect);
  fts_method_define_varargs(cl, fts_SystemInlet, sym_clear_collect, fts_dsp_control_fpe_clear_collect);
  fts_method_define_varargs(cl, fts_SystemInlet, sym_check_nan, fts_dsp_control_set_check_nan);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("dsp_restart"), fts_dsp_control_restart);

  return fts_Success;
}


/*void fts_kernel_dspcontrol_init(void)*/
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
