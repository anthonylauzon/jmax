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

#ifndef _FTS_TYPES_H_
#define _FTS_TYPES_H_

typedef const char *fts_symbol_t;

#define fts_type_t fts_symbol_t
#define fts_type_get_selector(T) (T)
#define fts_type_get_by_name(n) (n)

typedef void (*fts_fun_t)(void);
typedef struct fts_object fts_object_t;
typedef struct fts_connection fts_connection_t;

typedef struct fts_metaclass fts_metaclass_t;
typedef struct fts_class fts_class_t;
typedef struct fts_patcher fts_patcher_t;

typedef struct fts_inlet fts_inlet_t;
typedef struct fts_outlet fts_outlet_t;
typedef struct fts_template fts_template_t;
typedef struct fts_abstraction fts_abstraction_t;
typedef struct _fts_package_t fts_package_t;


typedef union  fts_word fts_word_t;
typedef struct fts_atom fts_atom_t;

typedef struct _fts_status_description_t fts_status_description_t;
typedef fts_status_description_t *fts_status_t;

/*
 * WORDS
 *
 * An fts_word_t is an union of the different basic types used in FTS.
 */

union fts_word
{
  int                 fts_int;
  float               fts_float;
  fts_symbol_t        fts_symbol;
  char                *fts_str;
  fts_object_t        *fts_obj;
  fts_connection_t    *fts_connection;
  void                *fts_ptr;		/*  just a pointer somewhere */
  void                (*fts_fun)(void);     /*  just a pointer to some function */
};

/*
 * ATOMS
 *
 * Atoms are type-tagged words; i.e. an atom include a type tag,
 * and a fts_word_t value.
 * They are used everywhere in FTS as self described datums, like
 * in object messages and so on.
 *
 * The types tag is a symbols; to each basic type correspond 
 * to a symbol predefined in symbol.h; basic types are supported by
 * means of a group of dedicated get/set macros, but the atoms are not limited
 * to basic types; the symbol can be set freely; the C type of the value part
 * should be considered as the representational type; it make sense for example
 * to have a type like "socket" that use for its representation the int C type.
 *
 * The special  symbol fts_s_void means a void value.
 * 
 * The special symbol fts_s_error means an error value, i.e. a value
 * that should generate an error each time somebody try to use it.
 *
 */

struct fts_atom
{	 
  fts_symbol_t type;		/* from the above defs, or others */
  fts_word_t value;
};



typedef void (*fts_method_t) (fts_object_t *, int, fts_symbol_t , int, const fts_atom_t *);

#endif
