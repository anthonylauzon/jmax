/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */


#ifndef _FTS_CLASS_H_
#define _FTS_CLASS_H_


typedef fts_status_t (*fts_instantiate_fun_t)(fts_class_t *, int, const fts_atom_t *);
typedef int (*fts_equiv_fun_t)(int, const fts_atom_t *, int, const fts_atom_t *);

typedef struct fts_inlet_decl fts_inlet_decl_t;
typedef struct fts_outlet_decl fts_outlet_decl_t;

struct fts_metaclass
{
  fts_instantiate_fun_t instantiate_fun;

  /* Instance data base */
  fts_equiv_fun_t equiv_fun;
  fts_class_t *inst_list;

  fts_symbol_t name;		/* the name of the metaclass, i.e. the first name used to register it */
};

struct fts_class
{
  /* Object management */
  fts_metaclass_t *mcl;
  fts_inlet_decl_t *sysinlet;

  int ninlets;
  fts_inlet_decl_t *inlets;

  int noutlets;
  fts_outlet_decl_t *outlets;

  int size;

  /* Class Instance Data Base support */
  int ac;
  const fts_atom_t *at;
  fts_class_t *next;

  /* property list handling */
  fts_plist_t *properties;		/* class' dynamic properties */

  struct daemon_list *daemons;

  /* User data */
  void *user_data;
};


/* Status return values */

FTS_API fts_status_description_t fts_ClassAlreadyInitialized;
FTS_API fts_status_description_t fts_InletOutOfRange;
FTS_API fts_status_description_t fts_OutletOutOfRange;
FTS_API fts_status_description_t fts_OutletAlreadyDefined;
FTS_API fts_status_description_t fts_CannotInstantiate;

/* Meta classes functions */

FTS_API fts_status_t fts_metaclass_install( fts_symbol_t name,
					   fts_instantiate_fun_t instantiate_fun,
					   fts_equiv_fun_t equiv_fun);

FTS_API fts_status_t fts_class_install( fts_symbol_t name, fts_instantiate_fun_t instantiate_fun);

FTS_API void fts_metaclass_alias(fts_symbol_t new_name, fts_symbol_t old_name);

FTS_API void fts_class_alias( fts_symbol_t new_name, fts_symbol_t old_name);


/* Class functions  and macros */

#define FTS_VAR_ARGS  -1

FTS_API fts_status_t fts_class_init(fts_class_t *, unsigned int, int ninlets, int noutlets, void *);
FTS_API fts_class_t *fts_class_instantiate(int ac, const fts_atom_t *at);
FTS_API fts_class_t *fts_class_get_by_name(fts_symbol_t name);

/* method definition */

#define fts_method_define(class, winlet, s, fun, argc, at)  \
  fts_method_define_optargs(class, winlet, s, fun, argc, at, argc) 

#define fts_method_define_varargs(class, winlet, s, fun)  \
  fts_method_define_optargs(class, winlet, s, fun, 0, 0, FTS_VAR_ARGS) 


FTS_API fts_status_t fts_method_define_optargs(fts_class_t *cl, int winlet, fts_symbol_t s,
					      fts_method_t fun, int, fts_symbol_t *at, int mandatory_args);

#define fts_method_define_int(class, winlet, fun) \
  fts_method_define_optargs(class, winlet, fts_s_int, fun, 1, &fts_s_int, 1);

#define fts_method_define_float(class, winlet, fun) \
  fts_method_define_optargs(class, winlet, fts_s_float, fun, 1, &fts_s_float, 1);

#define fts_method_define_number(class, winlet, fun) \
  fts_method_define_optargs(class, winlet, fts_s_int, fun, 1, &fts_s_int, 1); \
  fts_method_define_optargs(class, winlet, fts_s_float, fun, 1, &fts_s_float, 1);

#define fts_method_define_symbol(class, winlet, fun) \
  fts_method_define_optargs(class, winlet, fts_s_symbol, fun, 1, &fts_s_symbol, 1);

#define fts_method_define_bang(class, winlet, fun) \
  fts_method_define_optargs(class, winlet, fts_s_bang, fun, 0, 0, 0);

#define fts_method_define_list(class, winlet, fun) \
  fts_method_define_optargs(class, winlet, fts_s_list, fun, 0, 0, FTS_VAR_ARGS);

#define fts_method_define_anything(class, winlet, fun) \
  fts_method_define_optargs(class, winlet, fts_s_anything, fun, 0, 0, FTS_VAR_ARGS);

/* outlet type definition */
#define fts_outlet_type_define(class, woutlet, s, ac, at)  \
          fts_outlet_type_define_optargs(class, woutlet, s, ac, at, ac) 

#define fts_outlet_type_define_varargs(class, woutlet, s)  \
          fts_outlet_type_define_optargs(class, woutlet, s, 0, 0, FTS_VAR_ARGS) 

FTS_API fts_status_t fts_outlet_type_define_optargs(fts_class_t *cl, int woutlet, fts_symbol_t s,
						   int ac, fts_symbol_t *at,  int mandatory_args);

FTS_API fts_symbol_t fts_get_class_name(fts_class_t *cl);

FTS_API fts_method_t fts_class_get_method( fts_class_t *cl, int inlet, fts_symbol_t s);

#define fts_class_has_method(C,I,S) (fts_class_get_method((C),(I),(S))!=0)

#define fts_class_get_user_data(c) ((cl)->user_data)
#define fts_class_set_user_data(c, d) ((cl)->user_data = (d))

FTS_API const int fts_SystemInlet;

/*****************************************************************************
 *
 *  "thru" classes
 *
 */

typedef void (*fts_propagate_fun_t)(void *ptr, fts_object_t *object, int outlet);

FTS_API void fts_class_define_thru(fts_class_t *class, fts_method_t propagate_input);
FTS_API int fts_class_is_thru(fts_class_t *class);

/*****************************************************************************
 *
 *  equivalence function library
 *
 */
FTS_API int fts_arg_equiv( int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1);
FTS_API int fts_arg_equiv_or_float( int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1);
FTS_API int fts_first_arg_equiv( int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1);
FTS_API int fts_narg_equiv( int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1);
FTS_API int fts_never_equiv( int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1);
FTS_API int fts_always_equiv( int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1);
FTS_API int fts_arg_type_equiv( int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1);

#endif
