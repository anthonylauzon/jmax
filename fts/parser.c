
/*  A Bison parser, made from parser.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

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
#define	TK_SPACE	270
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

#line 23 "parser.y"

#include <stdio.h>
#include <fts/fts.h>
#include <ftsprivate/parser.h>

  /* #define this to 1 if you want a lot of debug printout of the parser */
#define YYDEBUG 0

#ifndef STANDALONE
#define free fts_free
#define malloc fts_malloc
static int yylex();
#else
extern int yylex();
extern void tokenizer_init( const char *s);
#endif

static int yyerror( const char *msg);

static fts_parsetree_t *fts_parsetree_new( int token, fts_atom_t *value, fts_parsetree_t *left, fts_parsetree_t *right);

static struct _parser_data {
  int ac;
  const fts_atom_t *at;
  fts_parsetree_t *tree;
} parser_data;

static fts_status_description_t syntax_error_status_description = {
  "Syntax error"
};
fts_status_t syntax_error_status = &syntax_error_status_description;


#line 57 "parser.y"
typedef union {
  fts_atom_t a;
  fts_parsetree_t *n;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		97
#define	YYFLAG		-32768
#define	YYNTBASE	40

#define YYTRANSLATE(x) ((unsigned)(x) <= 293 ? yytranslate[x] : 55)

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
    37,    38,    39
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     6,     8,     9,    11,    13,    15,    20,    24,
    26,    28,    30,    32,    34,    36,    38,    40,    42,    44,
    46,    48,    50,    52,    54,    56,    58,    61,    63,    68,
    71,    72,    74,    76,    78,    80,    82,    84,    86,    88,
    92,    95,    98,   101,   105,   109,   113,   117,   121,   125,
   129,   133,   137,   141,   145,   149,   153,   157,   161,   165,
   170,   172,   175
};

static const short yyrhs[] = {    41,
     0,    41,    15,    42,     0,    42,     0,     0,    43,     0,
    45,     0,    46,     0,     5,    39,    44,    47,     0,    39,
    44,    47,     0,     5,     0,    27,     0,    28,     0,    29,
     0,    30,     0,    31,     0,    25,     0,    26,     0,    21,
     0,    22,     0,    23,     0,    24,     0,    19,     0,    20,
     0,    34,     0,    17,     0,    18,     0,    45,    48,     0,
    48,     0,    53,    37,     5,    47,     0,    47,    48,     0,
     0,    49,     0,    50,     0,    51,     0,    52,     0,    53,
     0,     3,     0,     4,     0,     5,     0,     8,    42,     9,
     0,    27,    48,     0,    28,    48,     0,    34,    48,     0,
    48,    27,    48,     0,    48,    28,    48,     0,    48,    29,
    48,     0,    48,    30,    48,     0,    48,    35,    48,     0,
    48,    31,    48,     0,    48,    25,    48,     0,    48,    26,
    48,     0,    48,    18,    48,     0,    48,    17,    48,     0,
    48,    19,    48,     0,    48,    20,    48,     0,    48,    21,
    48,     0,    48,    22,    48,     0,    48,    23,    48,     0,
    48,    24,    48,     0,    54,    13,    42,    14,     0,    54,
     0,    38,     5,     0,    38,     3,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   129,   133,   135,   137,   141,   142,   143,   146,   148,   152,
   153,   155,   157,   159,   161,   163,   165,   167,   169,   171,
   173,   175,   177,   179,   181,   183,   187,   189,   192,   196,
   198,   202,   203,   204,   205,   206,   209,   211,   213,   217,
   221,   223,   225,   229,   231,   233,   235,   237,   239,   241,
   243,   245,   247,   249,   251,   253,   255,   257,   259,   263,
   265,   267,   269
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","TK_INT",
"TK_FLOAT","TK_SYMBOL","TK_SEMI","TK_PAR","TK_OPEN_PAR","TK_CLOSED_PAR","TK_CPAR",
"TK_OPEN_CPAR","TK_CLOSED_CPAR","TK_OPEN_SQPAR","TK_CLOSED_SQPAR","TK_COMMA",
"TK_SPACE","TK_LOGICAL_OR","TK_LOGICAL_AND","TK_EQUAL_EQUAL","TK_NOT_EQUAL",
"TK_GREATER","TK_GREATER_EQUAL","TK_SMALLER","TK_SMALLER_EQUAL","TK_SHIFT_LEFT",
"TK_SHIFT_RIGHT","TK_PLUS","TK_MINUS","TK_TIMES","TK_DIV","TK_PERCENT","TK_UMINUS",
"TK_UPLUS","TK_LOGICAL_NOT","TK_POWER","TK_SQPAR","TK_DOT","TK_DOLLAR","TK_COLON",
"toplevel","comma_expression_list","expression","instance","class_name","tuple",
"invocation","term_list","term","primitive","par","unary","binary","reference",
"variable", NULL
};
#endif

