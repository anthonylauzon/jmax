#include "sys.h"
#include "lang/mess.h"
#include "lang/datalib.h"
#include "lang/ftl.h"
#include "lang/dsp.h"
#include "runtime.h"	/* @@@@ what should we do ?? */
#include "lang/mess/fpe.h"

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
#define DSP_CONTROL_SAMPLING_RATE       5
#define DSP_CONTROL_FIFO_SIZE           6
#define DSP_CONTROL_DSP_ON              7

#define DSP_CONTROL_FPE_START_COLLECT   8
#define DSP_CONTROL_FPE_STOP_COLLECT    9
#define DSP_CONTROL_FPE_CLEAR_COLLECT  10

#define DSP_CONTROL_DSP_PRINT          11


extern fts_dev_t * fts_dsp_get_dac_slip_dev();

static fts_data_class_t *fts_dsp_control_data_class = 0;

typedef struct fts_dsp_control
{
  fts_data_t dataobj;

  fts_dev_t *dac_slip_dev;
  fts_alarm_t poll_alarm;
  int poll_interval;

  /* Old state */

  int prev_dac_slip;
  int prev_invalid_fpe;
  int prev_divide0_fpe;
  int prev_overflow_fpe;
} fts_dsp_control_t;


static void fts_dsp_control_poll(fts_alarm_t *alarm, void *data)
{
  fts_dsp_control_t *this = (fts_dsp_control_t *)data;
  fts_atom_t a;
  int dac_slip;
  int invalid_fpe;
  int divide0_fpe;
  int overflow_fpe;
  unsigned int fpe_state;

  if (this->dac_slip_dev &&  dsp_is_running() && fts_sig_dev_get_nerrors(this->dac_slip_dev) > 0)
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

  fts_alarm_init(&(this->poll_alarm), 0, fts_dsp_control_poll, this);
  fts_alarm_set_cycle(&(this->poll_alarm), this->poll_interval);
  fts_alarm_arm(&(this->poll_alarm));

  this->dac_slip_dev = fts_dsp_get_dac_slip_dev();

  fts_param_add_listener(fts_s_dsp_on, this, fts_dsp_control_dsp_on_listener);

  return (fts_data_t *) this;
}


static void fts_dsp_control_export_fun(fts_data_t *d)
{
  fts_atom_t a;
  fts_dsp_control_t *this = (fts_dsp_control_t *)d;

  fts_set_int(&a, fts_param_get_int(fts_s_fifo_size, 0));
  fts_data_remote_call((fts_data_t *)this, DSP_CONTROL_FIFO_SIZE, 1, &a);

  fts_set_int(&a, (int) fts_param_get_float(fts_s_sampling_rate, 44100.0));
  fts_data_remote_call((fts_data_t *)this, DSP_CONTROL_SAMPLING_RATE, 1, &a);
}



static void
fts_dsp_control_delete(fts_data_t *d)
{
  fts_dsp_control_t *this = (fts_dsp_control_t *)d;

  fts_alarm_unarm(&(this->poll_alarm));
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
      int on;

      fts_param_set_by(fts_s_dsp_on, at, this);
    }
}


static void fts_dsp_control_remote_dsp_print( fts_data_t *d, int ac, const fts_atom_t *at)
{
  dsp_chain_post();
}


void fts_dsp_control_config(void)
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
}







