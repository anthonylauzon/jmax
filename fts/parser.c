/* A Bison parser, made from parser.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

# define	FTS_TOKEN_INT	257
# define	FTS_TOKEN_FLOAT	258
# define	FTS_TOKEN_SYMBOL	259
# define	FTS_TOKEN_COLON	260
# define	FTS_TOKEN_PAR	261
# define	FTS_TOKEN_OPEN_PAR	262
# define	FTS_TOKEN_CLOSED_PAR	263
# define	FTS_TOKEN_CPAR	264
# define	FTS_TOKEN_OPEN_CPAR	265
# define	FTS_TOKEN_CLOSED_CPAR	266
# define	FTS_TOKEN_SQPAR	267
# define	FTS_TOKEN_OPEN_SQPAR	268
# define	FTS_TOKEN_CLOSED_SQPAR	269
# define	FTS_TOKEN_SEMI	270
# define	FTS_TOKEN_TUPLE	271
# define	FTS_TOKEN_EQUAL	272
# define	FTS_TOKEN_LOGICAL_OR	273
# define	FTS_TOKEN_LOGICAL_AND	274
# define	FTS_TOKEN_EQUAL_EQUAL	275
# define	FTS_TOKEN_NOT_EQUAL	276
# define	FTS_TOKEN_GREATER	277
# define	FTS_TOKEN_GREATER_EQUAL	278
# define	FTS_TOKEN_SMALLER	279
# define	FTS_TOKEN_SMALLER_EQUAL	280
# define	FTS_TOKEN_UMINUS	281
# define	FTS_TOKEN_UPLUS	282
# define	FTS_TOKEN_SHIFT_LEFT	283
# define	FTS_TOKEN_SHIFT_RIGHT	284
# define	FTS_TOKEN_PLUS	285
# define	FTS_TOKEN_MINUS	286
# define	FTS_TOKEN_TIMES	287
# define	FTS_TOKEN_DIV	288
# define	FTS_TOKEN_PERCENT	289
# define	FTS_TOKEN_LOGICAL_NOT	290
# define	FTS_TOKEN_POWER	291
# define	FTS_TOKEN_ARRAY_INDEX	292
# define	FTS_TOKEN_DOT	293
# define	FTS_TOKEN_DOLLAR	294

#line 23 "parser.y"

#include <math.h>
#include <stdio.h>
#include <fts/fts.h>

#define free fts_free
#define malloc fts_malloc
#define YYPARSE_PARAM data
#define YYLEX_PARAM data

static int yylex();
static int yyerror( const char *msg);

typedef struct _pnode_t pnode_t;
static pnode_t *pnode_new( int token, fts_atom_t *value, pnode_t *left, pnode_t *right);


#line 43 "parser.y"
#ifndef YYSTYPE
typedef union {
  fts_atom_t a;
  pnode_t *n;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		65
#define	YYFLAG		-32768
#define	YYNTBASE	41

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 294 ? yytranslate[x] : 51)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     2,     6,     8,     9,    12,    14,    16,    18,
      20,    22,    24,    26,    28,    30,    34,    37,    40,    43,
      47,    51,    55,    59,    63,    67,    71,    75,    79,    83,
      87,    91,    95,    99,   103,   107,   109,   114,   117
};
static const short yyrhs[] =
{
      42,     0,    42,    16,    43,     0,    43,     0,     0,    43,
      44,     0,    44,     0,    45,     0,    46,     0,    47,     0,
      48,     0,    49,     0,     3,     0,     4,     0,     5,     0,
       8,    43,     9,     0,    31,    44,     0,    32,    44,     0,
      36,    44,     0,    44,    31,    44,     0,    44,    32,    44,
       0,    44,    33,    44,     0,    44,    34,    44,     0,    44,
      37,    44,     0,    44,    35,    44,     0,    44,    29,    44,
       0,    44,    30,    44,     0,    44,    20,    44,     0,    44,
      19,    44,     0,    44,    21,    44,     0,    44,    22,    44,
       0,    44,    23,    44,     0,    44,    24,    44,     0,    44,
      25,    44,     0,    44,    26,    44,     0,    50,     0,    50,
      14,    43,    15,     0,    40,     5,     0,    40,     3,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   108,   112,   114,   116,   120,   122,   126,   127,   128,
     129,   130,   133,   135,   137,   141,   145,   147,   149,   153,
     155,   157,   159,   161,   163,   165,   167,   169,   171,   173,
     175,   177,   179,   181,   183,   187,   188,   191,   193
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "FTS_TOKEN_INT", "FTS_TOKEN_FLOAT", 
  "FTS_TOKEN_SYMBOL", "FTS_TOKEN_COLON", "FTS_TOKEN_PAR", 
  "FTS_TOKEN_OPEN_PAR", "FTS_TOKEN_CLOSED_PAR", "FTS_TOKEN_CPAR", 
  "FTS_TOKEN_OPEN_CPAR", "FTS_TOKEN_CLOSED_CPAR", "FTS_TOKEN_SQPAR", 
  "FTS_TOKEN_OPEN_SQPAR", "FTS_TOKEN_CLOSED_SQPAR", "FTS_TOKEN_SEMI", 
  "FTS_TOKEN_TUPLE", "FTS_TOKEN_EQUAL", "FTS_TOKEN_LOGICAL_OR", 
  "FTS_TOKEN_LOGICAL_AND", "FTS_TOKEN_EQUAL_EQUAL", "FTS_TOKEN_NOT_EQUAL", 
  "FTS_TOKEN_GREATER", "FTS_TOKEN_GREATER_EQUAL", "FTS_TOKEN_SMALLER", 
  "FTS_TOKEN_SMALLER_EQUAL", "FTS_TOKEN_UMINUS", "FTS_TOKEN_UPLUS", 
  "FTS_TOKEN_SHIFT_LEFT", "FTS_TOKEN_SHIFT_RIGHT", "FTS_TOKEN_PLUS", 
  "FTS_TOKEN_MINUS", "FTS_TOKEN_TIMES", "FTS_TOKEN_DIV", 
  "FTS_TOKEN_PERCENT", "FTS_TOKEN_LOGICAL_NOT", "FTS_TOKEN_POWER", 
  "FTS_TOKEN_ARRAY_INDEX", "FTS_TOKEN_DOT", "FTS_TOKEN_DOLLAR", "program", 
  "list", "tuple", "term", "primitive", "par_op", "unary_op", "binary_op", 
  "ref", "variable", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    41,    42,    42,    42,    43,    43,    44,    44,    44,
      44,    44,    45,    45,    45,    46,    47,    47,    47,    48,
      48,    48,    48,    48,    48,    48,    48,    48,    48,    48,
      48,    48,    48,    48,    48,    49,    49,    50,    50
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     1,     3,     1,     0,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     2,     2,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     1,     4,     2,     2
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       4,    12,    13,    14,     0,     0,     0,     0,     0,     1,
       3,     6,     7,     8,     9,    10,    11,    35,     0,    16,
      17,    18,    38,    37,     0,     5,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    15,     2,    28,    27,    29,    30,    31,
      32,    33,    34,    25,    26,    19,    20,    21,    22,    24,
      23,     0,    36,     0,     0,     0
};

static const short yydefgoto[] =
{
      63,     9,    10,    11,    12,    13,    14,    15,    16,    17
};

static const short yypact[] =
{
      58,-32768,-32768,-32768,    58,    58,    58,    58,    35,   -13,
      58,    80,-32768,-32768,-32768,-32768,-32768,   -10,    11,    38,
      38,   -26,-32768,-32768,    58,    80,    58,    58,    58,    58,
      58,    58,    58,    58,    58,    58,    58,    58,    58,    58,
      58,    58,    58,-32768,    58,    98,   115,   130,   130,    38,
      38,    38,    38,   -25,   -25,    49,    49,   -26,   -26,   -26,
  -32768,    45,-32768,    18,    41,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,    13,    -5,-32768,-32768,-32768,-32768,-32768,-32768
};


#define	YYLAST		167


static const short yytable[] =
{
      19,    20,    21,    24,    42,    25,    36,    37,    38,    39,
      40,    41,    41,    25,     1,     2,     3,    18,    64,     4,
      43,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    44,    22,    25,
      23,    65,     5,     6,     0,     0,     0,     7,     1,     2,
       3,     8,     0,     4,     0,    61,    25,     0,     0,     0,
      62,     1,     2,     3,     0,     0,     4,    34,    35,    36,
      37,    38,    39,    40,     0,    41,     5,     6,     0,     0,
       0,     7,    38,    39,    40,     8,    41,     0,     0,     5,
       6,     0,     0,     0,     7,     0,     0,     0,     8,    26,
      27,    28,    29,    30,    31,    32,    33,     0,     0,    34,
      35,    36,    37,    38,    39,    40,     0,    41,    27,    28,
      29,    30,    31,    32,    33,     0,     0,    34,    35,    36,
      37,    38,    39,    40,     0,    41,    28,    29,    30,    31,
      32,    33,     0,     0,    34,    35,    36,    37,    38,    39,
      40,     0,    41,    30,    31,    32,    33,     0,     0,    34,
      35,    36,    37,    38,    39,    40,     0,    41
};

static const short yycheck[] =
{
       5,     6,     7,    16,    14,    10,    31,    32,    33,    34,
      35,    37,    37,    18,     3,     4,     5,     4,     0,     8,
       9,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    24,     3,    44,
       5,     0,    31,    32,    -1,    -1,    -1,    36,     3,     4,
       5,    40,    -1,     8,    -1,    42,    61,    -1,    -1,    -1,
      15,     3,     4,     5,    -1,    -1,     8,    29,    30,    31,
      32,    33,    34,    35,    -1,    37,    31,    32,    -1,    -1,
      -1,    36,    33,    34,    35,    40,    37,    -1,    -1,    31,
      32,    -1,    -1,    -1,    36,    -1,    -1,    -1,    40,    19,
      20,    21,    22,    23,    24,    25,    26,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    37,    20,    21,
      22,    23,    24,    25,    26,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    37,    21,    22,    23,    24,
      25,    26,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    37,    23,    24,    25,    26,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    37
};
#define YYPURE 1

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"

/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software
   Foundation, Inc.

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

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#if ! defined (yyoverflow) || defined (YYERROR_VERBOSE)

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif


#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

#ifdef YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif
#endif

#line 315 "/usr/share/bison/bison.simple"


/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
YY_DECL_VARIABLES
#endif  /* !YYPURE */

