#include "fts.h"
#include "delbuf.h"
#include "vd.h"


void
ftl_delwrite(fts_word_t *argv)
{
  float *in = (float *)fts_word_get_ptr(argv + 0);
  del_buf_t *buf = (del_buf_t *)fts_word_get_ptr(argv + 1);
  long n_tick = fts_word_get_long(argv + 2);
  
  fts_vecx_fcpy(in, buf->delay_line + buf->phase, n_tick);

  if (buf->phase >= buf->ring_size) /* ring buffer wrap around */
    {
      buf->phase = n_tick;
      fts_vecx_fcpy(in, buf->delay_line, n_tick);
    }
  else
    buf->phase += n_tick;
}


void
ftl_delread(fts_word_t *argv)
{
  float *out = (float *) fts_word_get_ptr(argv + 0);
  del_buf_t *buf = (del_buf_t *) fts_word_get_ptr(argv + 1);
  long n_tick = fts_word_get_long(argv + 2);
  long *del_time = (long *)fts_word_get_ptr(argv + 3);
  long del = *del_time; 

  long phase;
  
  phase = buf->phase - del;
  
  if (phase < 0)
    phase += buf->ring_size; /* ring buffer wrap around */

  fts_vecx_fcpy(buf->delay_line + phase, out, n_tick);
}


void
ftl_vd(fts_word_t *argv)
{
  float *in = (float *)fts_word_get_ptr(argv);
  float *out = (float *)fts_word_get_ptr(argv + 1);
  del_buf_t *buf = (del_buf_t *) fts_word_get_ptr(argv + 2);
  vd_ctl_t *x = (vd_ctl_t *) fts_word_get_ptr(argv + 3);
  long n_tick = fts_word_get_long(argv + 4);
  long i;
  long min_offset, max_offset;
  float *del_ptr;

  del_ptr = buf->delay_line + buf->phase;
  min_offset = n_tick + 2;
  max_offset = buf->size + n_tick;
  
  for(i=0; i<n_tick; i++)
    {
      float f, *buf_ptr;
      float one_plus_f, one_minus_f, two_minus_f;
      float f_offset = (in[i] - x->delonset) * x->conv;
      long i_offset = f_offset;

      if (i_offset < min_offset)
	{
	  i_offset = min_offset;
	  f = 0.0f;
	}
      else if (i_offset >= max_offset)
	{
	  i_offset = max_offset;
	  f = 0.0f;
	}
      else
	f = f_offset - i_offset;

      one_plus_f = 1.0f + f;
      one_minus_f = 1.0f - f;
      two_minus_f = 2.0f - f;

      buf_ptr = del_ptr + i - i_offset;
      if (buf_ptr < buf->delay_line + 2)
	buf_ptr += buf->ring_size;

      out[i] = (
		0.5f * one_plus_f * two_minus_f * (one_minus_f * buf_ptr[0] + f * buf_ptr[-1])
		- .1666667f * f * one_minus_f * (two_minus_f * buf_ptr[1] + one_plus_f * buf_ptr[-2])
		);
    }
}

