#ifndef BISON_Y_TAB_H
# define BISON_Y_TAB_H

#ifndef YYSTYPE
typedef union {
  fts_atom_t a;
  fts_parsetree_t *n;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	TK_INT	257
# define	TK_FLOAT	258
# define	TK_SYMBOL	259
# define	TK_SEMI	260
# define	TK_COLON	261
# define	TK_PAR	262
# define	TK_OPEN_PAR	263
# define	TK_CLOSED_PAR	264
# define	TK_CPAR	265
# define	TK_OPEN_CPAR	266
# define	TK_CLOSED_CPAR	267
# define	TK_SQPAR	268
# define	TK_OPEN_SQPAR	269
# define	TK_CLOSED_SQPAR	270
# define	TK_COMMA	271
# define	TK_TUPLE	272
# define	TK_LOGICAL_OR	273
# define	TK_LOGICAL_AND	274
# define	TK_EQUAL_EQUAL	275
# define	TK_NOT_EQUAL	276
# define	TK_GREATER	277
# define	TK_GREATER_EQUAL	278
# define	TK_SMALLER	279
# define	TK_SMALLER_EQUAL	280
# define	TK_SHIFT_LEFT	281
# define	TK_SHIFT_RIGHT	282
# define	TK_PLUS	283
# define	TK_MINUS	284
# define	TK_TIMES	285
# define	TK_DIV	286
# define	TK_PERCENT	287
# define	TK_UMINUS	288
# define	TK_UPLUS	289
# define	TK_LOGICAL_NOT	290
# define	TK_POWER	291
# define	TK_ARRAY_INDEX	292
# define	TK_DOT	293
# define	TK_DOLLAR	294


#endif /* not BISON_Y_TAB_H */