int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
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
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
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
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  switch (yyn) {

case 1:
#line 109 "parser.y"
{ ((fts_parser_t *)data)->tree = yyvsp[0].n; }
    break;
case 2:
#line 113 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_SEMI, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 3:
#line 115 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_SEMI, 0, 0, yyvsp[0].n); }
    break;
case 4:
#line 117 "parser.y"
{ yyval.n = 0; }
    break;
case 5:
#line 121 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_TUPLE, 0, yyvsp[-1].n, yyvsp[0].n); }
    break;
case 6:
#line 123 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_TUPLE, 0, 0, yyvsp[0].n); }
    break;
case 12:
#line 134 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_INT, &(yyvsp[0].a), 0, 0); }
    break;
case 13:
#line 136 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_FLOAT, &(yyvsp[0].a), 0, 0); }
    break;
case 14:
#line 138 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_SYMBOL, &(yyvsp[0].a), 0, 0); }
    break;
case 15:
#line 142 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_PAR, 0, 0, yyvsp[-1].n); }
    break;
case 16:
#line 146 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_UPLUS, 0, yyvsp[0].n, 0); }
    break;
case 17:
#line 148 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_UMINUS, 0, yyvsp[0].n, 0); }
    break;
case 18:
#line 150 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_LOGICAL_NOT, 0, yyvsp[0].n, 0); }
    break;
case 19:
#line 154 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_PLUS, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 20:
#line 156 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_MINUS, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 21:
#line 158 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_TIMES, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 22:
#line 160 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_DIV, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 23:
#line 162 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_POWER, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 24:
#line 164 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_PERCENT, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 25:
#line 166 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_SHIFT_LEFT, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 26:
#line 168 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_SHIFT_RIGHT, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 27:
#line 170 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_LOGICAL_AND, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 28:
#line 172 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_LOGICAL_OR, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 29:
#line 174 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_EQUAL_EQUAL, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 30:
#line 176 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_NOT_EQUAL, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 31:
#line 178 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_GREATER, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 32:
#line 180 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_GREATER_EQUAL, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 33:
#line 182 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_SMALLER, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 34:
#line 184 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_SMALLER_EQUAL, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 37:
#line 192 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_DOLLAR, &(yyvsp[0].a), 0, 0); }
    break;
case 38:
#line 194 "parser.y"
{ yyval.n = pnode_new( FTS_TOKEN_DOLLAR, &(yyvsp[0].a), 0, 0); }
    break;
}

