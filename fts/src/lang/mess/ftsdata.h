#ifndef _FTSDATA_H_
#define _FTSDATA_H_

extern fts_data_class_t *fts_data_class_new( fts_symbol_t data_class_name);
extern void fts_data_class_define_export_function( fts_data_class_t *class, fts_data_export_fun_t export_fun);

extern void fts_data_class_define_remote_constructor( fts_data_class_t *class,
						      fts_data_remote_constructor_t constructor);

extern void fts_data_class_define_remote_destructor( fts_data_class_t *class,
						     fts_data_remote_destructor_t destructor);

extern void fts_data_class_define_function( fts_data_class_t *class, int key, fts_data_fun_t fun);


extern void fts_data_init( fts_data_t *d, fts_data_class_t *class);
extern void fts_data_export( fts_data_t *d);
extern void fts_data_delete( fts_data_t *d);

extern void fts_data_call( fts_data_t *d, int key, int ac, const fts_atom_t *at);
extern void fts_data_remote_call( fts_data_t *d, int key, int ac, const fts_atom_t *at);
extern void fts_data_start_remote_call( fts_data_t *d, int key);
extern void fts_data_end_remote_call(void);

/* Really necessary ? */
extern int fts_data_is_exported( fts_data_t *d);
extern int fts_data_get_id( fts_data_t *d);
extern void fts_data_id_init();
extern void fts_data_module_init();

#endif





