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


/* #include "binbuf.h" */

#define	MAXBUF	256

extern void *tabwind_class;
extern int max_ex_tab(struct expr *exp, fts_symbol_t s, struct ex_ex *arg, struct ex_ex *optr);
extern int ex_getsym(char *p, fts_symbol_t *s);
extern const char *ex_symname(fts_symbol_t s);

extern void argstostr(int argc, const fts_atom_t *argv, char *buf, int size);


