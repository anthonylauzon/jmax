/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#ifndef _NAMING_H_
#define _NAMING_H_


/* Only for compatibility with the ISPW Object set; don't use for new objects. */
/* (fd) Should this be moved to the ISPW package ? It is referred by:
 - message box (in package guiobj)
 - qlist (in package qlist)
*/


extern fts_object_t *fts_get_object_by_name(fts_symbol_t name);
extern void fts_register_named_object(fts_object_t *obj, fts_symbol_t name);
extern void fts_unregister_named_object(fts_object_t *obj, fts_symbol_t name);
extern int fts_named_object_exists(fts_symbol_t name);
extern void fts_named_object_send(fts_symbol_t name, fts_symbol_t s, int argc, const fts_atom_t *argv);

#endif
