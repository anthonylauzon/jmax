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

#ifndef _FTS_PRIVATE_PATCHER_H_
#define _FTS_PRIVATE_PATCHER_H_

extern fts_metaclass_t *patcher_metaclass;

struct fts_inlet
{
  fts_object_t o;
  int position; /* inlet position */
  struct fts_inlet *next; /* next inlet in the same position */
};

struct fts_outlet
{
  fts_object_t o;		
  int position; /* outlet position */
  struct fts_outlet *next; /* next outlet in the same position */
};

#define fts_object_is_patcher(o) ((o)->head.cl->mcl == patcher_metaclass)

#define fts_patcher_set_standard(p)      ((p)->type = fts_p_standard)
#define fts_patcher_set_abstraction(p)   ((p)->type = fts_p_abstraction)
#define fts_patcher_set_error(p)         ((p)->type = fts_p_error)
extern void fts_patcher_set_template(fts_patcher_t *patcher, fts_template_t *template);

#define fts_patcher_is_standard(p)       ((p)->type == fts_p_standard)
#define fts_patcher_is_abstraction(p)    ((p)->type == fts_p_abstraction)
#define fts_patcher_is_error(p)          ((p)->type == fts_p_error)
#define fts_patcher_is_template(p)       ((p)->type == fts_p_template)

#define fts_patcher_get_env(p)           (&((p)->env))

#define fts_patcher_being_deleted(p)      ((p)->deleted == 1)

#define fts_object_is_abstraction(o) (fts_object_is_patcher((o)) &&  \
				      fts_patcher_is_abstraction((fts_patcher_t *) (o)))

#define fts_object_is_standard_patcher(o) (fts_object_is_patcher((o)) &&  \
				      fts_patcher_is_standard((fts_patcher_t *) (o)))

#define fts_object_is_error(o) (fts_object_is_patcher((o)) &&  \
				      fts_patcher_is_error((fts_patcher_t *) (o)))

#define fts_object_is_template(o) (fts_object_is_patcher((o)) &&  \
				      fts_patcher_is_template((fts_patcher_t *) (o)))

#define fts_object_is_object(o) ((! fts_object_is_patcher((o))) || fts_object_is_error((o)))

extern void fts_patcher_redefine_number_of_inlets(fts_patcher_t *this, int new_ninlets);
extern void fts_patcher_redefine_number_of_outlets(fts_patcher_t *this, int new_noutlets);

extern void fts_patcher_add_object(fts_patcher_t *this, fts_object_t *obj);
extern void fts_patcher_remove_object(fts_patcher_t *this, fts_object_t *obj);
extern int  fts_patcher_get_objects_count( fts_patcher_t *this);
extern void fts_patcher_assign_variable(fts_symbol_t name, fts_atom_t *value, void *data);

extern void fts_patcher_reassign_inlets_outlets(fts_patcher_t *obj);

extern fts_patcher_t *fts_patcher_redefine(fts_patcher_t *this, int aoc, const fts_atom_t *aot);

extern void fts_patcher_blip(fts_patcher_t *this, const char *msg);

extern void fts_patcher_inlet_reposition(fts_object_t *o, int pos);
extern void fts_patcher_remove_inlet(fts_patcher_t *patcher, fts_inlet_t *this);
extern void fts_patcher_trim_number_of_inlets(fts_patcher_t *patcher);

extern void fts_patcher_outlet_reposition(fts_object_t *o, int pos);
extern void fts_patcher_remove_outlet(fts_patcher_t *patcher, fts_outlet_t *this);
extern void fts_patcher_trim_number_of_outlets(fts_patcher_t *patcher);

#endif