#line 705 "/usr/share/bison/bison.simple"


  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:
#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */

  /* If its default is to accept any token, ok.  Otherwise pop it.  */
  yyn = yydefact[yystate];
  if (yyn)
    goto yydefault;
#endif


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "Error: state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

/*--------------.
| yyerrhandle.  |
`--------------*/
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

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}
#line 197 "parser.y"


static fts_hashtable_t token_table;

static int yyerror( const char *msg)
{
  fprintf( stderr, "%s\n", msg);

  return 0;
}

static int yylex( YYSTYPE *lvalp, void *data)
{
  fts_parser_t *parser = (fts_parser_t *)data;
  int token = -1;

  if (parser->ac <= 0)
    return 0; /* end of file */

  if ( fts_is_symbol( parser->at))
    {
      fts_atom_t k, v;

      k = *parser->at;
      if (fts_hashtable_get( &token_table, &k, &v))
	token = fts_get_int( &v);
      else
	token = FTS_TOKEN_SYMBOL;
    }
  else if (fts_is_int( parser->at))
    token = FTS_TOKEN_INT;
  else if (fts_is_float( parser->at))
    token = FTS_TOKEN_FLOAT;

  lvalp->a = *parser->at;

  parser->at++;
  parser->ac--;

  return token;
}

/* **********************************************************************
 * 
 * Abstract tree 
 *
 */

struct _pnode_t {
  int token;
  fts_atom_t value;
  struct _pnode_t *left, *right;
};

static fts_heap_t *pnode_heap;

static pnode_t *pnode_new( int token, fts_atom_t *value, pnode_t *left, pnode_t *right)
{
  pnode_t *node;

  node = fts_heap_alloc( pnode_heap);

  node->token = token;

  if (value)
    node->value = *value;
  else
    fts_set_void( &node->value);

  node->left = left;
  node->right = right;

  return node;
}

