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

#ifndef _KEYSERVER_H_
#define _KEYSERVER_H_

#include <fts/fts.h>

/* the first 128 codes are ASCII, from 128 we invented these */
enum key_codes {
  code_f1 = 128, 
  code_f2, 
  code_f3, 
  code_f4, 
  code_f5, 
  code_f6, 
  code_f7, 
  code_f8,  
  code_f9, 
  code_f10, 
  code_f11, 
  code_f12,   
  code_enter, 
  code_esc, 
  code_shift, 
  code_alt, 
  code_ctrl, 
  n_codes
};

extern fts_symbol_t sym_f1;
extern fts_symbol_t sym_F1;
extern fts_symbol_t sym_f2;
extern fts_symbol_t sym_F2;
extern fts_symbol_t sym_f3;
extern fts_symbol_t sym_F3;
extern fts_symbol_t sym_f4;
extern fts_symbol_t sym_F4;
extern fts_symbol_t sym_f5;
extern fts_symbol_t sym_F5;
extern fts_symbol_t sym_f6;
extern fts_symbol_t sym_F6;
extern fts_symbol_t sym_f7;
extern fts_symbol_t sym_F7;
extern fts_symbol_t sym_f8;
extern fts_symbol_t sym_F8;
extern fts_symbol_t sym_f9;
extern fts_symbol_t sym_F9;
extern fts_symbol_t sym_f10;
extern fts_symbol_t sym_F10;
extern fts_symbol_t sym_f11;
extern fts_symbol_t sym_F11;
extern fts_symbol_t sym_f12;
extern fts_symbol_t sym_F12;
extern fts_symbol_t sym_enter;
extern fts_symbol_t sym_esc;
extern fts_symbol_t sym_escape;
extern fts_symbol_t sym_shift;
extern fts_symbol_t sym_alt;
extern fts_symbol_t sym_ctrl;
extern fts_symbol_t sym_space;

typedef struct _listener_
{
  fts_object_t *obj;
  fts_method_t meth;
  struct _listener_ *next;  
} listener_t;

extern listener_t *key_listeners[n_codes];

extern void keyserver_add_listener(int code, fts_object_t *obj, fts_method_t meth);
extern void keyserver_remove_listener(int code, fts_object_t *obj);

#endif
