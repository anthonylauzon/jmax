/*
 *                      Copyright (c) 1995 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *
 *  Eric Viara for Ircam, January 1995
 *
 */

#ifndef _CLASSES_H_
#define _CLASSES_H_

/* Status return values */

extern fts_status_description_t fts_ClassAlreadyInitialized;
extern fts_status_description_t fts_InletOutOfRange;
extern fts_status_description_t fts_OutletOutOfRange;
extern fts_status_description_t fts_OutletAlreadyDefined;
extern fts_status_description_t fts_CannotInstantiate;

/* Meta classes functions */

extern fts_metaclass_t *fts_metaclass_create(fts_symbol_t name,
					     fts_method_instantiate_t mth_instantiate,
					     fts_method_equiv_t mth_equiv);

extern fts_metaclass_t *fts_metaclass_create_override(fts_symbol_t name,
						      fts_method_instantiate_t mth_instantiate,
						      fts_method_equiv_t mth_equiv);


extern void fts_metaclass_alias(fts_symbol_t new_name, fts_symbol_t old_name);

extern void fts_metaclass_declare_dynamic(fts_symbol_t name, fts_object_t *loader);


/* Class functions  and macros */

#define FTS_VAR_ARGS  -1

extern fts_status_t fts_class_init(fts_class_t *, unsigned int, int ninlets, int noutlets, void *);

#define fts_method_define(class, winlet, s, fun, argc, at)  \
          fts_method_define_optargs(class, winlet, s, fun, argc, at, argc) 

#define fts_method_define_varargs(class, winlet, s, fun)  \
          fts_method_define_optargs(class, winlet, s, fun, 0, 0, FTS_VAR_ARGS) 


extern fts_status_t fts_method_define_optargs(fts_class_t *, int winlet,
					      fts_symbol_t , fts_method_t, int, fts_symbol_t *, int);


#define fts_outlet_type_define(class, woutlet, s, ac, at)  \
          fts_outlet_type_define_optargs(class, woutlet, s, ac, at, ac) 

#define fts_outlet_type_define_varargs(class, woutlet, s)  \
          fts_outlet_type_define_optargs(class, woutlet, s, 0, 0, FTS_VAR_ARGS) 

extern fts_status_t fts_outlet_type_define_optargs(fts_class_t *cl, int woutlet, fts_symbol_t s,
						   int ac, fts_symbol_t *at,  int mandatory_args);

extern fts_symbol_t fts_get_class_name(fts_class_t *cl);

extern const int fts_SystemInlet;

extern int fts_arg_equiv( int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1);
extern int fts_arg_equiv_or_float( int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1);
extern int fts_first_arg_equiv( int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1);
extern int fts_narg_equiv( int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1);
extern int fts_never_equiv( int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1);
extern int fts_always_equiv( int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1);
extern int fts_arg_type_equiv( int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1);


#endif