static const short yyr1[] = {     0,
    40,    41,    41,    41,    42,    42,    42,    43,    43,    44,
    44,    44,    44,    44,    44,    44,    44,    44,    44,    44,
    44,    44,    44,    44,    44,    44,    45,    45,    46,    47,
    47,    48,    48,    48,    48,    48,    49,    49,    49,    50,
    51,    51,    51,    52,    52,    52,    52,    52,    52,    52,
    52,    52,    52,    52,    52,    52,    52,    52,    52,    53,
    53,    54,    54
};

static const short yyr2[] = {     0,
     1,     3,     1,     0,     1,     1,     1,     4,     3,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     2,     1,     4,     2,
     0,     1,     1,     1,     1,     1,     1,     1,     1,     3,
     2,     2,     2,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     4,
     1,     2,     2
};

static const short yydefact[] = {     4,
    37,    38,    39,     0,     0,     0,     0,     0,     0,     1,
     3,     5,     6,     7,    28,    32,    33,    34,    35,    36,
    61,     0,     0,    39,    41,    36,    42,    43,    63,    62,
    10,    25,    26,    22,    23,    18,    19,    20,    21,    16,
    17,    11,    12,    13,    14,    15,    24,    31,     0,    27,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    31,    40,
     9,     2,    53,    52,    54,    55,    56,    57,    58,    59,
    50,    51,    44,    45,    46,    47,    49,    48,    31,     0,
     8,    30,    29,    60,     0,     0,     0
};

static const short yydefgoto[] = {    95,
    10,    11,    12,    48,    13,    14,    71,    15,    16,    17,
    18,    19,    26,    21
};

static const short yypact[] = {     1,
-32768,-32768,   -32,     1,    64,    64,    64,    16,    88,    -1,
-32768,-32768,    64,-32768,   106,-32768,-32768,-32768,-32768,   -22,
     7,    88,    15,-32768,   -13,-32768,   -13,   -13,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,     1,   106,
    64,    64,    64,    64,    64,    64,    64,    64,    64,    64,
    64,    64,    64,    64,    64,    64,    20,     1,-32768,-32768,
    64,-32768,   124,   141,   156,   156,    48,    48,    48,    48,
     3,     3,   -19,   -19,   -13,   -13,   -13,-32768,-32768,    12,
    64,   106,    64,-32768,    27,    36,-32768
};

static const short yypgoto[] = {-32768,
-32768,    14,-32768,    19,-32768,-32768,   -66,    -5,-32768,-32768,
-32768,-32768,    13,-32768
};


#define	YYLAST		191


