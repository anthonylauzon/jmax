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

/* the class of 'class' objects */
extern fts_class_t *fts_class_class;

struct fts_class_outlet
{
  fts_list_t *declarations;
};

#define fts_class_get_package(m) ((m)->package)
#define fts_class_set_package(m, p) ((m)->package = (p))

#define fts_class_set_name(m, s) ((m)->name = (s))

extern void fts_class_instantiate(fts_class_t *cl);

extern fts_method_t fts_class_inlet_get_method(fts_class_t *cl, int winlet, fts_class_t *type);

extern int fts_class_outlet_get_declarations(fts_class_t *cl, int woutlet, fts_iterator_t *iter);
extern int fts_class_outlet_has_type(fts_class_t *cl, int woutlet, fts_class_t *type);
extern int fts_class_outlet_has_message(fts_class_t *cl, int woutlet, fts_symbol_t selector);

#endif

