#ifndef _DSPFPE_H_
#define _DSPFPE_H_

/*
 * FPE handling and collection support 
 */

extern void fts_dsp_fpe_start_collect(fts_object_set_t *set);
extern void fts_dsp_fpe_empty_collection();
extern void fts_dsp_fpe_stop_collect();

#endif
