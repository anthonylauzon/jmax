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

typedef struct {
  int length;
  fts_symbol_t *symbols;
#ifdef CACHE_REPORT
  int naccess, nhit;
#endif
} fts_symbol_cache_t;


typedef struct {
  fts_object_t head;

  /* Input protocol decoder */
  /* Automata state */
  int state;
  /* Input decoding */
  fts_stack_t input_args;
  fts_stack_t input_buffer;
  /* Symbol caches */
  fts_symbol_cache_t input_cache;

  /* Output protocol encoder */
  /* Output buffer */
  fts_stack_t output_buffer;
  /* Symbol caches */
  fts_symbol_cache_t output_cache;
} fts_binary_protocol_t;

FTS_API fts_class_t* fts_binary_protocol_type;
FTS_API int fts_binary_protocol_decode(fts_binary_protocol_t* binary_protocol, int size, const unsigned char* buffer);
