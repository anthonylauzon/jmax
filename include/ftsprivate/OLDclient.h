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

#ifndef _FTS_PRIVATE_OLDCLIENT_H_
#define _FTS_PRIVATE_OLDCLIENT_H_

FTS_API void fts_client_upload(fts_object_t *obj, fts_symbol_t classname, int ac, const fts_atom_t *at);

/***********************************************************************
 *
 * Protocol
 *
 */

/* A message  is conceptually made of:
 *
 * 1- the command this message represent.
 * 2- an array of fts_values, whose interpretation depend
 *    on the type of message
 *
 *    The size of the array is variable
 *
 * Used Letters are:
 *
 * Forbidden values : 0x00 (easy message as string in C)
 * Low values: 0x01 0x02 0x03
 * Symbols   : - { } > < \
 * Lowercase : a b c d e f g h i l m n o p q s u x y w z  
 * Uppercase : A B C D E F G H M N O P Q R S U X Y Z 
 * 
 */

/* The first character identify
   The meaning of the command is subsystem specific.
   Here are the predefined commands
*/

#define CLIENTMESS_CODE                    'M'
#define CLIENTPROP_CODE                    'P'
#define CONNECTION_RELEASE_CODE            'd'
#define DECLARE_ABSTRACTION_CODE           'E'
#define DECLARE_ABSTRACTION_PATH_CODE      'F'
#define DECLARE_TEMPLATE_CODE              'y'
#define DECLARE_TEMPLATE_PATH_CODE         'w'
#define DELETE_CONNECTION_CODE             'a'
#define DELETE_OBJECT_CODE                 'f'
#define DOWNLOAD_CONNECTION_CODE           'H'
#define DOWNLOAD_OBJECT_CODE               'G'
#define FTS_SHUTDOWN_CODE                  's'
#define GETALLPROP_CODE                    'B'
#define GETPROP_CODE                       'g'
#define LOAD_PATCHER_BMAX_CODE             'U'
#define LOAD_PATCHER_DPAT_CODE             'Z'
#define MESSAGE_CODE                       'm'
#define NEW_CONNECTION_CODE                'c'
#define NEW_OBJECT_CODE                    'n'
#define NEW_OBJECT_VAR_CODE                'N'
#define NEW_TEMPLATE_INSTANCE_CODE         'T'
#define OBJECT_RELEASE_CODE                'l'
#define OBJECT_RELEASE_DATA_CODE           'b'
#define POST_CODE                          'h' /* a post request for the server */
#define POST_LINE_CODE                     'L' /* a full-line post request for the server */
#define PUTPROP_CODE                       'p'
#define RECOMPUTE_ERRORS_CODE              'e'
#define REDEFINE_CONNECTION_CODE           'V'
#define REDEFINE_OBJECT_CODE               'R'
#define REDEFINE_PATCHER_CODE              'Q'
#define SAVE_PATCHER_BMAX_CODE             'S'
#define SYNC_CODE                          '>'
#define SYNC_DONE_CODE                     '<'
#define UCS_CODE                           'u'
#define UPDATE_GROUP_END_CODE              '}'
#define UPDATE_GROUP_START_CODE            '{'
#define VOID_COMMAND_CODE                  '-'


/* Value coding */

/*
   Objects are represented as a positive number corresponding to its ID.
*/
   
#define STRING_CODE         0x01
#define STRING_END_CODE     0x02
#define INT_CODE            0x03
#define FLOAT_CODE          0x04
#define SYMBOL_CACHED_CODE  0x05
#define SYMBOL_AND_DEF_CODE 0x06
#define SYMBOL_CODE         0x07
#define OBJECT_CODE         0x08
#define CONNECTION_CODE     0x09
#define EOM_CODE            0x0b


/***********************************************************************
 *
 * Client private API
 *
 */

#if 0
extern int fts_client_is_up( void);
extern void fts_client_start_msg(int type);
extern void fts_client_add_connection(fts_connection_t *c);
extern void fts_client_add_string(const char *sp);
extern void fts_client_upload_object(fts_object_t *obj);
#endif

extern void fts_client_upload_connection(fts_connection_t *c);

extern void fts_client_release_connection(fts_connection_t *c);
extern void fts_client_redefine_connection(fts_connection_t *c);
extern void fts_client_release_object(fts_object_t *c);
extern void fts_client_release_object_data(fts_object_t *obj);

extern void fts_object_reset_changed(fts_object_t *obj);

extern void fts_updates_set_updates_per_ticks(int upt);
extern void fts_updates_set_update_period(int upt);
extern void fts_client_updates_sync(void);

#endif
