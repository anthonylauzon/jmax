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

/**
 * Client communication
 *
 * @defgroup client client
 */

/**
 * Send a message to the client mirror of an object
 *
 * @fn void fts_client_send_message(fts_object_t *obj, fts_symbol_t selector, int ac, const fts_atom_t *at)
 * @param obj the object
 * @param selector the selector
 * @param ac arguments count
 * @param at arguments
 *
 * @ingroup client
 */
FTS_API void fts_client_send_message(fts_object_t *obj, fts_symbol_t selector, int ac, const fts_atom_t *at);


/**
 * Start a message to the client mirror of an object
 *
 * @fn void fts_client_start_message( fts_object_t *obj, fts_symbol_t selector)
 * @param obj the object
 * @param selector the selector
 *
 * @ingroup client
 */
FTS_API void fts_client_start_message( fts_object_t *obj, fts_symbol_t selector);

/**
 * Add an integer argument to a message to the client mirror of an object
 *
 * @fn void fts_client_add_int( fts_object_t *obj, int v);
 * @param obj the object
 * @param v the integer value
 *
 * @ingroup client
 */
FTS_API void fts_client_add_int( fts_object_t *obj, int v);

/**
 * Add a float argument to a message to the client mirror of an object
 *
 * @fn void fts_client_add_float( fts_object_t *obj, float v);
 * @param obj the object
 * @param v the float value
 *
 * @ingroup client
 */
FTS_API void fts_client_add_float( fts_object_t *obj, float v);

/**
 * Add a symbol argument to a message to the client mirror of an object
 *
 * @fn void fts_client_add_symbol( fts_object_t *obj, fts_symbol_t v);
 * @param obj the object
 * @param v the symbol value
 *
 * @ingroup client
 */
FTS_API void fts_client_add_symbol( fts_object_t *obj, fts_symbol_t v);

/**
 * Add a string argument to a message to the client mirror of an object
 *
 * @fn void fts_client_add_string( fts_object_t *obj, const char *v);
 * @param obj the object
 * @param v the string value
 *
 * @ingroup client
 */
FTS_API void fts_client_add_string( fts_object_t *obj, const char *v);

/**
 * Add an object argument to a message to the client mirror of an object
 *
 * @fn void fts_client_add_object( fts_object_t *obj, fts_object_t *v);
 * @param obj the object
 * @param v the object value
 *
 * @ingroup client
 */
FTS_API void fts_client_add_object( fts_object_t *obj, fts_object_t *v);

/**
 * Add an array of atoms to a message to the client mirror of an object
 *
 * @fn void fts_client_add_atoms( fts_object_t *obj, int ac, const fts_atom_t *at);
 * @param obj the object
 * @param ac the arguments count
 * @param at the arguments
 *
 * @ingroup client
 */
FTS_API void fts_client_add_atoms( fts_object_t *obj, int ac, const fts_atom_t *at);

/**
 * Flushes the message to the client mirror of an object
 *
 * @fn void fts_client_done_message( fts_object_t *obj);
 * @param obj the object
 *
 * @ingroup client
 */
FTS_API void fts_client_done_message( fts_object_t *obj);

/**
 * Upload the object: assign an id to the object and send upload_child message to the parent
 *
 * @fn void fts_client_upload_object( fts_object_t *obj, int client_id);
 * @param obj the object to upload
 * @param client_id the client id 
 *
 * @ingroup client
 */
FTS_API void fts_client_upload_object( fts_object_t *obj, int client_id);
FTS_API void fts_client_register_object( fts_object_t *obj, int client_id);

/* compatibility */
/* implemented for updates */
FTS_API void fts_object_ui_property_changed(fts_object_t *obj, fts_symbol_t property);

/* these functions are empty */
FTS_API void fts_object_property_changed(fts_object_t *obj, fts_symbol_t property);
FTS_API void fts_client_send_property(fts_object_t *obj, fts_symbol_t property);

