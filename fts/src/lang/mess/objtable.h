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


#ifndef _FTS_OBJTABLE_H_
#define _FTS_OBJTABLE_H_
/* New version of the objtable.

   This version imply that the object IDs are assigned
   consecutively, starting from 0; any other client behaviour
   will eat enourmous amount of memory, and possibly will
   just kill the machine.


   The data structure is direct access table, adaptively resized 
   when needed.

   Register generate an ID for the object, and add it to the table;
   put require an id; put should only be used for server request,
   no ID should be generated in this case.
   register generate even numbers, the server should generate odd
   numbers !!!

   It add also the connection table.
   */

extern void fts_object_table_register(fts_object_t *obj);
extern void fts_object_table_put(int id, fts_object_t *obj);
extern void fts_object_table_remove(int id);
extern fts_object_t *fts_object_table_get(int id);
extern void fts_object_table_delete_all(void);


extern void fts_connection_table_register(fts_connection_t *obj);
extern void fts_connection_table_put(int id, fts_connection_t *obj);
extern void fts_connection_table_remove(int id);
extern fts_connection_t *fts_connection_table_get(int id);
extern void fts_connection_table_delete_all(void);

#endif






