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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 */

#ifndef _FTS_PRIVATE_OBJECT_H_
#define _FTS_PRIVATE_OBJECT_H_

extern void fts_object_set_id(fts_object_t *obj, int id);

/* support for redefinition */
extern fts_object_t *fts_object_recompute(fts_object_t *old);
extern fts_object_t *fts_object_redefine(fts_object_t *old, int ac, const fts_atom_t *at);

/* thru objects */
#define fts_object_is_thru(o) fts_class_is_thru((o)->head.cl)

#endif
