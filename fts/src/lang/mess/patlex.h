#ifndef _PATLEX_H_
#define _PATLEX_H_

#include <stdio.h>

/* Token types */

#define FTS_LEX_NUMBER  0
#define FTS_LEX_FLOAT   1
#define FTS_LEX_SYMBOL  2
#define FTS_LEX_EOC     3
#define FTS_LEX_EOF  4


typedef struct fts_pat_lexer
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

} fts_pat_lexer_t;


#define pushBack(this) ((this)->pushedBack = 1)

extern fts_pat_lexer_t *fts_open_pat_lexer(const char *filename, int env_argc, const fts_atom_t *env_argv);
extern fts_pat_lexer_t *fts_open_pat_lexer_file(FILE *file, int env_argc, const fts_atom_t *env_argv);
extern void fts_close_pat_lexer(fts_pat_lexer_t *this);

extern void nextToken(fts_pat_lexer_t *this);


/* Convenience macros: test of a values against a symbol */

#define token_sym_equals(in, sym)  (((in)->ttype == FTS_LEX_SYMBOL) && (fts_get_symbol(&((in)->val)) == (sym)))

#endif