static void pnode_walk( pnode_t *node, fts_parser_callback_table_t *callbacks, void *data)
{
  fts_parser_callback_t prefix, infix, postfix;

  if (!node)
    return;

#define CASE( TOKEN, CALLBACK)			\
case TOKEN:					\
  prefix = callbacks->CALLBACK.prefix;		\
  infix = callbacks->CALLBACK.infix;		\
  postfix = callbacks->CALLBACK.postfix;	\
break;

  switch( node->token) {
    CASE( FTS_TOKEN_SEMI, semi);
    CASE( FTS_TOKEN_TUPLE, tuple);
    CASE( FTS_TOKEN_INT, c_int);
    CASE( FTS_TOKEN_FLOAT, c_float);
    CASE( FTS_TOKEN_SYMBOL, symbol);
    CASE( FTS_TOKEN_DOLLAR, dollar);
    CASE( FTS_TOKEN_UPLUS, uplus);
    CASE( FTS_TOKEN_UMINUS, uminus);
    CASE( FTS_TOKEN_LOGICAL_NOT, logical_not);
    CASE( FTS_TOKEN_PLUS, plus);
    CASE( FTS_TOKEN_MINUS, minus);
    CASE( FTS_TOKEN_TIMES, times);
    CASE( FTS_TOKEN_DIV, div);
    CASE( FTS_TOKEN_POWER, power);
    CASE( FTS_TOKEN_PERCENT, percent);
    CASE( FTS_TOKEN_SHIFT_LEFT, shift_left);
    CASE( FTS_TOKEN_SHIFT_RIGHT, shift_right);
    CASE( FTS_TOKEN_LOGICAL_AND, logical_and);
    CASE( FTS_TOKEN_LOGICAL_OR, logical_or);
    CASE( FTS_TOKEN_EQUAL_EQUAL, equal_equal);
    CASE( FTS_TOKEN_NOT_EQUAL, not_equal);
    CASE( FTS_TOKEN_GREATER, greater);
    CASE( FTS_TOKEN_GREATER_EQUAL, greater_equal);
    CASE( FTS_TOKEN_SMALLER, smaller);
    CASE( FTS_TOKEN_SMALLER_EQUAL, smaller_equal);
  default:
    prefix = 0;
    infix = 0;
    postfix = 0;
    break;
  }

  if (prefix)
    (*prefix)( node->token, &node->value, data);

  pnode_walk( node->left, callbacks, data);

  if (infix)
    (*infix)( node->token, &node->value, data);

  pnode_walk( node->right, callbacks, data);

  if (postfix)
    (*postfix)( node->token, &node->value, data);
}

/* **********************************************************************
 *
 * Evaluator
 *
 */

/*
 * The evaluation stack is a stack of atoms, organized into frames.
 *
 * The frame pointer points to the first atom of the frame.
 * The returned value is located at (fp-2)
 *
 *
 * top ->        arg2       argument 2              ----------------
 *               arg1       argument 1              
 * fp ->         arg0       argument 2               current frame
 *               savedfp    saved frame pointer     
 *               retval     return value            ----------------
 *               arg1'      argument 1              ----------------
 * savedfp ->    arg0'      argument 0               previous frame
 *               savedfp'   saved frame pointer     
 *               retval'    return value            ----------------
 *
 */

/* #define EVALSTACK_DEBUG */
#undef EVALSTACK_DEBUG

typedef struct {
  fts_stack_t stack;
  int fp;
} evalstack_t;

#define evalstack_fp(EV) ((EV)->fp)
#define evalstack_push(EV,P) fts_stack_push( &(EV)->stack, fts_atom_t, *(P))
#define evalstack_pop(EV,N) fts_stack_pop( &(EV)->stack, (N))
#define evalstack_base(EV) ((fts_atom_t *)fts_stack_base( &(EV)->stack))
#define evalstack_top(EV) fts_stack_top(&(EV)->stack)

#ifdef EVALSTACK_DEBUG
static void evalstack_print( evalstack_t *ev, const char *msg)
{
  int i, current_fp;
  fts_atom_t *p = evalstack_base( ev);

  fprintf( stderr, "%s:\n", msg);

  current_fp = ev->fp;

  for ( i = fts_stack_top( &ev->stack); i >= 0; i--)
    {
      fprintf( stderr, "[%2d]", i);
      if ( i == current_fp - 1)
	{
	  fprintf( stderr, "* ");
	  current_fp = fts_get_int( p+i);
	}
      else
	fts_log( "  ");

      if (fts_is_int( p+i))
	fprintf( stderr, "INT %d\n", fts_get_int(p+i));
      else if (fts_is_float( p+i))
	fprintf( stderr, "FLOAT %g\n", fts_get_float(p+i));
      else if (fts_is_void( p+i))
	fprintf( stderr, "VOID\n");
      else if (fts_is_symbol( p+i))
	fprintf( stderr, "SYMBOL %s\n", fts_get_symbol(p+i));
    }
}
#endif

static void evalstack_init( evalstack_t *ev)
{
  fts_stack_init( &ev->stack, fts_atom_t);
  ev->fp = 0;
}

static void evalstack_push_frame( evalstack_t *ev)
{
  fts_atom_t a;

  /* return value */
  fts_set_void( &a);
  evalstack_push( ev, &a);

  /* saved frame pointer */
  fts_set_int( &a, ev->fp);
  evalstack_push( ev, &a);
  ev->fp = fts_stack_top( &ev->stack) + 1;

#ifdef EVALSTACK_DEBUG
  evalstack_print( ev, "Stack after pushing frame");
#endif
}

