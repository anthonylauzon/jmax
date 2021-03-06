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

#ifndef _FTS_PRIVATE_ERROBJ_H_
#define _FTS_PRIVATE_ERROBJ_H_

typedef struct
{
  fts_object_t o;
  fts_symbol_t name;
  fts_symbol_t description;
} fts_error_object_t;

extern fts_class_t *fts_error_object_class;

#define fts_error_object_get_name(e) ((e)->name)
#define fts_error_object_get_description(e) ((e)->description)

extern fts_class_t *fts_error_object_get_class(fts_error_object_t *obj);

extern void fts_error_object_fit_inlet(fts_object_t *obj, int ninlet);
extern void fts_error_object_fit_outlet(fts_object_t *obj, int noutlet);

#endif
