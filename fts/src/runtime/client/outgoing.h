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

#ifndef _OUTGOING_H_
#define _OUTGOING_H_

/* Sending a message */


extern void fts_client_start_msg(int type);
extern void fts_client_add_int(int value);
extern void fts_client_add_float(float value);
extern void fts_client_add_symbol(fts_symbol_t s);
extern void fts_client_add_object(fts_object_t *obj);
extern void fts_client_add_connection(fts_connection_t *c);
extern void fts_client_add_data( fts_data_t *data);
extern void fts_client_add_string(const char *sp);
extern void fts_client_add_atoms(int ac, const fts_atom_t *args);
extern void fts_client_done_msg(void);
extern void fts_client_message_send(fts_object_t *obj, fts_symbol_t selector, int argc, const fts_atom_t *args);

extern void fts_client_upload_object(fts_object_t *obj);
extern void fts_client_upload_connection(fts_connection_t *c);

extern void fts_client_release_connection(fts_connection_t *c);
extern void fts_client_redefine_connection(fts_connection_t *c);
extern void fts_client_release_object(fts_object_t *c);
extern void fts_client_release_object_data(fts_object_t *obj);
#endif