static void evalstack_pop_frame( evalstack_t *ev)
{
  int old_fp;

#ifdef EVALSTACK_DEBUG
  evalstack_print( ev, "Stack before poping frame");
#endif

  old_fp = ev->fp;
  ev->fp = fts_get_int( evalstack_base( ev) + ev->fp - 1);
  evalstack_pop( ev, fts_stack_top( &ev->stack) - old_fp + 2);

#ifdef EVALSTACK_DEBUG
  evalstack_print( ev, "Stack after poping frame");
#endif
}

void evalstack_return( evalstack_t *ev, const fts_atom_t *p)
{
  evalstack_base(ev)[ evalstack_fp(ev) - 2] = *p;
  evalstack_pop_frame( ev);
}

/*
 * Evaluator stack
 */

static evalstack_t parser_eval_stack;

struct _eval_data_t {
  fts_object_t *obj;
  int ac;
  const fts_atom_t *at;
};

/*
 * Evaluator callbacks
 */

static void semi_infix_eval( int token, fts_atom_t *value, void *data)
{
  evalstack_push_frame( &parser_eval_stack);
}

static void semi_postfix_eval( int token, fts_atom_t *value, void *data)
{
  fts_object_t *obj;
  fts_symbol_t selector;
  fts_atom_t *at;
  int ac;

  obj = ((struct _eval_data_t *)data)->obj;

  ac = evalstack_top( &parser_eval_stack) - evalstack_fp( &parser_eval_stack) + 1;
  at = evalstack_base( &parser_eval_stack) + evalstack_fp( &parser_eval_stack);

  if ( fts_is_symbol( at))
    {
      selector = fts_get_symbol( at);
      ac--;
      at++;
    }
  else if (ac == 1)
    selector = fts_get_selector( at);
  else
    selector = fts_s_list;

  fts_outlet_send( obj, 0, selector, ac, at);

  evalstack_pop_frame( &parser_eval_stack);

  /* Pop return value */
  evalstack_pop( &parser_eval_stack, 1);
}

static void par_prefix_eval( int token, fts_atom_t *value, void *data)
{
  evalstack_push_frame( &parser_eval_stack);
}

static void par_postfix_eval( int token, fts_atom_t *value, void *data)
{
  fts_atom_t *at;
  int ac;

  ac = evalstack_top( &parser_eval_stack) - evalstack_fp( &parser_eval_stack) + 1;
  at = evalstack_base( &parser_eval_stack) + evalstack_fp( &parser_eval_stack);

  if (ac == 1)
    evalstack_return( &parser_eval_stack, at);
  else
    {
      fts_tuple_t *tuple;
      fts_atom_t ret[1];

      tuple = (fts_tuple_t *)fts_object_create( fts_tuple_metaclass, ac, at);
      fts_object_refer( tuple);

      fts_set_tuple( ret, tuple);
      
      evalstack_return( &parser_eval_stack, ret);
    }
}

static void value_prefix_eval( int token, fts_atom_t *value, void *data)
{
  evalstack_push( &parser_eval_stack, value);
}

static void dollar_postfix_eval( int token, fts_atom_t *value, void *data)
{
  int ac = ((struct _eval_data_t *)data)->ac;
  const fts_atom_t *at = ((struct _eval_data_t *)data)->at;

  if (fts_is_int( value))
    {
      int index = fts_get_int( value);

      if ( index < ac)
	evalstack_push( &parser_eval_stack, at+index);
    }
  else if (fts_is_symbol( value))
    {
      fts_atom_t *p;
      fts_object_t *obj;

      obj = ((struct _eval_data_t *)data)->obj;

      if ((p = fts_variable_get_value( fts_object_get_patcher( obj), fts_get_symbol( value))))
	evalstack_push( &parser_eval_stack, p);
      else
	{
	  fts_atom_t a[1];

	  fts_set_void( a);
	  evalstack_push( &parser_eval_stack, a);
	}
    }
  else
    {
      fts_atom_t a[1];

      fts_set_void( a);
      evalstack_push( &parser_eval_stack, a);
    }
}

static void op_prefix_eval( int token, fts_atom_t *value, void *data)
{
  evalstack_push_frame( &parser_eval_stack);
}

#define UNOP_POSTFIX_EVAL(FUN,OP)						\
static void FUN( int token, fts_atom_t *value, void *data)			\
{										\
  fts_atom_t *p;								\
  fts_atom_t ret;								\
										\
  p = evalstack_base( &parser_eval_stack) + evalstack_top( &parser_eval_stack);	\
  if (fts_is_int( p))								\
    fts_set_int( &ret, OP fts_get_int( p));					\
  else if (fts_is_float( p))							\
    fts_set_float( &ret, OP fts_get_float( p));					\
  else										\
    fts_set_void( &ret);							\
										\
  evalstack_return( &parser_eval_stack, &ret);					\
}

#define IUNOP_POSTFIX_EVAL(FUN,OP)						\
static void FUN( int token, fts_atom_t *value, void *data)			\
{										\
  fts_atom_t *p;								\
  fts_atom_t ret;								\
										\
  p = evalstack_base( &parser_eval_stack) + evalstack_top( &parser_eval_stack);	\
  if (fts_is_int( p))								\
    fts_set_int( &ret, OP fts_get_int( p));					\
  else										\
    fts_set_void( &ret);							\
										\
  evalstack_return( &parser_eval_stack, &ret);					\
}

