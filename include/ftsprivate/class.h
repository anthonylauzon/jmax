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

struct fts_inlet_decl
{
  fts_hashtable_t messhash;
  fts_method_t anything;
};

struct fts_outlet_decl
{
  fts_symbol_t selector;	
};

#define fts_metaclass_get_package(m) ((m)->package)
#define fts_metaclass_set_package(m, p) ((m)->package = (p))

#define fts_metaclass_set_selector(m, s) ((m)->selector = (s))
#define fts_metaclass_get_typeid(m) ((m)->typeid)

extern fts_method_t fts_class_inlet_get_method(fts_class_t *cl, int inlet, fts_symbol_t s);
extern fts_method_t fts_class_inlet_get_anything(fts_class_t *cl, int inlet);
extern int fts_class_inlet_has_anything_only(fts_class_t *cl, int inlet);

extern fts_symbol_t fts_class_outlet_get_selector(fts_class_t *cl, int outlet);


#endif

