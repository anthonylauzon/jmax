#include "fts.h"
#include "delbuf.h"
#include "vd.h"


void ftl_delwrite(fts_word_t *argv)
{
  float *in = (float *)fts_word_get_ptr(argv + 0);
  del_buf_t *buf = (del_buf_t *)fts_word_get_ptr(argv + 1);
  long n_tick = fts_word_get_long(argv + 2);
  int i;
  int phase = buf->phase;

  for (i = 0; i < n_tick ; i++)
    buf->delay_line[phase + i] = in[i];

  if (buf->phase >= buf->ring_size) /* ring buffer wrap around */
    {
      buf->phase = n_tick;

      for (i = 0; i < n_tick ; i++)
	buf->delay_line[i] = in[i];
    }
  else
    buf->phase += n_tick;
}


void ftl_delread(fts_word_t *argv)
{
  float * restrict out = (float *) fts_word_get_ptr(argv + 0);
  del_buf_t * restrict buf = (del_buf_t *) fts_word_get_ptr(argv + 1);
  long n_tick = fts_word_get_long(argv + 2);
  long *del_time = (long *)fts_word_get_ptr(argv + 3);
  long del = *del_time; 
  int i;

  long phase;
  
  phase = buf->phase - del;
  
  if (phase < 0)
    phase += buf->ring_size; /* ring buffer wrap around */

  for (i = 0; i < n_tick ; i++)
    out[i] = buf->delay_line[phase + i];
}


void ftl_vd(fts_word_t *argv)
{
  float * restrict in = (float *)fts_word_get_ptr(argv);
  float * restrict out = (float *)fts_word_get_ptr(argv + 1);
  del_buf_t * restrict buf = (del_buf_t *) fts_word_get_ptr(argv + 2);
  vd_ctl_t * restrict ctl = (vd_ctl_t *) fts_word_get_ptr(argv + 3);
  long n_tick = fts_word_get_long(argv + 4);
  int write_advance = ctl->write_advance;
  float conv = ctl->conv;
  float * restrict del_ptr = buf->delay_line + buf->phase - write_advance;
  int min_delay = n_tick - write_advance + 2; /* n_tick minimum delay if read before write */
  int max_delay = buf->size;
  int i;

  for(i=0; i<n_tick; i++)
    {
      float f, *buf_ptr;
      float one_plus_f, one_minus_f, two_minus_f;
      float f_delay = in[i] * conv;
      long i_delay = f_delay;

      if (i_delay < min_delay)
	{
	  i_delay = min_delay;
	  f = 0.0f;
	}
      else if (i_delay >= max_delay)
	{
	  i_delay = max_delay;
	  f = 0.0f;
	}
      else
	f = f_delay - i_delay;

      buf_ptr = del_ptr + i - i_delay;
      if (buf_ptr < buf->delay_line + 2)
	buf_ptr += buf->ring_size;

      one_plus_f = 1.0f + f;
      one_minus_f = 1.0f - f;
      two_minus_f = 2.0f - f;

      out[i] = (
		0.5f * one_plus_f * two_minus_f * (one_minus_f * buf_ptr[0] + f * buf_ptr[-1])
		- .1666667f * f * one_minus_f * (two_minus_f * buf_ptr[1] + one_plus_f * buf_ptr[-2])
		);
    }
}

void ftl_vd_inplace(fts_word_t *argv)
{
  float * restrict sig = (float *)fts_word_get_ptr(argv);
  del_buf_t * restrict buf = (del_buf_t *) fts_word_get_ptr(argv + 1);
  vd_ctl_t * restrict ctl = (vd_ctl_t *) fts_word_get_ptr(argv + 2);
  long n_tick = fts_word_get_long(argv + 3);
  int write_advance = ctl->write_advance;
  float conv = ctl->conv;
  float * restrict del_ptr = buf->delay_line + buf->phase - write_advance;
  int min_delay = n_tick - write_advance + 2; /* n_tick minimum delay if read before write */
  int max_delay = buf->size;
  int i;

  for(i=0; i<n_tick; i++)
    {
      float f, *buf_ptr;
      float one_plus_f, one_minus_f, two_minus_f;
      float f_delay = sig[i] * conv;
      long i_delay = f_delay;

      if (i_delay < min_delay)
	{
	  i_delay = min_delay;
	  f = 0.0f;
	}
      else if (i_delay >= max_delay)
	{
	  i_delay = max_delay;
	  f = 0.0f;
	}
      else
	f = f_delay - i_delay;

      buf_ptr = del_ptr + i - i_delay;
      if (buf_ptr < buf->delay_line + 2)
	buf_ptr += buf->ring_size;

      one_plus_f = 1.0f + f;
      one_minus_f = 1.0f - f;
      two_minus_f = 2.0f - f;

      sig[i] = (
		0.5f * one_plus_f * two_minus_f * (one_minus_f * buf_ptr[0] + f * buf_ptr[-1])
		- .1666667f * f * one_minus_f * (two_minus_f * buf_ptr[1] + one_plus_f * buf_ptr[-2])
		);
    }
}

