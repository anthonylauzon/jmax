#include "sys.h"
#include "lang/mess.h"
#include "lang/ftl.h"
#include "lang/dsp.h"
#include "runtime.h"	/* @@@@ what should we do ?? */

/*
 * An ftl_data_t object implementing a set of control functions for the DSP engine.
 * It provide access to dac slip and fpe monitoring.
 * Later, we may add sampling rate, vector size, and the pause factor (?).
 * dsp on/off functions.
 * 
 * Currently, it does not implement any remote calls, just send the result
 * of the poll; the poll interval is given at the creation of the object.
 */

#define DSP_CONTROL_DAC_SLIP_STATE      1
#define DSP_CONTROL_INVALID_FPE_STATE   2
#define DSP_CONTROL_DIVIDE0_FPE_STATE   3
#define DSP_CONTROL_OVERFLOW_FPE_STATE  4

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

  if (this->dac_slip_dev && fts_sig_dev_get_nerrors(this->dac_slip_dev) > 0)
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

  return (fts_data_t *) this;
}


static void
fts_dsp_control_delete(fts_data_t *d)
{
  fts_dsp_control_t *this = (fts_dsp_control_t *)d;

  fts_alarm_unarm(&(this->poll_alarm));
  fts_free((void *)this);
}



void fts_dsp_control_config(void)
{
  fts_dsp_control_data_class = fts_data_class_new( fts_new_symbol( "dspcontrol_data"));

  fts_data_class_define_remote_constructor( fts_dsp_control_data_class, fts_dsp_control_new);
  fts_data_class_define_remote_destructor(fts_dsp_control_data_class, fts_dsp_control_delete);
}







