#ifndef _FPE_H_
#define _FPE_H_

/*
 * FPE handling and collection support 
 */

extern void fts_fpe_start_collect(fts_object_set_t *set);
extern void fts_fpe_empty_collection();
extern void fts_fpe_stop_collect();

#endif
