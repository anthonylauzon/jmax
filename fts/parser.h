typedef union {
  fts_atom_t a;
  fts_parsetree_t *n;
} YYSTYPE;
#define	TK_INT	257
#define	TK_FLOAT	258
#define	TK_SYMBOL	259
#define	TK_SEMI	260
#define	TK_PAR	261
#define	TK_OPEN_PAR	262
#define	TK_CLOSED_PAR	263
#define	TK_CPAR	264
#define	TK_OPEN_CPAR	265
#define	TK_CLOSED_CPAR	266
#define	TK_OPEN_SQPAR	267
#define	TK_CLOSED_SQPAR	268
#define	TK_COMMA	269
#define	TK_TUPLE	270
#define	TK_LOGICAL_OR	271
#define	TK_LOGICAL_AND	272
#define	TK_EQUAL_EQUAL	273
#define	TK_NOT_EQUAL	274
#define	TK_GREATER	275
#define	TK_GREATER_EQUAL	276
#define	TK_SMALLER	277
#define	TK_SMALLER_EQUAL	278
#define	TK_SHIFT_LEFT	279
#define	TK_SHIFT_RIGHT	280
#define	TK_PLUS	281
#define	TK_MINUS	282
#define	TK_TIMES	283
#define	TK_DIV	284
#define	TK_PERCENT	285
#define	TK_UMINUS	286
#define	TK_UPLUS	287
#define	TK_LOGICAL_NOT	288
#define	TK_POWER	289
#define	TK_SQPAR	290
#define	TK_DOT	291
#define	TK_DOLLAR	292
#define	TK_COLON	293


extern YYSTYPE yylval;