static const short yytable[] = {    25,
    27,    28,    91,     1,     2,     3,    22,    50,     4,    63,
    64,    65,    20,    49,    67,    66,    20,    23,    29,    68,
    30,    66,    93,    70,    89,    94,    96,     5,     6,    61,
    62,    63,    64,    65,     7,    97,     0,    66,     8,     9,
    69,     0,     0,     0,     0,    73,    74,    75,    76,    77,
    78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
    88,    20,    72,     0,     0,    92,     1,     2,    24,     0,
     0,     4,    59,    60,    61,    62,    63,    64,    65,     0,
    20,    90,    66,     0,     0,    92,     0,    92,     0,     0,
     5,     6,    31,     0,     0,     0,     0,     7,     0,     0,
     0,     8,     0,     0,    32,    33,    34,    35,    36,    37,
    38,    39,    40,    41,    42,    43,    44,    45,    46,     0,
     0,    47,    51,    52,    53,    54,    55,    56,    57,    58,
    59,    60,    61,    62,    63,    64,    65,     0,     0,     0,
    66,    52,    53,    54,    55,    56,    57,    58,    59,    60,
    61,    62,    63,    64,    65,     0,     0,     0,    66,    53,
    54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
    64,    65,     0,     0,     0,    66,    55,    56,    57,    58,
    59,    60,    61,    62,    63,    64,    65,     0,     0,     0,
    66
};

static const short yycheck[] = {     5,
     6,     7,    69,     3,     4,     5,    39,    13,     8,    29,
    30,    31,     0,    15,    37,    35,     4,     4,     3,    13,
     5,    35,    89,     9,     5,    14,     0,    27,    28,    27,
    28,    29,    30,    31,    34,     0,    -1,    35,    38,    39,
    22,    -1,    -1,    -1,    -1,    51,    52,    53,    54,    55,
    56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
    66,    49,    49,    -1,    -1,    71,     3,     4,     5,    -1,
    -1,     8,    25,    26,    27,    28,    29,    30,    31,    -1,
    68,    68,    35,    -1,    -1,    91,    -1,    93,    -1,    -1,
    27,    28,     5,    -1,    -1,    -1,    -1,    34,    -1,    -1,
    -1,    38,    -1,    -1,    17,    18,    19,    20,    21,    22,
    23,    24,    25,    26,    27,    28,    29,    30,    31,    -1,
    -1,    34,    17,    18,    19,    20,    21,    22,    23,    24,
    25,    26,    27,    28,    29,    30,    31,    -1,    -1,    -1,
    35,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    -1,    -1,    -1,    35,    19,
    20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
    30,    31,    -1,    -1,    -1,    35,    21,    22,    23,    24,
    25,    26,    27,    28,    29,    30,    31,    -1,    -1,    -1,
    35
};
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

case 1:
#line 130 "parser.y"
{ parser_data.tree = yyvsp[0].n; ;
    break;}
case 2:
#line 134 "parser.y"
{ yyval.n = fts_parsetree_new( TK_COMMA, 0, yyvsp[-2].n, yyvsp[0].n); ;
    break;}
case 3:
#line 136 "parser.y"
{ yyval.n = fts_parsetree_new( TK_COMMA, 0, 0, yyvsp[0].n); ;
    break;}
case 4:
#line 138 "parser.y"
{ yyval.n = 0; ;
    break;}
case 8:
#line 147 "parser.y"
{ yyval.n = fts_parsetree_new( TK_COLON, &(yyvsp[-1].a), fts_parsetree_new( TK_SYMBOL, &(yyvsp[-3].a), 0, 0), yyvsp[0].n); ;
    break;}
case 9:
#line 149 "parser.y"
{ yyval.n = fts_parsetree_new( TK_COLON, &(yyvsp[-1].a), 0, yyvsp[0].n); ;
    break;}
case 11:
#line 154 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_plus); ;
    break;}
case 12:
#line 156 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_minus); ;
    break;}
case 13:
#line 158 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_times); ;
    break;}
case 14:
#line 160 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_div); ;
    break;}
case 15:
#line 162 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_percent); ;
    break;}
case 16:
#line 164 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_shift_left); ;
    break;}
case 17:
#line 166 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_shift_right); ;
    break;}
case 18:
#line 168 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_greater); ;
    break;}
case 19:
#line 170 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_greater_equal); ;
    break;}
case 20:
#line 172 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_smaller); ;
    break;}
case 21:
#line 174 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_smaller_equal); ;
    break;}
case 22:
#line 176 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_equal_equal); ;
    break;}
case 23:
#line 178 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_not_equal); ;
    break;}
