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

#ifndef _FTS_PRIVATE_CLASS_H_
#define _FTS_PRIVATE_CLASS_H_

typedef struct fts_mess_type fts_mess_type_t;
typedef struct fts_class_mess fts_class_mess_t;

struct fts_mess_type
{
  fts_symbol_t symb;

  int mandatory_args;
  int nargs;
  fts_symbol_t *arg_types;
};


struct fts_class_mess
{
  fts_mess_type_t tmess;
  fts_method_t mth;
};

struct fts_inlet_decl
{
  int nmess;
  int nalloc;
  fts_class_mess_t **messlist;
};

struct fts_outlet_decl
{
  fts_mess_type_t tmess;	
};

#define fts_metaclass_get_package(m) ((m)->package)
#define fts_metaclass_set_package(m, p) ((m)->package = (p))

extern fts_metaclass_t *fts_metaclass_get_by_name(fts_symbol_t name);
extern fts_class_mess_t *fts_class_mess_inlet_get(fts_inlet_decl_t *in, fts_symbol_t s,  int *panything);
extern fts_class_mess_t *fts_class_mess_get(fts_class_t *cl, int winlet, fts_symbol_t s, int *panything);

#endif
