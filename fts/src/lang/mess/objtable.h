/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */

#ifndef _OBJTABLE_H_
#define _OBJTABLE_H_
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






