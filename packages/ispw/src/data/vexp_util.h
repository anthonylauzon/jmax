
/*
 *                      Copyright (c) 1993 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *      $Revision: 1.1 $ IRCAM $Date: 1998/09/19 14:36:26 $
 *
 */


/* #include "binbuf.h" */

#define	MAXBUF	256

extern void *tabwind_class;
extern int max_ex_tab(struct expr *exp, fts_symbol_t s, struct ex_ex *arg, struct ex_ex *optr);
extern int ex_getsym(char *p, fts_symbol_t *s);
extern const char *ex_symname(fts_symbol_t s);

extern void argstostr(int argc, const fts_atom_t *argv, char *buf, int size);


