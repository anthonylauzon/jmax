#ifndef _FTSDATA_H_
#define _FTSDATA_H_

#include "lang/mess/atoms.h"
#include "lang/mess/mess_types.h"

extern fts_data_class_t *fts_data_class_new( fts_symbol_t java_class_name);
extern void fts_data_class_define_function( fts_data_class_t *class, int key, fts_data_fun_t fun);

extern void fts_data_init( fts_data_t *d, fts_data_class_t *class);
extern void fts_data_export( fts_data_t *d);

extern void fts_data_call( fts_data_t *d, int key, int ac, fts_atom_t *at);
extern void fts_data_remote_call( fts_data_t *d, int key, int ac, fts_atom_t *at);

/* Really necessary ? */
extern int fts_data_get_id( fts_data_t *d);

#endif

