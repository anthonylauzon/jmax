/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

/* Must be declared forward */
typedef const char *fts_symbol_t;
typedef struct fts_object fts_object_t;
typedef union fts_word fts_word_t;
typedef struct fts_atom fts_atom_t;
typedef struct fts_array fts_array_t;
typedef struct fts_class fts_class_t;
typedef struct fts_status_description *fts_status_t;

#define fts_method_status_t void
typedef fts_method_status_t (*fts_method_t) (fts_object_t *, fts_symbol_t , int, const fts_atom_t *, fts_atom_t *);
typedef fts_status_t (*fts_fun_t) (int, const fts_atom_t *, fts_atom_t *);

typedef struct fts_bytestream fts_bytestream_t;

/* Should it be declared forward ??? */
typedef struct fts_connection fts_connection_t;
typedef struct fts_patcher fts_patcher_t;
typedef struct fts_inlet fts_inlet_t;
typedef struct fts_outlet fts_outlet_t;
typedef struct fts_template fts_template_t;
typedef struct fts_abstraction fts_abstraction_t;
typedef struct fts_package fts_package_t;

/**
 * A fts_word_t is an union of the different basic types used in FTS.
 */

union fts_word
{
  int                 fts_int;
  double              fts_float;
  fts_symbol_t        fts_symbol;
  fts_object_t        *fts_object;
  void                *fts_pointer;
  char                *fts_string;
};

/**
 * An fts_atom_t is a type-tagged word, i.e. includes a type identifier and a fts_word_t value.
 */

struct fts_atom { 
  fts_class_t *type;
  fts_word_t value;
};