case 24:
#line 180 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_logical_not); ;
    break;}
case 25:
#line 182 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_logical_or); ;
    break;}
case 26:
#line 184 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_logical_and); ;
    break;}
case 27:
#line 188 "parser.y"
{ yyval.n = fts_parsetree_new( TK_SPACE, 0, yyvsp[-1].n, yyvsp[0].n); ;
    break;}
case 29:
#line 193 "parser.y"
{ yyval.n = fts_parsetree_new( TK_DOT, &(yyvsp[-1].a), yyvsp[-3].n, yyvsp[0].n); ;
    break;}
case 30:
#line 197 "parser.y"
{ yyval.n = fts_parsetree_new( TK_SPACE, 0, yyvsp[-1].n, yyvsp[0].n); ;
    break;}
case 31:
#line 199 "parser.y"
{ yyval.n = 0; ;
    break;}
case 37:
#line 210 "parser.y"
{ yyval.n = fts_parsetree_new( TK_INT, &(yyvsp[0].a), 0, 0); ;
    break;}
case 38:
#line 212 "parser.y"
{ yyval.n = fts_parsetree_new( TK_FLOAT, &(yyvsp[0].a), 0, 0); ;
    break;}
case 39:
#line 214 "parser.y"
{ yyval.n = fts_parsetree_new( TK_SYMBOL, &(yyvsp[0].a), 0, 0); ;
    break;}
case 40:
#line 218 "parser.y"
{ yyval.n = fts_parsetree_new( TK_PAR, 0, 0, yyvsp[-1].n); ;
    break;}
case 41:
#line 222 "parser.y"
{ yyval.n = fts_parsetree_new( TK_UPLUS, 0, yyvsp[0].n, 0); ;
    break;}
case 42:
#line 224 "parser.y"
{ yyval.n = fts_parsetree_new( TK_UMINUS, 0, yyvsp[0].n, 0); ;
    break;}
case 43:
#line 226 "parser.y"
{ yyval.n = fts_parsetree_new( TK_LOGICAL_NOT, 0, yyvsp[0].n, 0); ;
    break;}
case 44:
#line 230 "parser.y"
{ yyval.n = fts_parsetree_new( TK_PLUS, 0, yyvsp[-2].n, yyvsp[0].n); ;
    break;}
case 45:
#line 232 "parser.y"
{ yyval.n = fts_parsetree_new( TK_MINUS, 0, yyvsp[-2].n, yyvsp[0].n); ;
    break;}
case 46:
#line 234 "parser.y"
{ yyval.n = fts_parsetree_new( TK_TIMES, 0, yyvsp[-2].n, yyvsp[0].n); ;
    break;}
case 47:
#line 236 "parser.y"
{ yyval.n = fts_parsetree_new( TK_DIV, 0, yyvsp[-2].n, yyvsp[0].n); ;
    break;}
case 48:
#line 238 "parser.y"
{ yyval.n = fts_parsetree_new( TK_POWER, 0, yyvsp[-2].n, yyvsp[0].n); ;
    break;}
case 49:
#line 240 "parser.y"
{ yyval.n = fts_parsetree_new( TK_PERCENT, 0, yyvsp[-2].n, yyvsp[0].n); ;
    break;}
case 50:
#line 242 "parser.y"
{ yyval.n = fts_parsetree_new( TK_SHIFT_LEFT, 0, yyvsp[-2].n, yyvsp[0].n); ;
    break;}
case 51:
#line 244 "parser.y"
{ yyval.n = fts_parsetree_new( TK_SHIFT_RIGHT, 0, yyvsp[-2].n, yyvsp[0].n); ;
    break;}
case 52:
#line 246 "parser.y"
{ yyval.n = fts_parsetree_new( TK_LOGICAL_AND, 0, yyvsp[-2].n, yyvsp[0].n); ;
    break;}
case 53:
#line 248 "parser.y"
{ yyval.n = fts_parsetree_new( TK_LOGICAL_OR, 0, yyvsp[-2].n, yyvsp[0].n); ;
    break;}
