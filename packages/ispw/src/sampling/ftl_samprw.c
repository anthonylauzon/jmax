#include "fts.h"
#include "sampbuf.h"
#include "sampfilt.h"
#include "sampwrite.h"
#include "sampread.h"


void ftl_sampwrite(fts_word_t *argv)
{
  float *in = (float *)fts_word_get_ptr(argv + 0);
  sampwrite_ctl_t *ctl = (sampwrite_ctl_t *)fts_word_get_ptr(argv + 1);
  long n_tick = fts_word_get_long(argv + 2);
  long onset = ctl->onset;
  sampbuf_t *buf = ctl->buf;
  float *xp = buf->samples + onset;
  long n_left = buf->size + GUARDPTS - onset;

  if(n_left <= 0) return;
  if(n_left >= n_tick){
     fts_vecx_fcpy(in, buf->samples + onset, n_tick);
  }else{
     fts_vec_fcpy(in, buf->samples + onset, n_left);
  }
  ctl->onset = onset + n_tick;
}


void ftl_sampread(fts_word_t *argv)
{
  float *in1 = (float *)fts_word_get_ptr(argv);
  float *out = (float *)fts_word_get_ptr(argv + 1);
  sampread_ctl_t *ctl = (sampread_ctl_t *)fts_word_get_ptr(argv + 2);
  long n = fts_word_get_long(argv + 3);
  long addr1, addr2, n2;
  float f1, f2, extent, *fp;

  sampbuf_t *buf = ctl->buf;
  float *sp;
  float tbuf[512];

  /* add by ev for portage */
  in1 += n-1;

  if(!buf) goto zero;

  f1 = ctl->last_in;
  f2 = in1[0] * ctl->conv;
  ctl->last_in = f2;

  if(f2 >= f1){        /* reading forward */
    if(f1 < 0 || f2 >= buf->size - 6 || (extent = f2 - f1) > ctl->max_extent) goto zero;
    n2 = f1;
    sp = buf->samples + n2;
    addr1 = (f1 - n2) * 65536.0f;
    addr2 = 65536.0f * extent * ctl->inv_n;
  }else{ /* reading backward */
    if(f2 < 0 || f1 >= buf->size - 6 || (extent = f1 - f2) > ctl->max_extent) goto zero;
    n2 = f2 - .5f;
    sp = buf->samples + n2;
    addr1 = (f1 - n2) * 65536.0f;
    addr2 = - (long)(65536.0f * extent * ctl->inv_n);
    /* are you sure this won't go past the bottom? */
  }
  n2 = ((long)extent) + 5;
  
  for(fp = tbuf; n2--;) *fp++ = *sp++;
  
  while(n--){
    sampfilt_t *t = sampfilt_tab + ((addr1 >> 8) & 255);
    fp = tbuf + (addr1 >> 16);
    *out++ = t->f1 * fp[0] + t->f2 * fp[1] + t->f3 * fp[2] + t->f4 * fp[3];
    addr1 += addr2;
  }
  return;
  
 zero:
  while(n--) *out++ = 0;
}

