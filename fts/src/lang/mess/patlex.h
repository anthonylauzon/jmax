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

#ifndef _PATLEX_H_
#define _PATLEX_H_


/* Token types */

#define FTS_LEX_NUMBER  0
#define FTS_LEX_FLOAT   1
#define FTS_LEX_SYMBOL  2
#define FTS_LEX_EOC     3
#define FTS_LEX_EOF  4


typedef struct fts_patlex
{
  int ttype;
  fts_atom_t val;

  const fts_atom_t *env_argv;
  int env_argc;

  FILE *fd;

  int pushedBack;
  int lookahead_valid;
  int lookahead;	

  int unique_var;

  char buf[512];
  int buf_fill;

  int messbox_mode;
} fts_patlex_t;


#define fts_patlex_push_back(this) ((this)->pushedBack = 1)

extern fts_patlex_t *fts_patlex_open(const char *filename, int env_argc, const fts_atom_t *env_argv);
extern fts_patlex_t *fts_patlex_open_file(FILE *file, int env_argc, const fts_atom_t *env_argv);
extern void fts_patlex_close(fts_patlex_t *this);
extern void fts_patlex_next_token(fts_patlex_t *this);

#define  fts_patparse_set_messbox_mode(in) ((in)->messbox_mode = 1)
#define  fts_patparse_set_normal_mode(in) ((in)->messbox_mode = 0)

/* Convenience macros: test of a values against a symbol */

#define token_sym_equals(in, sym)  (((in)->ttype == FTS_LEX_SYMBOL) && (fts_get_symbol(&((in)->val)) == (sym)))

#endif