case 54:
#line 250 "parser.y"
{ yyval.n = fts_parsetree_new( TK_EQUAL_EQUAL, 0, yyvsp[-2].n, yyvsp[0].n); ;
    break;}
case 55:
#line 252 "parser.y"
{ yyval.n = fts_parsetree_new( TK_NOT_EQUAL, 0, yyvsp[-2].n, yyvsp[0].n); ;
    break;}
case 56:
#line 254 "parser.y"
{ yyval.n = fts_parsetree_new( TK_GREATER, 0, yyvsp[-2].n, yyvsp[0].n); ;
    break;}
case 57:
#line 256 "parser.y"
{ yyval.n = fts_parsetree_new( TK_GREATER_EQUAL, 0, yyvsp[-2].n, yyvsp[0].n); ;
    break;}
case 58:
#line 258 "parser.y"
{ yyval.n = fts_parsetree_new( TK_SMALLER, 0, yyvsp[-2].n, yyvsp[0].n); ;
    break;}
case 59:
#line 260 "parser.y"
{ yyval.n = fts_parsetree_new( TK_SMALLER_EQUAL, 0, yyvsp[-2].n, yyvsp[0].n); ;
    break;}
case 60:
#line 264 "parser.y"
{ yyval.n = fts_parsetree_new( TK_SQPAR, 0, yyvsp[-3].n, yyvsp[-1].n); ;
    break;}
case 62:
#line 268 "parser.y"
{ yyval.n = fts_parsetree_new( TK_DOLLAR, &(yyvsp[0].a), 0, 0); ;
    break;}
