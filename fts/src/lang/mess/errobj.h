#ifndef _ERROBJ_H_
#define _ERROBJ_H_

extern fts_object_t *fts_error_object_new(fts_object_t *patcher, int id, int ac, const fts_atom_t *at);
extern void fts_error_object_fit_inlet(fts_object_t *obj, int ninlet);
extern void fts_error_object_fit_outlet(fts_object_t *obj, int noutlet);



#endif
