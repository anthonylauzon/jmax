#ifndef _MESSP_H_
#define _MESSP_H_

/* extern declarations for private functions of the message system
   Do not included by fts objects or user modules, not exported or distributed
   with the .h files.
 */


/* Init functions of the various submodules */

extern void fts_symbols_init(void);
extern void fts_classes_init(void);
extern void fts_patcher_init(void);
extern void fts_patcher_shutdown(void);
extern void fts_events_init(void);
extern void class_config(void);
extern void send_config(void);
extern void fts_patparser_init(void);
extern void fts_mess_naming_init(void);

/* System classes config */

extern void fts_void_object_config(void);
extern void fts_selection_config(void);
extern void fts_clipboard_config(void);

/*
 * private class functions exported for the objects.c file
 */

extern fts_metaclass_t *fts_metaclass_get_by_name(fts_symbol_t name);

extern fts_class_t      *fts_class_instantiate(int ac, const fts_atom_t *at);
extern fts_class_mess_t *fts_class_mess_inlet_get(fts_inlet_decl_t *in, fts_symbol_t s,  int *panything);
extern fts_class_mess_t *fts_class_mess_get(fts_class_t *cl, int winlet, fts_symbol_t s, int *panything);

/*
 *  Functions to handle patcher/object relationship
 */

extern void fts_create_root_patcher(void);
extern void fts_patcher_add_object(fts_patcher_t *this, fts_object_t *obj);
extern void fts_patcher_remove_object(fts_patcher_t *this, fts_object_t *obj);
extern int fts_patcher_number_of_objects(fts_patcher_t *this);
extern void fts_patcher_assign_variable(fts_symbol_t name, fts_atom_t *value, void *data);

/* Functions for direct .pat loading support */

extern void fts_patparse_parse_patlex(fts_object_t *parent, fts_patlex_t *in);

extern void fts_patcher_reassign_inlets_outlets(fts_patcher_t *obj);
extern void fts_patcher_reassign_name(fts_patcher_t *this, fts_symbol_t new_name);

/* 
 * Property related private functions
 */

extern void fts_properties_free(fts_object_t *obj);

/* Doctor */

extern void fts_doctor_init();
extern fts_object_t *fts_call_object_doctor(fts_patcher_t *patcher, int ac, const fts_atom_t *at);
extern int fts_object_doctor_exists(fts_symbol_t class_name);


#endif
