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
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */


#ifndef _OBJECTS_H_
#define _OBJECTS_H_

/* Object functions and macros */

/* init function */

extern void fts_objects_init(void);

/* Object creation, deleting and replacing */

#define FTS_NO_ID -1

extern int fts_object_description_defines_variable(int ac, const fts_atom_t *at);

extern fts_object_t *fts_eval_object_description(fts_patcher_t *patcher, int ac, const fts_atom_t *at);
extern void fts_object_set_id(fts_object_t *obj, int id);
extern void fts_object_delete(fts_object_t *);
extern void fts_object_send_properties(fts_object_t *obj);
extern void fts_object_send_ui_properties(fts_object_t *obj);

/* This is only for object doctors (macros) */
fts_status_t fts_object_new(fts_patcher_t *patcher, int ac, const fts_atom_t *at, fts_object_t **ret);

/* Change the object description; more "system" oriented */
extern void fts_object_set_description(fts_object_t *obj, int argc, const fts_atom_t *argv);
extern void fts_object_set_description_and_class(fts_object_t *obj, fts_symbol_t class_name, int argc, const fts_atom_t *argv);

extern void fts_object_reset_description(fts_object_t *obj);

/* Support for redefinition */
extern fts_object_t *fts_object_recompute(fts_object_t *old);
extern fts_object_t *fts_object_redefine(fts_object_t *old, int new_id, int ac, const fts_atom_t *at);

/* Object Access */
#define fts_object_get_outlet_type(O, WOUTLET) (((fts_object_t *)(O))->cl->outlets[(WOUTLET)].tmess.symb)
#define fts_object_get_outlets_number(O) (((fts_object_t *)(O))->cl->noutlets)
#define fts_object_get_inlets_number(O) (((fts_object_t *)(O))->cl->ninlets)
#define fts_object_get_patcher(O) (((fts_object_t *)(O))->patcher)

#define fts_object_get_variable(o) ((o)->varname)
#define fts_object_set_variable(o, name) ((o)->varname = (name))

extern int fts_object_handle_message(fts_object_t *o, int winlet, fts_symbol_t s);
extern fts_symbol_t fts_object_get_class_name(fts_object_t *obj);

#define fts_object_is_outlet(o) ((o)->cl->mcl == outlet_metaclass)
#define fts_object_is_inlet(o) ((o)->cl->mcl == inlet_metaclass)

#define fts_object_get_id(o) ((o)->id)

/* Return true if the object is being deleted, i.e. if 
 the patcher (or an ancestor of the patcher) is being deleted */
extern int fts_object_being_deleted(fts_object_t *obj);

/* Test recursively if an object is inside a patcher (or its subpatchers) */
extern int fts_object_is_in_patcher(fts_object_t *obj, fts_patcher_t *patcher);

/* Messages for the status line */
extern void fts_object_blip(fts_object_t *obj, const char *format , ...);

/* Debug print */
extern void fprintf_object(FILE *f, fts_object_t *obj);
extern void post_object(fts_object_t *obj);

#endif
