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

#ifndef _FTS_PRIVATE_TOKENIZER_H_
#define _FTS_PRIVATE_TOKENIZER_H_

/*
 * The FLEX tokenizer
 * A wrapper on FLEX functions for tokenizing strings into atoms (int, float, symbols and tokens)
 * The operators are returned as token atoms.
 * This tokenizer is ***NOT*** used by the parser. The parser parses an array of atoms
 * that may have been filled by this tokenizer, but not necessarily.
 */

typedef struct {
  void *p;
} fts_tokenizer_t;

/* Initializes the tokenizer structures */
extern void fts_tokenizer_init( fts_tokenizer_t *tokenizer, const char *s);

/* Run the tokenizer and returns the next token.
   Token value is stored in atom pointed by a.
   Returns 0 when end of input is reached.
*/
extern int fts_tokenizer_run( fts_tokenizer_t *tokenizer, fts_atom_t *a);

/* Deinitializes the tokenizer structure and reclaim allocated memory */
extern void fts_tokenizer_destroy( fts_tokenizer_t *tokenizer);


#endif