case 63:
#line 270 "parser.y"
{ yyval.n = fts_parsetree_new( TK_DOLLAR, &(yyvsp[0].a), 0, 0); ;
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
#line 273 "parser.y"


#ifndef STANDALONE

/* **********************************************************************
 *
 * FTS code
 *
 */

static fts_hashtable_t fts_token_table;

static int yyerror( const char *msg)
{
  return 0;
}

static int yylex()
{
  int token = -1;
  if (parser_data.ac <= 0)
    return 0; /* end of file */

  if ( fts_is_symbol(parser_data.at))
    {
      fts_atom_t v;

      if (fts_hashtable_get( &fts_token_table, parser_data.at, &v))
	token = fts_get_int( &v);
      else
	{
	  token = TK_SYMBOL;
	  yylval.a = *parser_data.at;
	}
    }
  else if (fts_is_int( parser_data.at))
    {
      token = TK_INT;
      yylval.a = *parser_data.at;
    }
  else if (fts_is_float( parser_data.at))
    {
      token = TK_FLOAT;
      yylval.a = *parser_data.at;
    }

  parser_data.at++;
  parser_data.ac--;

  return token;
}

/* **********************************************************************
 *
 * Parser abstract tree construction/destruction
 *
 */

static fts_heap_t *parsetree_heap;

static fts_parsetree_t *fts_parsetree_new( int token, fts_atom_t *value, fts_parsetree_t *left, fts_parsetree_t *right)
{
  fts_parsetree_t *tree;

  tree = (fts_parsetree_t *)fts_heap_alloc( parsetree_heap);

  tree->token = token;

  if (value)
    tree->value = *value;
  else
    fts_set_void( &tree->value);

  tree->left = left;
  tree->right = right;

  return tree;
}

fts_status_t fts_parsetree_parse( int ac, const fts_atom_t *at, fts_parsetree_t **ptree)
{
  parser_data.ac = ac;
  parser_data.at = at;

  if (yyparse())
    {
      *ptree = NULL;
      return syntax_error_status;
    }

  *ptree = parser_data.tree;
  return fts_ok;
}

void fts_parsetree_delete( fts_parsetree_t *tree)
{
  if (!tree)
    return;

  fts_parsetree_delete( tree->left);
  fts_parsetree_delete( tree->right);
  fts_heap_free( tree, parsetree_heap);
}

/* **********************************************************************
 *
 * Kernel initialization
 *
 */

void fts_kernel_parser_init( void)
{
  parsetree_heap = fts_heap_new( sizeof( fts_parsetree_t));

  fts_hashtable_init( &fts_token_table, fts_symbol_class, FTS_HASHTABLE_MEDIUM);

#define PUT_TOKEN(S,T)					\
 {							\
   fts_atom_t k, v;					\
							\
   fts_set_symbol( &k, S);				\
   fts_set_int( &v, T);					\
   fts_hashtable_put( &fts_token_table, &k, &v);	\
 }

  PUT_TOKEN( fts_s_dollar, TK_DOLLAR);
  PUT_TOKEN( fts_s_semi, TK_SEMI);
  PUT_TOKEN( fts_s_comma, TK_COMMA);
  PUT_TOKEN( fts_s_plus, TK_PLUS);
  PUT_TOKEN( fts_s_minus, TK_MINUS);
  PUT_TOKEN( fts_s_times, TK_TIMES);
  PUT_TOKEN( fts_s_div, TK_DIV);
  PUT_TOKEN( fts_s_power, TK_POWER);
  PUT_TOKEN( fts_s_open_par, TK_OPEN_PAR);
  PUT_TOKEN( fts_s_closed_par, TK_CLOSED_PAR);
  PUT_TOKEN( fts_s_open_sqpar, TK_OPEN_SQPAR);
  PUT_TOKEN( fts_s_closed_sqpar, TK_CLOSED_SQPAR);
  PUT_TOKEN( fts_s_open_cpar, TK_OPEN_CPAR);
  PUT_TOKEN( fts_s_closed_cpar, TK_CLOSED_CPAR);
  PUT_TOKEN( fts_s_dot, TK_DOT);
  PUT_TOKEN( fts_s_percent, TK_PERCENT);
  PUT_TOKEN( fts_s_shift_left, TK_SHIFT_LEFT);
  PUT_TOKEN( fts_s_shift_right, TK_SHIFT_RIGHT);
  PUT_TOKEN( fts_s_logical_and, TK_LOGICAL_AND);
  PUT_TOKEN( fts_s_logical_or, TK_LOGICAL_OR);
  PUT_TOKEN( fts_s_logical_not, TK_LOGICAL_NOT);
  PUT_TOKEN( fts_s_equal_equal, TK_EQUAL_EQUAL);
  PUT_TOKEN( fts_s_not_equal, TK_NOT_EQUAL);
  PUT_TOKEN( fts_s_greater, TK_GREATER);
  PUT_TOKEN( fts_s_greater_equal, TK_GREATER_EQUAL);
  PUT_TOKEN( fts_s_smaller, TK_SMALLER);
  PUT_TOKEN( fts_s_smaller_equal, TK_SMALLER_EQUAL);
  PUT_TOKEN( fts_s_colon, TK_COLON);

#if YYDEBUG
  yydebug = 1;
#endif
}

#else

/* **********************************************************************
 *
 * Standalone code
 *
 */

#define PREDEF_SYMBOL(V,S) fts_symbol_t V = S;
#include <fts/predefsymbols.h>

static int yyerror( const char *msg)
{
  fprintf( stderr, "***** %s\n", msg);
  return 0;
}

static fts_parsetree_t *fts_parsetree_new( int token, fts_atom_t *value, fts_parsetree_t *left, fts_parsetree_t *right)
{
  fts_parsetree_t *tree;

  tree = (fts_parsetree_t *)malloc( sizeof( fts_parsetree_t));

  tree->token = token;

  if (value)
    tree->value = *value;
  else
    fts_set_void( &tree->value);

  tree->left = left;
  tree->right = right;

  return tree;
}

fts_status_t fts_parsetree_parse( int ac, const fts_atom_t *at, fts_parsetree_t **ptree)
{
  parser_data.ac = ac;
  parser_data.at = at;

  if (yyparse())
    {
      *ptree = NULL;
      return syntax_error_status;
    }

  *ptree = parser_data.tree;
  return fts_ok;
}

void fts_parsetree_delete( fts_parsetree_t *tree)
{
  if (!tree)
    return;

  fts_parsetree_delete( tree->left);
  fts_parsetree_delete( tree->right);
  free( tree);
}

static void parsetree_print_aux( fts_parsetree_t *tree, int indent)
{
  int i;

  fprintf( stderr, "%d:", indent);

  if (!tree)
    {
      fprintf( stderr, "\n");
      return;
    }

  for ( i = 0; i < indent; i++)
    fprintf( stderr, "   ");

  switch( tree->token) {
  case TK_DOT: fprintf( stderr, ".%s\n", fts_get_symbol( &tree->value)); break;
  case TK_COLON: fprintf( stderr, ": %s\n", fts_get_symbol( &tree->value)); break;
  case TK_COMMA: fprintf( stderr, ",\n"); break;
  case TK_SPACE: fprintf( stderr, "SPACE\n"); break;
  case TK_INT: fprintf( stderr, "INT %d\n", fts_get_int( &tree->value)); break;
  case TK_FLOAT: fprintf( stderr, "FLOAT %g\n", fts_get_float( &tree->value)); break;
  case TK_SYMBOL: 
    {
      fts_symbol_t s = fts_get_symbol( &tree->value);
      fprintf( stderr, "SYMBOL %s\n", (s != NULL) ? s : "null");
    }
    break;
  case TK_PAR: fprintf( stderr, "()\n"); break;
  case TK_CPAR: fprintf( stderr, "{}\n"); break;
  case TK_SQPAR: fprintf( stderr, "[]\n"); break;
  case TK_DOLLAR: 
    if (fts_is_int( &tree->value))
      fprintf( stderr, "$%d\n", fts_get_int( &tree->value)); 
    else if (fts_is_symbol( &tree->value))
      fprintf( stderr, "$%s\n", fts_get_symbol( &tree->value)); 
    break;
  case TK_UPLUS: fprintf( stderr, "+u\n"); break;
  case TK_UMINUS: fprintf( stderr, "-u\n"); break;
  case TK_LOGICAL_NOT: fprintf( stderr, "!\n"); break;
  case TK_PLUS: fprintf( stderr, "+\n"); break;
  case TK_MINUS: fprintf( stderr, "-\n"); break;
  case TK_TIMES: fprintf( stderr, "*\n"); break;
  case TK_DIV: fprintf( stderr, "/\n"); break;
  case TK_POWER: fprintf( stderr, "**\n"); break;
  case TK_PERCENT: fprintf( stderr, "%%\n"); break;
  case TK_SHIFT_LEFT: fprintf( stderr, "<<\n"); break;
  case TK_SHIFT_RIGHT: fprintf( stderr, ">>\n"); break;
  case TK_LOGICAL_AND: fprintf( stderr, "&&\n"); break;
  case TK_LOGICAL_OR: fprintf( stderr, "||\n"); break;
  case TK_EQUAL_EQUAL: fprintf( stderr, "==\n"); break;
  case TK_NOT_EQUAL: fprintf( stderr, "!=\n"); break;
  case TK_GREATER: fprintf( stderr, ">\n"); break;
  case TK_GREATER_EQUAL: fprintf( stderr, ">=\n"); break;
  case TK_SMALLER: fprintf( stderr, "<\n"); break;
  case TK_SMALLER_EQUAL: fprintf( stderr, "<=\n"); break;
  default: fprintf( stderr, "UNKNOWN %d\n", tree->token); 
  }

  parsetree_print_aux( tree->left, indent+1);
  parsetree_print_aux( tree->right, indent+1);
}

void fts_parsetree_print( fts_parsetree_t *tree)
{
  parsetree_print_aux( tree, 0);
}

main( int argc, char **argv)
{
  fts_parsetree_t *tree;

#if YYDEBUG
  yydebug = 1;
#endif

  tokenizer_init( argv[1]);

  if (fts_parsetree_parse( 0, 0, &tree) == fts_ok)
    fts_parsetree_print( tree);
  else
    fprintf( stderr, "Syntax error\n");
}

#endif
