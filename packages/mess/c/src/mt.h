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

