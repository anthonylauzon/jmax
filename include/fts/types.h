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

typedef int fts_typeid_t;
typedef const char *fts_symbol_t;
typedef void (*fts_fun_t)(void);
typedef struct fts_object fts_object_t;
typedef union  fts_word fts_word_t;
typedef struct fts_atom fts_atom_t;
typedef struct fts_connection fts_connection_t;
typedef struct fts_metaclass fts_metaclass_t;
typedef struct fts_class fts_class_t;
typedef struct fts_patcher fts_patcher_t;
typedef struct fts_inlet fts_inlet_t;
typedef struct fts_outlet fts_outlet_t;
typedef struct fts_template fts_template_t;
typedef struct fts_abstraction fts_abstraction_t;
typedef struct fts_package fts_package_t;

typedef struct fts_status_description fts_status_description_t, *fts_status_t;

/**
 * A fts_word_t is an union of the different basic types used in FTS.
 */

union fts_word
{
  int                 fts_int;
  float               fts_float;
  fts_symbol_t        fts_symbol;
  fts_object_t        *fts_object;
  void                *fts_pointer;
  char                *fts_string;
  fts_connection_t    *fts_connection;
};

/**
 * An fts_atom_t is a type-tagged word, i.e. includes a type identifier and a fts_word_t value.
 */

struct fts_atom { 
  fts_metaclass_t *typeid;
  fts_word_t value;
};


typedef void (*fts_method_t) (fts_object_t *, int, fts_symbol_t , int, const fts_atom_t *);

