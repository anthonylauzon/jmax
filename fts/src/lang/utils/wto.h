#ifndef _WTO_H_
#define _WTO_H_

#include "fts.h"

typedef struct _fts_wto
{
  float *table;
  long size;
  double index;
  double incr;
} fts_wto_t;

#define fts_wto_set_phase(wto, phase) \
  (wto->index = (double)wto->size * phase)

#define fts_wto_set_phase_incr(wto, phase_incr) \
  (wto->incr = (double)wto->size * phase_incr) /* phase_incr = freq / samplerate */

extern void fts_wto_run(fts_wto_t *wto, float *out, long n);
extern void fts_wto_run_freq(fts_wto_t *wto, float *freq, float *out, long n);

#endif
