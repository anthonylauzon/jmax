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
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#ifndef _FTS_MESSP_H_
#define _FTS_MESSP_H_

/* extern declarations for private functions of the message system
   Do not included by fts objects or user modules, not exported or distributed
   with the .h files.
 */


/* Init functions of the various submodules */

extern void fts_symbols_init(void);
extern void fts_classes_init(void);
extern void fts_patcher_init(void);
extern void fts_patcher_shutdown(void);
extern void fts_events_init(void);
extern void fts_patparser_init(void);
extern void fts_mess_naming_init(void);
extern void fts_params_init(void);
extern void fts_properties_init(void);
extern void fts_patcher_data_config(void);
extern void fts_variables_init(void);

/* System classes config */

extern void fts_void_object_config(void);
extern void fts_selection_config(void);
extern void fts_clipboard_config(void);

/*
 * private class functions exported for the objects.c file
 */

extern fts_metaclass_t *fts_metaclass_get_by_name(fts_symbol_t name);

extern fts_class_t      *fts_class_instantiate(int ac, const fts_atom_t *at);
extern fts_class_mess_t *fts_class_mess_inlet_get(fts_inlet_decl_t *in, fts_symbol_t s,  int *panything);
extern fts_class_mess_t *fts_class_mess_get(fts_class_t *cl, int winlet, fts_symbol_t s, int *panything);

/*
 *  Functions to handle patcher/object relationship
 */

extern void fts_create_root_patcher(void);
extern void fts_patcher_add_object(fts_patcher_t *this, fts_object_t *obj);
extern void fts_patcher_remove_object(fts_patcher_t *this, fts_object_t *obj);
extern int  fts_patcher_get_objects_count( fts_patcher_t *this);
extern void fts_patcher_assign_variable(fts_symbol_t name, fts_atom_t *value, void *data);

/* Functions for direct .pat loading support */

extern void fts_patparse_parse_patlex(fts_object_t *parent, fts_patlex_t *in);

extern void fts_patcher_reassign_inlets_outlets(fts_patcher_t *obj);


/* 
 * Property related private functions
 */

extern void fts_properties_free(fts_object_t *obj);

/* Doctor */

extern void fts_doctor_init(void);
extern fts_object_t *fts_call_object_doctor(fts_patcher_t *patcher, int ac, const fts_atom_t *at);
extern int fts_object_doctor_exists(fts_symbol_t class_name);


#endif
