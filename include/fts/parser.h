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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 */

typedef void (*fts_parser_callback_t)( int token, fts_atom_t *value, void *data);

struct _fts_parser_callback_entry_t {
  fts_parser_callback_t prefix;
  fts_parser_callback_t infix;
  fts_parser_callback_t postfix;
};

typedef struct {
  struct _fts_parser_callback_entry_t semi;
  struct _fts_parser_callback_entry_t tuple;
  struct _fts_parser_callback_entry_t c_int;
  struct _fts_parser_callback_entry_t c_float;
  struct _fts_parser_callback_entry_t symbol;
  struct _fts_parser_callback_entry_t par;
  struct _fts_parser_callback_entry_t cpar;
  struct _fts_parser_callback_entry_t sqpar;
  struct _fts_parser_callback_entry_t dollar;
  struct _fts_parser_callback_entry_t uplus;
  struct _fts_parser_callback_entry_t uminus;
  struct _fts_parser_callback_entry_t logical_not;
  struct _fts_parser_callback_entry_t plus;
  struct _fts_parser_callback_entry_t minus;
  struct _fts_parser_callback_entry_t times;
  struct _fts_parser_callback_entry_t div;
  struct _fts_parser_callback_entry_t power;
  struct _fts_parser_callback_entry_t percent;
  struct _fts_parser_callback_entry_t shift_left;
  struct _fts_parser_callback_entry_t shift_right;
  struct _fts_parser_callback_entry_t logical_and;
  struct _fts_parser_callback_entry_t logical_or;
  struct _fts_parser_callback_entry_t equal_equal;
  struct _fts_parser_callback_entry_t not_equal;
  struct _fts_parser_callback_entry_t greater;
  struct _fts_parser_callback_entry_t greater_equal;
  struct _fts_parser_callback_entry_t smaller;
  struct _fts_parser_callback_entry_t smaller_equal;
} fts_parser_callback_table_t;

typedef struct _fts_parser_t {
  int ac;
  const fts_atom_t *at;
  struct _pnode_t *tree;
} fts_parser_t;

FTS_API void fts_parser_init( fts_parser_t *parser, int ac, const fts_atom_t *at);

void fts_parser_apply( fts_parser_t *parser, fts_parser_callback_table_t *callbacks, void *data);

void fts_parser_print( fts_parser_t *parser);

void fts_parser_eval( fts_parser_t *parser, fts_object_t *obj, int ac, const fts_atom_t *at);

FTS_API void fts_parser_destroy( fts_parser_t *parser);



