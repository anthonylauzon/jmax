
/*  A Bison parser, made from parser.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	FTS_TOKEN_INT	257
#define	FTS_TOKEN_DOUBLE	258
#define	FTS_TOKEN_SYMBOL	259
#define	FTS_TOKEN_SEMI	260
#define	FTS_TOKEN_COLON	261
#define	FTS_TOKEN_OPEN_PAR	262
#define	FTS_TOKEN_CLOSED_PAR	263
#define	FTS_TOKEN_OPEN_CPAR	264
#define	FTS_TOKEN_CLOSED_CPAR	265
#define	FTS_TOKEN_OPEN_SQPAR	266
#define	FTS_TOKEN_CLOSED_SQPAR	267
#define	FTS_TOKEN_SPACE	268
#define	FTS_TOKEN_EQUAL	269
#define	FTS_TOKEN_LOGICAL_OR	270
#define	FTS_TOKEN_LOGICAL_AND	271
#define	FTS_TOKEN_EQUAL_EQUAL	272
#define	FTS_TOKEN_NOT_EQUAL	273
#define	FTS_TOKEN_GREATER	274
#define	FTS_TOKEN_GREATER_EQUAL	275
#define	FTS_TOKEN_SMALLER	276
#define	FTS_TOKEN_SMALLER_EQUAL	277
#define	FTS_TOKEN_SHIFT_LEFT	278
#define	FTS_TOKEN_SHIFT_RIGHT	279
#define	FTS_TOKEN_PLUS	280
#define	FTS_TOKEN_MINUS	281
#define	FTS_TOKEN_TIMES	282
#define	FTS_TOKEN_DIV	283
#define	FTS_TOKEN_PERCENT	284
#define	FTS_TOKEN_UNARY_MINUS	285
#define	FTS_TOKEN_UNARY_PLUS	286
#define	FTS_TOKEN_LOGICAL_NOT	287
#define	FTS_TOKEN_ARRAY_INDEX	288
#define	FTS_TOKEN_DOT	289
#define	FTS_TOKEN_DOLLAR	290

#line 23 "parser.y"

#include <stdio.h>
#include <unistd.h>
#include <fts/fts.h>

#define free fts_free

#define YYSTYPE fts_atom_t
#define YYPARSE_PARAM data
#define YYLEX_PARAM data

static int yylex( YYSTYPE *lvalp, void *data);
static int yyerror( const char *msg);

typedef struct {
  int ac;
  fts_atom_t *at;
  int *tokens;
  int *current_token;
} fts_parser_t;

#define ARITH_UNNOP( ret, a1, OP)			\
  if (fts_is_int( a1))					\
    fts_set_int( ret, OP fts_get_int( a1));		\
  else if (fts_is_double( a1))				\
    fts_set_double( ret, OP fts_get_double( a1));	\
  else							\
    fts_set_void( ret);

#define ARITH_IUNNOP( ret, a1, OP)		\
  if (fts_is_int( a1))				\
    fts_set_int( ret, OP fts_get_int( a1));	\
  else						\
    fts_set_void( ret);

#define ARITH_BINOP( ret, a1, a2, OP)							\
  if (fts_is_int( a1) && fts_is_int( a2))						\
    fts_set_int( ret, fts_get_int( a1) OP fts_get_int( a2));				\
  else if (fts_is_double( a1) && fts_is_double( a2))					\
    fts_set_double( ret, fts_get_number_double( a1) OP fts_get_number_double( a2));	\
  else											\
    fts_set_void( ret);

#define ARITH_IBINOP( ret, a1, a2, OP)							\
  if (fts_is_int( a1) && fts_is_int( a2))						\
    fts_set_int( ret, fts_get_int( a1) OP fts_get_int( a2));				\
  else											\
    fts_set_void( ret);

#define LOGIC_BINOP( ret, a1, a2, OP)					\
  if (fts_is_int( a1))							\
    {									\
      if (fts_is_int( a2))						\
	fts_set_int( ret, fts_get_int( a1) OP fts_get_int( a2)); 	\
      else if (fts_is_double( a2))					\
	fts_set_int( ret, fts_get_int( a1) OP fts_get_double( a2)); 	\
      else								\
	fts_set_void( ret);						\
    }									\
  else if (fts_is_double( a1))						\
    {									\
      if (fts_is_int( a2))						\
	fts_set_int( ret, fts_get_double( a1) OP fts_get_int( a2)); 	\
      else if (fts_is_double( a2))					\
	fts_set_int( ret, fts_get_double( a1) OP fts_get_double( a2)); 	\
      else								\
	fts_set_void( ret);						\
    }									\
  else									\
    fts_set_void( ret);

#ifndef YYSTYPE
#define YYSTYPE int
#endif
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		61
#define	YYFLAG		-32768
#define	YYNTBASE	38

#define YYTRANSLATE(x) ((unsigned)(x) <= 291 ? yytranslate[x] : 48)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     6,     8,    11,    12,    14,    16,    18,    20,
    22,    24,    26,    28,    32,    35,    38,    41,    45,    49,
    53,    57,    61,    65,    69,    73,    77,    81,    85,    89,
    93,    97,   101,   103,   108
};

static const short yyrhs[] = {    39,
     0,    39,    37,    40,     0,    40,     0,    40,    41,     0,
     0,    42,     0,    43,     0,    44,     0,    45,     0,    46,
     0,     3,     0,     4,     0,     5,     0,     8,    41,     9,
     0,    26,    41,     0,    27,    41,     0,    33,    41,     0,
    41,    26,    41,     0,    41,    27,    41,     0,    41,    28,
    41,     0,    41,    29,    41,     0,    41,    30,    41,     0,
    41,    24,    41,     0,    41,    25,    41,     0,    41,    17,
    41,     0,    41,    16,    41,     0,    41,    18,    41,     0,
    41,    19,    41,     0,    41,    20,    41,     0,    41,    21,
    41,     0,    41,    22,    41,     0,    41,    23,    41,     0,
    47,     0,    47,    12,    41,    13,     0,    36,     5,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   142,   144,   145,   147,   148,   150,   151,   152,   153,   154,
   157,   158,   159,   162,   164,   165,   166,   169,   170,   171,
   172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
   182,   183,   186,   187,   190
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","FTS_TOKEN_INT",
"FTS_TOKEN_DOUBLE","FTS_TOKEN_SYMBOL","FTS_TOKEN_SEMI","FTS_TOKEN_COLON","FTS_TOKEN_OPEN_PAR",
"FTS_TOKEN_CLOSED_PAR","FTS_TOKEN_OPEN_CPAR","FTS_TOKEN_CLOSED_CPAR","FTS_TOKEN_OPEN_SQPAR",
"FTS_TOKEN_CLOSED_SQPAR","FTS_TOKEN_SPACE","FTS_TOKEN_EQUAL","FTS_TOKEN_LOGICAL_OR",
"FTS_TOKEN_LOGICAL_AND","FTS_TOKEN_EQUAL_EQUAL","FTS_TOKEN_NOT_EQUAL","FTS_TOKEN_GREATER",
"FTS_TOKEN_GREATER_EQUAL","FTS_TOKEN_SMALLER","FTS_TOKEN_SMALLER_EQUAL","FTS_TOKEN_SHIFT_LEFT",
"FTS_TOKEN_SHIFT_RIGHT","FTS_TOKEN_PLUS","FTS_TOKEN_MINUS","FTS_TOKEN_TIMES",
"FTS_TOKEN_DIV","FTS_TOKEN_PERCENT","FTS_TOKEN_UNARY_MINUS","FTS_TOKEN_UNARY_PLUS",
"FTS_TOKEN_LOGICAL_NOT","FTS_TOKEN_ARRAY_INDEX","FTS_TOKEN_DOT","FTS_TOKEN_DOLLAR",
"\";\"","program","list","tuple","term","primitive","par_op","unary_op","binary_op",
"ref","variable", NULL
};
#endif

static const short yyr1[] = {     0,
    38,    39,    39,    40,    40,    41,    41,    41,    41,    41,
    42,    42,    42,    43,    44,    44,    44,    45,    45,    45,
    45,    45,    45,    45,    45,    45,    45,    45,    45,    45,
    45,    45,    46,    46,    47
};

static const short yyr2[] = {     0,
     1,     3,     1,     2,     0,     1,     1,     1,     1,     1,
     1,     1,     1,     3,     2,     2,     2,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     1,     4,     2
};

static const short yydefact[] = {     5,
     1,     3,     5,    11,    12,    13,     0,     0,     0,     0,
     0,     4,     6,     7,     8,     9,    10,    33,     2,     0,
    15,    16,    17,    35,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    14,    26,    25,    27,    28,    29,    30,    31,    32,    23,
    24,    18,    19,    20,    21,    22,     0,    34,     0,     0,
     0
};

static const short yydefgoto[] = {    59,
     1,     2,    12,    13,    14,    15,    16,    17,    18
};

static const short yypact[] = {-32768,
   -23,     8,-32768,-32768,-32768,-32768,     8,     8,     8,     8,
    10,    62,-32768,-32768,-32768,-32768,-32768,     5,     8,    29,
-32768,-32768,-32768,-32768,     8,     8,     8,     8,     8,     8,
     8,     8,     8,     8,     8,     8,     8,     8,     8,     8,
-32768,    76,    89,   100,   100,   -20,   -20,   -20,   -20,   105,
   105,   108,   108,-32768,-32768,-32768,    47,-32768,    36,    37,
-32768
};

static const short yypgoto[] = {-32768,
-32768,    39,    -7,-32768,-32768,-32768,-32768,-32768,-32768
};


#define	YYLAST		138


static const short yytable[] = {    20,
    21,    22,    23,    33,    34,    35,    36,    37,    38,    39,
     4,     5,     6,     3,    24,     7,    40,    42,    43,    44,
    45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
    55,    56,    57,     8,     9,    60,    61,    41,     0,     0,
    10,    19,     0,    11,    25,    26,    27,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    58,
     0,     0,    25,    26,    27,    28,    29,    30,    31,    32,
    33,    34,    35,    36,    37,    38,    39,    25,    26,    27,
    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
    38,    39,    26,    27,    28,    29,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    27,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    35,    36,    37,    38,    39,    37,    38,    39
};

static const short yycheck[] = {     7,
     8,     9,    10,    24,    25,    26,    27,    28,    29,    30,
     3,     4,     5,    37,     5,     8,    12,    25,    26,    27,
    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
    38,    39,    40,    26,    27,     0,     0,     9,    -1,    -1,
    33,     3,    -1,    36,    16,    17,    18,    19,    20,    21,
    22,    23,    24,    25,    26,    27,    28,    29,    30,    13,
    -1,    -1,    16,    17,    18,    19,    20,    21,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    16,    17,    18,
    19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
    29,    30,    17,    18,    19,    20,    21,    22,    23,    24,
    25,    26,    27,    28,    29,    30,    18,    19,    20,    21,
    22,    23,    24,    25,    26,    27,    28,    29,    30,    20,
    21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    26,    27,    28,    29,    30,    28,    29,    30
};
#define YYPURE 1

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/lib/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/lib/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 14:
#line 162 "parser.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 15:
#line 164 "parser.y"
{ ARITH_UNNOP( &(yyval), &(yyvsp[0]), +); ;
    break;}
case 16:
#line 165 "parser.y"
{ ARITH_UNNOP( &(yyval), &(yyvsp[0]), -); ;
    break;}
case 17:
#line 166 "parser.y"
{ ARITH_IUNNOP( &(yyval), &(yyvsp[0]), !); ;
    break;}
case 18:
#line 169 "parser.y"
{ ARITH_BINOP( &(yyval), &(yyvsp[-2]), &(yyvsp[0]), +); ;
    break;}
case 19:
#line 170 "parser.y"
{ ARITH_BINOP( &(yyval), &(yyvsp[-2]), &(yyvsp[0]), -); ;
    break;}
case 20:
#line 171 "parser.y"
{ ARITH_BINOP( &(yyval), &(yyvsp[-2]), &(yyvsp[0]), *); ;
    break;}
case 21:
#line 172 "parser.y"
{ ARITH_BINOP( &(yyval), &(yyvsp[-2]), &(yyvsp[0]), /); ;
    break;}
case 22:
#line 173 "parser.y"
{ ARITH_IBINOP( &(yyval), &(yyvsp[-2]), &(yyvsp[0]), %); ;
    break;}
case 23:
#line 174 "parser.y"
{ ARITH_IBINOP( &(yyval), &(yyvsp[-2]), &(yyvsp[0]), <<); ;
    break;}
case 24:
#line 175 "parser.y"
{ ARITH_IBINOP( &(yyval), &(yyvsp[-2]), &(yyvsp[0]), >>); ;
    break;}
case 25:
#line 176 "parser.y"
{ ARITH_IBINOP( &(yyval), &(yyvsp[-2]), &(yyvsp[0]), &&); ;
    break;}
case 26:
#line 177 "parser.y"
{ ARITH_IBINOP( &(yyval), &(yyvsp[-2]), &(yyvsp[0]), ||); ;
    break;}
case 27:
#line 178 "parser.y"
{ LOGIC_BINOP( &(yyval), &(yyvsp[-2]), &(yyvsp[0]), ==); ;
    break;}
case 28:
#line 179 "parser.y"
{ LOGIC_BINOP( &(yyval), &(yyvsp[-2]), &(yyvsp[0]), !=); ;
    break;}
case 29:
#line 180 "parser.y"
{ LOGIC_BINOP( &(yyval), &(yyvsp[-2]), &(yyvsp[0]), >); ;
    break;}
case 30:
#line 181 "parser.y"
{ LOGIC_BINOP( &(yyval), &(yyvsp[-2]), &(yyvsp[0]), >=); ;
    break;}
case 31:
#line 182 "parser.y"
{ LOGIC_BINOP( &(yyval), &(yyvsp[-2]), &(yyvsp[0]), <); ;
    break;}
case 32:
#line 183 "parser.y"
{ LOGIC_BINOP( &(yyval), &(yyvsp[-2]), &(yyvsp[0]), <=); ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/lib/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 193 "parser.y"


/* **********************************************************************
 *
 * Parser extra functions
 *
 */

