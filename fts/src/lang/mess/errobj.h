#ifndef _ERROBJ_H_
#define _ERROBJ_H_

extern fts_object_t *fts_error_object_new(fts_patcher_t *parent, int ac, const fts_atom_t *at,
					  const char *format, ...);

extern void fts_error_object_fit_inlet(fts_object_t *obj, int ninlet);
extern void fts_error_object_fit_outlet(fts_object_t *obj, int noutlet);
extern void fts_recompute_errors(void);
extern void fts_do_recompute_errors(void);

#endif