#define ABINOP_POSTFIX_EVAL(FUN,OP)						\
static void FUN( int token, fts_atom_t *value, void *data)			\
{										\
  fts_atom_t *p;								\
  fts_atom_t ret;								\
										\
  p = evalstack_base( &parser_eval_stack) + evalstack_top( &parser_eval_stack);	\
  if (fts_is_int( p-1) && fts_is_int( p))					\
    fts_set_int( &ret, fts_get_int( p-1) OP fts_get_int( p));			\
  else if (fts_is_int( p-1) && fts_is_float( p))				\
    fts_set_float( &ret, fts_get_int( p-1) OP fts_get_float( p));		\
  else if (fts_is_float( p-1) && fts_is_int( p))				\
    fts_set_float( &ret, fts_get_float( p-1) OP fts_get_int( p));		\
  else if (fts_is_float( p-1) && fts_is_float( p))				\
    fts_set_float( &ret, fts_get_float( p-1) OP fts_get_float( p));		\
  else										\
    fts_set_void( &ret);							\
										\
  evalstack_return( &parser_eval_stack, &ret);					\
}

#define IABINOP_POSTFIX_EVAL(FUN,OP)						\
static void FUN( int token, fts_atom_t *value, void *data)			\
{										\
  fts_atom_t *p;								\
  fts_atom_t ret;								\
										\
  p = evalstack_base( &parser_eval_stack) + evalstack_top( &parser_eval_stack);	\
  if (fts_is_int( p) && fts_is_int( p-1))					\
    fts_set_int( &ret, fts_get_int( p-1) OP fts_get_int( p) );			\
  else										\
    fts_set_void( &ret);							\
										\
  evalstack_return( &parser_eval_stack, &ret);					\
}

#define LBINOP_POSTFIX_EVAL(FUN,OP)								\
static void FUN( int token, fts_atom_t *value, void *data)					\
{												\
  fts_atom_t *p;										\
  fts_atom_t ret;										\
												\
  p = evalstack_base( &parser_eval_stack) + evalstack_top( &parser_eval_stack);			\
  if (fts_is_int( p) && fts_is_int( p-1))						       	\
    fts_set_int( &ret, fts_get_int( p-1) OP fts_get_int( p));					\
  else if (fts_is_int( p) && fts_is_float( p-1))						\
    fts_set_int( &ret, fts_get_int( p-1) OP fts_get_float( p));					\
  else if (fts_is_float( p) && fts_is_int( p-1))						\
    fts_set_int( &ret, fts_get_float( p-1) OP fts_get_int( p));					\
  else if (fts_is_float( p) && fts_is_float( p-1))						\
    fts_set_int( &ret, fts_get_float( p-1) OP fts_get_float( p));				\
  else												\
    fts_set_void( &ret);									\
												\
  evalstack_return( &parser_eval_stack, &ret);							\
}

static void power_postfix_eval( int token, fts_atom_t *value, void *data)
{
  fts_atom_t *p;
  fts_atom_t ret;

  p = evalstack_base( &parser_eval_stack) + evalstack_top( &parser_eval_stack);

  if (fts_is_int( p-1) && fts_is_int( p))
    fts_set_int( &ret, (int)pow( (double)fts_get_int( p-1), (double)fts_get_int( p)));
  else if (fts_is_float( p-1) && fts_is_int( p))
    fts_set_float( &ret, pow( fts_get_float( p-1), (double)fts_get_int( p)));
  else if (fts_is_int( p-1) && fts_is_float( p))
    fts_set_float( &ret, pow( (double)fts_get_int( p-1), fts_get_float( p)));
  else if (fts_is_float( p-1) && fts_is_float( p))
    fts_set_float( &ret, pow( fts_get_float( p-1), fts_get_float( p)));
  else
    fts_set_void( &ret);

  evalstack_return( &parser_eval_stack, &ret);
}

UNOP_POSTFIX_EVAL( uplus_postfix_eval,+)
UNOP_POSTFIX_EVAL( uminus_postfix_eval,+)
IUNOP_POSTFIX_EVAL( logical_not_postfix_eval,!)
ABINOP_POSTFIX_EVAL( plus_postfix_eval,+)
ABINOP_POSTFIX_EVAL( minus_postfix_eval,-)
ABINOP_POSTFIX_EVAL( times_postfix_eval,*)
ABINOP_POSTFIX_EVAL( div_postfix_eval,/)
IABINOP_POSTFIX_EVAL( percent_postfix_eval,%)
IABINOP_POSTFIX_EVAL( shift_left_postfix_eval,<<)
IABINOP_POSTFIX_EVAL( shift_right_postfix_eval,>>)
IABINOP_POSTFIX_EVAL( logical_and_postfix_eval,&&)
IABINOP_POSTFIX_EVAL( logical_or_postfix_eval,||)
LBINOP_POSTFIX_EVAL( equal_equal_postfix_eval,==)
LBINOP_POSTFIX_EVAL( not_equal_postfix_eval,!=)
LBINOP_POSTFIX_EVAL( greater_postfix_eval,>)
LBINOP_POSTFIX_EVAL( greater_equal_postfix_eval,>=)
LBINOP_POSTFIX_EVAL( smaller_postfix_eval,<)
LBINOP_POSTFIX_EVAL( smaller_equal_postfix_eval,<=)