static int yyerror( const char *msg)
{
  fprintf( stderr, "%s\n", msg);

  return 0;
}


/* **********************************************************************
 * 
 * Atom array parser + scanner
 *
 */

static fts_hashtable_t token_table;

void fts_parser_init( fts_parser_t *parser, int ac, fts_atom_t *at)
{
  int i;

  parser->ac = ac;
  parser->at = at;

  parser->tokens = (int *)fts_malloc( ac * sizeof( int));

  for ( i = 0; i < ac; i++)
    {
      if ( fts_is_symbol( at+i))
	{
	  fts_atom_t k, v;

	  k = *at;
	  if (fts_hashtable_get( &token_table, &k, &v))
	    parser->tokens[i] = fts_get_int( &v);
	  else
	    parser->tokens[i] = FTS_TOKEN_SYMBOL;
	}
      else if (fts_is_int( at+i))
	parser->tokens[i] = FTS_TOKEN_INT;
      else if (fts_is_double( at+i))
	parser->tokens[i] = FTS_TOKEN_DOUBLE;
    }
}

static int yylex( YYSTYPE *lvalp, void *data)
{
  fts_parser_t *parser = (fts_parser_t *)data;
  fts_atom_t *at = parser->at;
  int token = -1;

  if (parser->ac <= 0)
    return 0; /* end of file */

  token = *parser->current_token;
  parser->current_token++;

  *lvalp = *at;

  parser->at++;
  parser->ac--;

  return token;
}

