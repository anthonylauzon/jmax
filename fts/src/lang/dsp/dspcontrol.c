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


#include <fts/sys.h>
#include <fts/lang/mess.h>
#include <fts/lang/datalib.h>
#include <fts/lang/ftl.h>
#include <fts/lang/dsp.h>
#include <fts/runtime.h>
#include "../mess/fpe.h"

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

#define DSP_CONTROL_DAC_SLIP_STATE      1
#define DSP_CONTROL_INVALID_FPE_STATE   2
#define DSP_CONTROL_DIVIDE0_FPE_STATE   3
#define DSP_CONTROL_OVERFLOW_FPE_STATE  4
#define DSP_CONTROL_DENORMALIZED_FPE_STATE  41
#define DSP_CONTROL_SAMPLING_RATE       5
#define DSP_CONTROL_FIFO_SIZE           6
#define DSP_CONTROL_DSP_ON              7

#define DSP_CONTROL_FPE_START_COLLECT   8
#define DSP_CONTROL_FPE_STOP_COLLECT    9
#define DSP_CONTROL_FPE_CLEAR_COLLECT  10

#define DSP_CONTROL_DSP_PRINT          11
#define DSP_CONTROL_SET_POLL_INTERVAL  12

#define DSP_CONTROL_SET_CHECK_NAN      13


static fts_data_class_t *fts_dsp_control_data_class = 0;

typedef struct fts_dsp_control
{
  fts_data_t dataobj;

  fts_alarm_t poll_alarm;
  int poll_interval;

  /* Old state */

  int prev_dac_slip;
  int prev_invalid_fpe;
  int prev_divide0_fpe;
  int prev_overflow_fpe;
  int prev_denormalized_fpe;
} fts_dsp_control_t;


static void fts_dsp_control_poll(fts_alarm_t *alarm, void *data)
{
  fts_dsp_control_t *this = (fts_dsp_control_t *)data;
  fts_atom_t a;
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
      fts_set_int(&a, dac_slip);
      fts_data_remote_call((fts_data_t *)this, DSP_CONTROL_DAC_SLIP_STATE, 1, &a);
    }

  fpe_state = fts_check_fpe();

  invalid_fpe  = ((fpe_state & FTS_INVALID_FPE) ? 1 : 0);

  if (invalid_fpe != this->prev_invalid_fpe)
    {
      this->prev_invalid_fpe = invalid_fpe;

      fts_set_int(&a, invalid_fpe);
      fts_data_remote_call((fts_data_t *)this, DSP_CONTROL_INVALID_FPE_STATE, 1, &a);
    }

  divide0_fpe  = ((fpe_state & FTS_DIVIDE0_FPE) ? 1 : 0);

  if (divide0_fpe != this->prev_divide0_fpe)
    {
      this->prev_divide0_fpe = divide0_fpe;

      fts_set_int(&a, divide0_fpe);
      fts_data_remote_call((fts_data_t *)this, DSP_CONTROL_DIVIDE0_FPE_STATE, 1, &a);
    }

  overflow_fpe = ((fpe_state & FTS_OVERFLOW_FPE) ? 1 : 0);

  if (overflow_fpe != this->prev_overflow_fpe)
    {
      this->prev_overflow_fpe = overflow_fpe;

      fts_set_int(&a, overflow_fpe);
      fts_data_remote_call((fts_data_t *)this, DSP_CONTROL_OVERFLOW_FPE_STATE, 1, &a);
    }

/*    denormalized_fpe = ((fpe_state & FTS_DENORMALIZED_FPE) ? 1 : 0); */

/*    if (denormalized_fpe != this->prev_denormalized_fpe) */
/*      { */
/*        this->prev_denormalized_fpe = denormalized_fpe; */

/*        fts_set_int(&a, denormalized_fpe); */
/*        fts_data_remote_call((fts_data_t *)this, DSP_CONTROL_DENORMALIZED_FPE_STATE, 1, &a); */
/*      } */

  fts_alarm_set_delay(&(this->poll_alarm), this->poll_interval);
}


static void fts_dsp_control_dsp_on_listener(void *listener, fts_symbol_t name,  const fts_atom_t *value)
{
  fts_dsp_control_t *this = (fts_dsp_control_t *)listener;

  fts_data_remote_call((fts_data_t *)this, DSP_CONTROL_DSP_ON, 1, value);
}