static fts_parser_callback_table_t eval_callbacks = {
  /* semi             */   { 0, semi_infix_eval, semi_postfix_eval},
  /* tuple            */   { 0, 0, 0},
  /* c_int            */   { value_prefix_eval, 0, 0},
  /* c_float          */   { value_prefix_eval, 0, 0},
  /* symbol           */   { value_prefix_eval, 0, 0},
  /* par              */   { par_prefix_eval, 0, par_postfix_eval},
  /* cpar             */   { 0, 0, 0},
  /* sqpar            */   { 0, 0, 0},
  /* dollar           */   { 0, 0, dollar_postfix_eval},
  /* uplus            */   { op_prefix_eval, 0, uplus_postfix_eval},
  /* uminus           */   { op_prefix_eval, 0, uminus_postfix_eval},
  /* logical_not      */   { op_prefix_eval, 0, logical_not_postfix_eval},
  /* plus             */   { op_prefix_eval, 0, plus_postfix_eval},
  /* minus            */   { op_prefix_eval, 0, minus_postfix_eval},
  /* times            */   { op_prefix_eval, 0, times_postfix_eval},
  /* div              */   { op_prefix_eval, 0, div_postfix_eval},
  /* power            */   { op_prefix_eval, 0, power_postfix_eval},
  /* percent          */   { op_prefix_eval, 0, percent_postfix_eval},
  /* shift_left       */   { op_prefix_eval, 0, shift_left_postfix_eval},
  /* shift_right      */   { op_prefix_eval, 0, shift_right_postfix_eval},
  /* logical_and      */   { op_prefix_eval, 0, logical_and_postfix_eval},
  /* logical_or       */   { op_prefix_eval, 0, logical_or_postfix_eval},
  /* equal_equal      */   { op_prefix_eval, 0, equal_equal_postfix_eval},
  /* not_equal        */   { op_prefix_eval, 0, not_equal_postfix_eval},
  /* greater          */   { op_prefix_eval, 0, greater_postfix_eval},
  /* greater_equal    */   { op_prefix_eval, 0, greater_equal_postfix_eval},
  /* smaller          */   { op_prefix_eval, 0, smaller_postfix_eval},
  /* smaller_equal    */   { op_prefix_eval, 0, smaller_equal_postfix_eval}
};

void fts_parser_eval( fts_parser_t *parser, fts_object_t *obj, int ac, const fts_atom_t *at)
{
  struct _eval_data_t data;

  data.obj = obj;
  data.ac = ac;
  data.at = at;

  fts_parser_apply( parser, &eval_callbacks, &data);
}


/* **********************************************************************
 *
 * Parser API
 *
 */

void fts_parser_init( fts_parser_t *parser, int ac, const fts_atom_t *at)
{
  parser->ac = ac;
  parser->at = at;

  yyparse( parser);

  parser->ac = 0;
  parser->at = 0;
}

static void pnode_destroy( pnode_t *node)
{
  if (!node)
    return;

  pnode_destroy( node->left);
  pnode_destroy( node->right);
  fts_heap_free( node, pnode_heap);
}

void fts_parser_destroy( fts_parser_t *parser)
{
  pnode_destroy( parser->tree);
}

void fts_parser_apply( fts_parser_t *parser, fts_parser_callback_table_t *callbacks, void *data)
{
  pnode_walk( parser->tree, callbacks, data);
}

static void print_prefix_callback( int token, fts_atom_t *value, void *data)
{
  int *indentp = (int *)data;
  int i;

  fprintf( stderr, "%d:", *indentp);

  for ( i = 0; i < *indentp; i++)
    fprintf( stderr, "   ");

  switch( token) {
  case FTS_TOKEN_SEMI: fprintf( stderr, ";\n"); break;
  case FTS_TOKEN_TUPLE: fprintf( stderr, "TUPLE\n"); break;
  case FTS_TOKEN_INT: fprintf( stderr, "INT %d\n", fts_get_int( value)); break;
  case FTS_TOKEN_FLOAT: fprintf( stderr, "FLOAT %g\n", fts_get_float( value)); break;
  case FTS_TOKEN_SYMBOL: fprintf( stderr, "SYMBOL %s\n", fts_get_symbol( value)); break;
  case FTS_TOKEN_PAR: fprintf( stderr, "()\n"); break;
  case FTS_TOKEN_CPAR: fprintf( stderr, "{}\n"); break;
  case FTS_TOKEN_SQPAR: fprintf( stderr, "[]\n"); break;
  case FTS_TOKEN_DOLLAR: 
    if (fts_is_int( value))
      fprintf( stderr, "$%d\n", fts_get_int( value)); 
    else if (fts_is_symbol( value))
      fprintf( stderr, "$%s\n", fts_get_symbol( value)); 
    break;
  case FTS_TOKEN_UPLUS: fprintf( stderr, "+\n"); break;
  case FTS_TOKEN_UMINUS: fprintf( stderr, "-\n"); break;
  case FTS_TOKEN_LOGICAL_NOT: fprintf( stderr, "!\n"); break;
  case FTS_TOKEN_PLUS: fprintf( stderr, "+\n"); break;
  case FTS_TOKEN_MINUS: fprintf( stderr, "-\n"); break;
  case FTS_TOKEN_TIMES: fprintf( stderr, "*\n"); break;
  case FTS_TOKEN_DIV: fprintf( stderr, "/\n"); break;
  case FTS_TOKEN_POWER: fprintf( stderr, "**\n"); break;
  case FTS_TOKEN_PERCENT: fprintf( stderr, "%%\n"); break;
  case FTS_TOKEN_SHIFT_LEFT: fprintf( stderr, "<<\n"); break;
  case FTS_TOKEN_SHIFT_RIGHT: fprintf( stderr, ">>\n"); break;
  case FTS_TOKEN_LOGICAL_AND: fprintf( stderr, "&&\n"); break;
  case FTS_TOKEN_LOGICAL_OR: fprintf( stderr, "||\n"); break;
  case FTS_TOKEN_EQUAL_EQUAL: fprintf( stderr, "==\n"); break;
  case FTS_TOKEN_NOT_EQUAL: fprintf( stderr, "!=\n"); break;
  case FTS_TOKEN_GREATER: fprintf( stderr, ">\n"); break;
  case FTS_TOKEN_GREATER_EQUAL: fprintf( stderr, ">=\n"); break;
  case FTS_TOKEN_SMALLER: fprintf( stderr, "<\n"); break;
  case FTS_TOKEN_SMALLER_EQUAL: fprintf( stderr, "<=\n"); break;
  }

  *indentp += 1;
}

