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
#ifndef _MT_H_
#define _MT_H_

typedef struct
{
  fts_symbol_t s;
  int ac;
  fts_atom_t *at;
} message_t;

/**********************************************************
 *
 *  dynamic messages
 *
 */

extern void message_init(message_t *mess, fts_symbol_t s, int ac, const fts_atom_t *at);
extern void message_clear(message_t *mess);
extern void message_set(message_t *mess, fts_symbol_t s, int ac, const fts_atom_t *at);
extern void message_move(message_t *from, message_t *to);


/**********************************************************
 *
 *  message table
 *
 */

typedef struct
{
  message_t *mess;
  int size;
  int alloc;
  int refcnt;
} message_table_t;

#define message_table_get_element(mt, i) ((mt)->mess[i])
#define message_table_get_size(mt) ((mt)->size)

extern fts_type_t message_table_type;

extern message_table_t *message_table_new(int size);
extern void message_table_clear(message_table_t *mt);
extern void message_table_delete(message_table_t *mt);
extern void message_table_set_size(message_table_t *mt, int size);

extern void message_table_refer(message_table_t *mt);
extern void message_table_release(message_table_t *mt);

extern int message_table_file_import_ascii(message_table_t *mt, fts_symbol_t file_name);
extern int message_table_file_export_ascii(message_table_t *mt, fts_symbol_t file_name);

#endif

