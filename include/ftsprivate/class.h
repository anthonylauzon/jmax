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

#ifndef _FTS_PRIVATE_CLASS_H_
#define _FTS_PRIVATE_CLASS_H_

/* Predefined class ids */
#define FTS_TYPEID_VOID     1
#define FTS_TYPEID_INT      2
#define FTS_TYPEID_FLOAT    3
#define FTS_TYPEID_SYMBOL   4
#define FTS_TYPEID_POINTER  5
#define FTS_TYPEID_STRING   6

#define fts_class_get_id(CL) ((CL)->type_id)

/* the class of 'class' objects */
extern fts_class_t *fts_class_class;

struct fts_class_outlet
{
  fts_list_t *declarations;
};

#define fts_class_get_package(m) ((m)->package)
#define fts_class_set_package(m, p) ((m)->package = (p))

#define fts_class_set_name(m, s) ((m)->name = (s))

/* get inlet methods */
extern fts_method_t fts_class_get_inlet_method(fts_class_t *cl, int winlet, fts_class_t *type);

extern int fts_class_outlet_get_declarations(fts_class_t *cl, int woutlet, fts_iterator_t *iter);
extern int fts_class_outlet_has_type(fts_class_t *cl, int woutlet, fts_class_t *type);
extern int fts_class_outlet_has_message(fts_class_t *cl, int woutlet, fts_symbol_t selector);

#endif

