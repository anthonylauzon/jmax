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
# define	TK_PAR	261
# define	TK_OPEN_PAR	262
# define	TK_CLOSED_PAR	263
# define	TK_CPAR	264
# define	TK_OPEN_CPAR	265
# define	TK_CLOSED_CPAR	266
# define	TK_SQPAR	267
# define	TK_OPEN_SQPAR	268
# define	TK_CLOSED_SQPAR	269
# define	TK_COMMA	270
# define	TK_TUPLE	271
# define	TK_LOGICAL_OR	272
# define	TK_LOGICAL_AND	273
# define	TK_EQUAL_EQUAL	274
# define	TK_NOT_EQUAL	275
# define	TK_GREATER	276
# define	TK_GREATER_EQUAL	277
# define	TK_SMALLER	278
# define	TK_SMALLER_EQUAL	279
# define	TK_SHIFT_LEFT	280
# define	TK_SHIFT_RIGHT	281
# define	TK_PLUS	282
# define	TK_MINUS	283
# define	TK_TIMES	284
# define	TK_DIV	285
# define	TK_PERCENT	286
# define	TK_UMINUS	287
# define	TK_UPLUS	288
# define	TK_LOGICAL_NOT	289
# define	TK_POWER	290
# define	TK_ARRAY_INDEX	291
# define	TK_DOT	292
# define	TK_DOLLAR	293
# define	TK_COLON	294


#endif /* not BISON_Y_TAB_H */