static void print_postfix_callback( int token, fts_atom_t *value, void *data)
{
  int *indentp = (int *)data;

  *indentp -= 1;
}

static fts_parser_callback_table_t print_callbacks = {
  /* semi             */   { print_prefix_callback, 0, print_postfix_callback},
  /* tuple            */   { print_prefix_callback, 0, print_postfix_callback},
  /* c_int            */   { print_prefix_callback, 0, print_postfix_callback},
  /* c_float          */   { print_prefix_callback, 0, print_postfix_callback},
  /* symbol           */   { print_prefix_callback, 0, print_postfix_callback},
  /* par              */   { print_prefix_callback, 0, print_postfix_callback},
  /* cpar             */   { print_prefix_callback, 0, print_postfix_callback},
  /* sqpar            */   { print_prefix_callback, 0, print_postfix_callback},
  /* dollar           */   { print_prefix_callback, 0, print_postfix_callback},
  /* uplus            */   { print_prefix_callback, 0, print_postfix_callback},
  /* uminus           */   { print_prefix_callback, 0, print_postfix_callback},
  /* logical_not      */   { print_prefix_callback, 0, print_postfix_callback},
  /* plus             */   { print_prefix_callback, 0, print_postfix_callback},
  /* minus            */   { print_prefix_callback, 0, print_postfix_callback},
  /* times            */   { print_prefix_callback, 0, print_postfix_callback},
  /* div              */   { print_prefix_callback, 0, print_postfix_callback},
  /* power            */   { print_prefix_callback, 0, print_postfix_callback},
  /* percent          */   { print_prefix_callback, 0, print_postfix_callback},
  /* shift_left       */   { print_prefix_callback, 0, print_postfix_callback},
  /* shift_right      */   { print_prefix_callback, 0, print_postfix_callback},
  /* logical_and      */   { print_prefix_callback, 0, print_postfix_callback},
  /* logical_or       */   { print_prefix_callback, 0, print_postfix_callback},
  /* equal_equal      */   { print_prefix_callback, 0, print_postfix_callback},
  /* not_equal        */   { print_prefix_callback, 0, print_postfix_callback},
  /* greater          */   { print_prefix_callback, 0, print_postfix_callback},
  /* greater_equal    */   { print_prefix_callback, 0, print_postfix_callback},
  /* smaller          */   { print_prefix_callback, 0, print_postfix_callback},
  /* smaller_equal    */   { print_prefix_callback, 0, print_postfix_callback}
};

void fts_parser_print( fts_parser_t *parser)
{
  int indent = 0;

  fts_parser_apply( parser, &print_callbacks, &indent);
}

/* **********************************************************************
 *
 * Kernel initialization
 *
 */

void fts_kernel_parser_init( void)
{
  fts_atom_t k, v;

  evalstack_init( &parser_eval_stack);

  pnode_heap = fts_heap_new( sizeof( pnode_t));

  fts_hashtable_init( &token_table, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_MEDIUM);

#define PUT(S,T) fts_set_symbol( &k, S); fts_set_int( &v, T); fts_hashtable_put( &token_table, &k, &v);
  PUT( fts_s_dollar, FTS_TOKEN_DOLLAR);
  PUT( fts_s_semi, FTS_TOKEN_SEMI);
  PUT( fts_s_plus, FTS_TOKEN_PLUS);
  PUT( fts_s_minus, FTS_TOKEN_MINUS);
  PUT( fts_s_times, FTS_TOKEN_TIMES);
  PUT( fts_s_div, FTS_TOKEN_DIV);
  PUT( fts_s_power, FTS_TOKEN_POWER);
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