static fts_data_t *fts_dsp_control_new(int ac, const fts_atom_t *at)
{
  fts_dsp_control_t *this = (fts_dsp_control_t *)fts_malloc(sizeof(fts_dsp_control_t));

  fts_data_init((fts_data_t *) this, fts_dsp_control_data_class);

  if (ac > 0)
    this->poll_interval = fts_get_int(&at[0]);
  else
    this->poll_interval = 1000;

  this->prev_dac_slip = 0;
  this->prev_invalid_fpe = 0;
  this->prev_divide0_fpe = 0;
  this->prev_overflow_fpe = 0;
  this->prev_denormalized_fpe = 0;

  fts_alarm_init(&(this->poll_alarm), 0, fts_dsp_control_poll, this);
  fts_alarm_set_delay(&(this->poll_alarm), this->poll_interval);

  fts_param_add_listener(fts_s_dsp_on, this, fts_dsp_control_dsp_on_listener);

  return (fts_data_t *) this;
}


static void fts_dsp_control_export_fun(fts_data_t *d)
{
  fts_atom_t a;
  fts_dsp_control_t *this = (fts_dsp_control_t *)d;
  float sr;

  fts_set_int(&a, fts_param_get_int(fts_s_fifo_size, 0));
  fts_data_remote_call((fts_data_t *)this, DSP_CONTROL_FIFO_SIZE, 1, &a);

  sr = fts_dsp_get_sample_rate();
  fts_set_int(&a, (int)sr );
  fts_data_remote_call((fts_data_t *)this, DSP_CONTROL_SAMPLING_RATE, 1, &a);
}



static void
fts_dsp_control_delete(fts_data_t *d)
{
  fts_dsp_control_t *this = (fts_dsp_control_t *)d;

  fts_alarm_reset(&(this->poll_alarm));
  fts_free((void *)this);
}


static void fts_dsp_control_fpe_start_collect( fts_data_t *d, int ac, const fts_atom_t *at)
{
  if ((ac == 1) && fts_is_data(at))
    {
      fts_object_set_t *set;

      set = (fts_object_set_t *) fts_get_data(at);
      
      fts_fpe_start_collect(set);
    }
}


static void fts_dsp_control_fpe_stop_collect( fts_data_t *d, int ac, const fts_atom_t *at)
{
  fts_fpe_stop_collect();
}

static void fts_dsp_control_fpe_clear_collect( fts_data_t *d, int ac, const fts_atom_t *at)
{
  fts_fpe_empty_collection();
}


static void fts_dsp_control_remote_dsp_on( fts_data_t *d, int ac, const fts_atom_t *at)
{
  if ((ac == 1) && fts_is_int(at))
    {
      fts_dsp_control_t *this = (fts_dsp_control_t *)d;

      fts_param_set(fts_s_dsp_on, at);
    }
}


static void fts_dsp_control_remote_dsp_print( fts_data_t *d, int ac, const fts_atom_t *at)
{
  dsp_chain_post();
}

static void fts_dsp_control_remote_set_poll_interval( fts_data_t *d, int ac, const fts_atom_t *at)
{
  if ( (ac == 1) && fts_is_int( at))
    {
      fts_dsp_control_t *this = (fts_dsp_control_t *)d;

      this->poll_interval = fts_get_int(&at[0]);
    }
}

static void fts_dsp_control_set_check_nan( fts_data_t *d, int ac, const fts_atom_t *at)
{
  if ( (ac == 1) && fts_is_int( at))
    {
      ftl_program_set_check_nan( dsp_get_current_dsp_chain(), fts_get_int( at));
    }
}

void fts_dsp_control_config( void)
{
  fts_dsp_control_data_class = fts_data_class_new( fts_new_symbol( "dspcontrol_data"));

  fts_data_class_define_remote_constructor( fts_dsp_control_data_class, fts_dsp_control_new);

  fts_data_class_define_export_function(fts_dsp_control_data_class, fts_dsp_control_export_fun);

  fts_data_class_define_remote_destructor(fts_dsp_control_data_class, fts_dsp_control_delete);

  fts_data_class_define_function(fts_dsp_control_data_class, DSP_CONTROL_FPE_START_COLLECT,
				 fts_dsp_control_fpe_start_collect);
  fts_data_class_define_function(fts_dsp_control_data_class, DSP_CONTROL_FPE_STOP_COLLECT,
				 fts_dsp_control_fpe_stop_collect);
  fts_data_class_define_function(fts_dsp_control_data_class, DSP_CONTROL_FPE_CLEAR_COLLECT,
				 fts_dsp_control_fpe_clear_collect);

  fts_data_class_define_function(fts_dsp_control_data_class, DSP_CONTROL_DSP_ON,
				 fts_dsp_control_remote_dsp_on);

  fts_data_class_define_function(fts_dsp_control_data_class, DSP_CONTROL_DSP_PRINT,
				 fts_dsp_control_remote_dsp_print);

  fts_data_class_define_function(fts_dsp_control_data_class, DSP_CONTROL_SET_POLL_INTERVAL,
				 fts_dsp_control_remote_set_poll_interval);

  fts_data_class_define_function( fts_dsp_control_data_class, DSP_CONTROL_SET_CHECK_NAN,
				  fts_dsp_control_set_check_nan);
}