int fts_parser_run( fts_parser_t *parser)
{
  parser->current_token = parser->tokens;

  return yyparse( parser);
}

void fts_parser_destroy( fts_parser_t *parser)
{
  fts_free( parser->tokens);
}


/* **********************************************************************
 *
 * Kernel initialization
 *
 */

void fts_kernel_parser_init( void)
{
  fts_atom_t k, v;

  fts_hashtable_init( &token_table, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_MEDIUM);

#define PUT(S,T) fts_set_symbol( &k, S); fts_set_int( &v, T); fts_hashtable_put( &token_table, &k, &v);
  PUT( fts_s_dollar, FTS_TOKEN_DOLLAR);
  PUT( fts_s_semi, FTS_TOKEN_SEMI);
  PUT( fts_s_plus, FTS_TOKEN_PLUS);
  PUT( fts_s_minus, FTS_TOKEN_MINUS);
  PUT( fts_s_times, FTS_TOKEN_TIMES);
  PUT( fts_s_div, FTS_TOKEN_DIV);
  PUT( fts_s_open_par, FTS_TOKEN_OPEN_PAR);
  PUT( fts_s_closed_par, FTS_TOKEN_CLOSED_PAR);
  PUT( fts_s_open_sqpar, FTS_TOKEN_OPEN_SQPAR);
  PUT( fts_s_closed_sqpar, FTS_TOKEN_CLOSED_SQPAR);
  PUT( fts_s_open_cpar, FTS_TOKEN_OPEN_CPAR);
  PUT( fts_s_closed_cpar, FTS_TOKEN_CLOSED_CPAR);
  PUT( fts_s_dot, FTS_TOKEN_DOT);
  PUT( fts_s_percent, FTS_TOKEN_PERCENT);
  PUT( fts_s_shift_left, FTS_TOKEN_SHIFT_LEFT);
  PUT( fts_s_shift_right, FTS_TOKEN_SHIFT_RIGHT);
  PUT( fts_s_logical_and, FTS_TOKEN_LOGICAL_AND);
  PUT( fts_s_logical_or, FTS_TOKEN_LOGICAL_OR);
  PUT( fts_s_logical_not, FTS_TOKEN_LOGICAL_NOT);
  PUT( fts_s_equal_equal, FTS_TOKEN_EQUAL_EQUAL);
  PUT( fts_s_not_equal, FTS_TOKEN_NOT_EQUAL);
  PUT( fts_s_greater, FTS_TOKEN_GREATER);
  PUT( fts_s_greater_equal, FTS_TOKEN_GREATER_EQUAL);
  PUT( fts_s_smaller, FTS_TOKEN_SMALLER);
  PUT( fts_s_smaller_equal, FTS_TOKEN_SMALLER_EQUAL);
  PUT( fts_s_colon, FTS_TOKEN_COLON);
  PUT( fts_s_equal, FTS_TOKEN_EQUAL);
}
