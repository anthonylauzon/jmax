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

#ifndef _FTS_PARAMS_H_
#define _FTS_PARAMS_H_

typedef void (* fts_param_listener_fun_t) (void *listener, fts_symbol_t name,
					   const fts_atom_t *value);


extern void fts_param_set(fts_symbol_t name, const fts_atom_t *value);
extern void fts_param_set_by(fts_symbol_t name, const fts_atom_t *value, const void *author);

extern const fts_atom_t *fts_param_get(fts_symbol_t name);
extern void fts_param_add_listener(fts_symbol_t name, void *listener, fts_param_listener_fun_t listener_fun);
extern void fts_param_remove_listener(void *listener);

/* Convenience function for getting/setting scalar parameter values */

extern float fts_param_get_float(fts_symbol_t name, float default_value);
extern int fts_param_get_int(fts_symbol_t name, int default_value);
extern fts_symbol_t fts_param_get_symbol(fts_symbol_t name, fts_symbol_t default_value);

extern void fts_param_set_float(fts_symbol_t name,  float value);
extern void fts_param_set_float_by(fts_symbol_t name,  float value, void *author);
extern void fts_param_set_int(fts_symbol_t name,    int value);
extern void fts_param_set_int_by(fts_symbol_t name,    int value, void *author);
extern void fts_param_set_symbol(fts_symbol_t name, fts_symbol_t value);
extern void fts_param_set_symbol_by(fts_symbol_t name, fts_symbol_t value, void *author);

#endif
