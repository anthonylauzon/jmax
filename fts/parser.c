/* A Bison parser, made from parser.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

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
# define	TK_OPEN_SQPAR	267
# define	TK_CLOSED_SQPAR	268
# define	TK_COMMA	269
# define	TK_TUPLE	270
# define	TK_LOGICAL_OR	271
# define	TK_LOGICAL_AND	272
# define	TK_EQUAL_EQUAL	273
# define	TK_NOT_EQUAL	274
# define	TK_GREATER	275
# define	TK_GREATER_EQUAL	276
# define	TK_SMALLER	277
# define	TK_SMALLER_EQUAL	278
# define	TK_SHIFT_LEFT	279
# define	TK_SHIFT_RIGHT	280
# define	TK_PLUS	281
# define	TK_MINUS	282
# define	TK_TIMES	283
# define	TK_DIV	284
# define	TK_PERCENT	285
# define	TK_UMINUS	286
# define	TK_UPLUS	287
# define	TK_LOGICAL_NOT	288
# define	TK_POWER	289
# define	TK_SQPAR	290
# define	TK_DOT	291
# define	TK_DOLLAR	292
# define	TK_COLON	293

#line 23 "parser.y"

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

struct _parser_data {
  int ac;
  const fts_atom_t *at;
  fts_parsetree_t *tree;
} parser_data;

static fts_status_description_t syntax_error_status_description = {
  "Syntax error"
};
fts_status_t syntax_error_status = &syntax_error_status_description;


#line 56 "parser.y"
#ifndef YYSTYPE
typedef union {
  fts_atom_t a;
  fts_parsetree_t *n;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		88
#define	YYFLAG		-32768
#define	YYNTBASE	40

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 293 ? yytranslate[x] : 53)

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
      36,    37,    38,    39
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     2,     6,     8,     9,    12,    14,    16,    18,
      20,    22,    24,    26,    28,    30,    32,    36,    39,    42,
      45,    49,    53,    57,    61,    65,    69,    73,    77,    81,
      85,    89,    93,    97,   101,   105,   109,   111,   116,   118,
     121,   124,   128,   131,   133,   135,   137,   139,   141,   143,
     145,   147,   149,   151,   153,   155,   157,   159,   161,   163
};
static const short yyrhs[] =
{
      41,     0,    41,    15,    42,     0,    42,     0,     0,    42,
      43,     0,    43,     0,    44,     0,    45,     0,    46,     0,
      47,     0,    48,     0,    51,     0,     3,     0,     4,     0,
       5,     0,     8,    42,     9,     0,    27,    43,     0,    28,
      43,     0,    34,    43,     0,    43,    27,    43,     0,    43,
      28,    43,     0,    43,    29,    43,     0,    43,    30,    43,
       0,    43,    35,    43,     0,    43,    31,    43,     0,    43,
      25,    43,     0,    43,    26,    43,     0,    43,    18,    43,
       0,    43,    17,    43,     0,    43,    19,    43,     0,    43,
      20,    43,     0,    43,    21,    43,     0,    43,    22,    43,
       0,    43,    23,    43,     0,    43,    24,    43,     0,    49,
       0,    50,    13,    42,    14,     0,    50,     0,    38,     5,
       0,    38,     3,     0,     5,    39,    52,     0,    39,    52,
       0,     5,     0,    27,     0,    28,     0,    29,     0,    30,
       0,    31,     0,    25,     0,    26,     0,    21,     0,    22,
       0,    23,     0,    24,     0,    19,     0,    20,     0,    34,
       0,    17,     0,    18,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   127,   131,   133,   135,   139,   141,   144,   145,   146,
     147,   148,   149,   152,   154,   156,   160,   164,   166,   168,
     172,   174,   176,   178,   180,   182,   184,   186,   188,   190,
     192,   194,   196,   198,   200,   202,   206,   209,   211,   214,
     216,   220,   222,   226,   227,   229,   231,   233,   235,   237,
     239,   241,   243,   245,   247,   249,   251,   253,   255,   257
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "TK_INT", "TK_FLOAT", "TK_SYMBOL", "TK_SEMI", 
  "TK_PAR", "TK_OPEN_PAR", "TK_CLOSED_PAR", "TK_CPAR", "TK_OPEN_CPAR", 
  "TK_CLOSED_CPAR", "TK_OPEN_SQPAR", "TK_CLOSED_SQPAR", "TK_COMMA", 
  "TK_TUPLE", "TK_LOGICAL_OR", "TK_LOGICAL_AND", "TK_EQUAL_EQUAL", 
  "TK_NOT_EQUAL", "TK_GREATER", "TK_GREATER_EQUAL", "TK_SMALLER", 
  "TK_SMALLER_EQUAL", "TK_SHIFT_LEFT", "TK_SHIFT_RIGHT", "TK_PLUS", 
  "TK_MINUS", "TK_TIMES", "TK_DIV", "TK_PERCENT", "TK_UMINUS", "TK_UPLUS", 
  "TK_LOGICAL_NOT", "TK_POWER", "TK_SQPAR", "TK_DOT", "TK_DOLLAR", 
  "TK_COLON", "expression", "comma_tuple_list", "tuple", "term", 
  "primitive", "par", "unary", "binary", "reference", "array_reference", 
  "variable_reference", "class", "class_name", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    40,    41,    41,    41,    42,    42,    43,    43,    43,
      43,    43,    43,    44,    44,    44,    45,    46,    46,    46,
      47,    47,    47,    47,    47,    47,    47,    47,    47,    47,
      47,    47,    47,    47,    47,    47,    48,    49,    49,    50,
      50,    51,    51,    52,    52,    52,    52,    52,    52,    52,
      52,    52,    52,    52,    52,    52,    52,    52,    52,    52
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     1,     3,     1,     0,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     2,     2,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     1,     4,     1,     2,
       2,     3,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       4,    13,    14,    15,     0,     0,     0,     0,     0,     0,
       1,     3,     6,     7,     8,     9,    10,    11,    36,    38,
      12,     0,     0,    17,    18,    19,    40,    39,    43,    58,
      59,    55,    56,    51,    52,    53,    54,    49,    50,    44,
      45,    46,    47,    48,    57,    42,     0,     5,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    41,    16,     2,    29,    28,
      30,    31,    32,    33,    34,    35,    26,    27,    20,    21,
      22,    23,    25,    24,     0,    37,     0,     0,     0
};

static const short yydefgoto[] =
{
      86,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    45
};

static const short yypact[] =
{
      68,-32768,-32768,   -32,    68,    68,    68,    68,    30,    92,
       4,    68,   110,-32768,-32768,-32768,-32768,-32768,-32768,    13,
  -32768,    92,     0,     2,     2,     2,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,    68,   110,    68,    68,
      68,    68,    68,    68,    68,    68,    68,    68,    68,    68,
      68,    68,    68,    68,    68,-32768,-32768,    68,   128,   145,
     160,   160,   -15,   -15,   -15,   -15,    -6,    -6,     1,     1,
       2,     2,     2,-32768,    60,-32768,    40,    41,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,    14,    -5,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,    21
};


#define	YYLAST		195


static const short yytable[] =
{
      23,    24,    25,     1,     2,     3,    47,    21,     4,    66,
      56,    57,    58,    59,    60,    61,    62,    47,    22,    46,
      63,    58,    59,    60,    61,    62,    64,     5,     6,    63,
      60,    61,    62,    26,     7,    27,    63,    63,     8,     9,
      87,    88,    65,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,     0,
      67,     0,    47,     1,     2,     3,     0,     0,     4,     0,
       0,     1,     2,     3,    85,     0,     4,     0,    84,    47,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     0,
       0,     0,     0,     0,     7,     5,     6,    28,     8,     9,
       0,     0,     7,     0,     0,     0,     8,     9,     0,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,     0,     0,    44,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,     0,     0,     0,    63,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
       0,     0,     0,    63,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,     0,     0,     0,
      63,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,     0,     0,     0,    63
};

static const short yycheck[] =
{
       5,     6,     7,     3,     4,     5,    11,    39,     8,     9,
      25,    26,    27,    28,    29,    30,    31,    22,     4,    15,
      35,    27,    28,    29,    30,    31,    13,    27,    28,    35,
      29,    30,    31,     3,    34,     5,    35,    35,    38,    39,
       0,     0,    21,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    -1,
      46,    -1,    67,     3,     4,     5,    -1,    -1,     8,    -1,
      -1,     3,     4,     5,    14,    -1,     8,    -1,    64,    84,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    28,    -1,
      -1,    -1,    -1,    -1,    34,    27,    28,     5,    38,    39,
      -1,    -1,    34,    -1,    -1,    -1,    38,    39,    -1,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    -1,    -1,    34,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    -1,    -1,    -1,    35,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      -1,    -1,    -1,    35,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    -1,    -1,    -1,
      35,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    -1,    -1,    -1,    35
};
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
#line 128 "parser.y"
{ parser_data.tree = yyvsp[0].n; }
    break;
case 2:
#line 132 "parser.y"
{ yyval.n = fts_parsetree_new( TK_COMMA, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 3:
#line 134 "parser.y"
{ yyval.n = fts_parsetree_new( TK_COMMA, 0, 0, yyvsp[0].n); }
    break;
case 4:
#line 136 "parser.y"
{ yyval.n = 0; }
    break;
case 5:
#line 140 "parser.y"
{ yyval.n = fts_parsetree_new( TK_TUPLE, 0, yyvsp[-1].n, yyvsp[0].n); }
    break;
case 13:
#line 153 "parser.y"
{ yyval.n = fts_parsetree_new( TK_INT, &(yyvsp[0].a), 0, 0); }
    break;
case 14:
#line 155 "parser.y"
{ yyval.n = fts_parsetree_new( TK_FLOAT, &(yyvsp[0].a), 0, 0); }
    break;
case 15:
#line 157 "parser.y"
{ yyval.n = fts_parsetree_new( TK_SYMBOL, &(yyvsp[0].a), 0, 0); }
    break;
case 16:
#line 161 "parser.y"
{ yyval.n = fts_parsetree_new( TK_PAR, 0, 0, yyvsp[-1].n); }
    break;
case 17:
#line 165 "parser.y"
{ yyval.n = fts_parsetree_new( TK_UPLUS, 0, yyvsp[0].n, 0); }
    break;
case 18:
#line 167 "parser.y"
{ yyval.n = fts_parsetree_new( TK_UMINUS, 0, yyvsp[0].n, 0); }
    break;
case 19:
#line 169 "parser.y"
{ yyval.n = fts_parsetree_new( TK_LOGICAL_NOT, 0, yyvsp[0].n, 0); }
    break;
case 20:
#line 173 "parser.y"
{ yyval.n = fts_parsetree_new( TK_PLUS, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 21:
#line 175 "parser.y"
{ yyval.n = fts_parsetree_new( TK_MINUS, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 22:
#line 177 "parser.y"
{ yyval.n = fts_parsetree_new( TK_TIMES, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 23:
#line 179 "parser.y"
{ yyval.n = fts_parsetree_new( TK_DIV, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 24:
#line 181 "parser.y"
{ yyval.n = fts_parsetree_new( TK_POWER, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 25:
#line 183 "parser.y"
{ yyval.n = fts_parsetree_new( TK_PERCENT, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 26:
#line 185 "parser.y"
{ yyval.n = fts_parsetree_new( TK_SHIFT_LEFT, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 27:
#line 187 "parser.y"
{ yyval.n = fts_parsetree_new( TK_SHIFT_RIGHT, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 28:
#line 189 "parser.y"
{ yyval.n = fts_parsetree_new( TK_LOGICAL_AND, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 29:
#line 191 "parser.y"
{ yyval.n = fts_parsetree_new( TK_LOGICAL_OR, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 30:
#line 193 "parser.y"
{ yyval.n = fts_parsetree_new( TK_EQUAL_EQUAL, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 31:
#line 195 "parser.y"
{ yyval.n = fts_parsetree_new( TK_NOT_EQUAL, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 32:
#line 197 "parser.y"
{ yyval.n = fts_parsetree_new( TK_GREATER, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 33:
#line 199 "parser.y"
{ yyval.n = fts_parsetree_new( TK_GREATER_EQUAL, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 34:
#line 201 "parser.y"
{ yyval.n = fts_parsetree_new( TK_SMALLER, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 35:
#line 203 "parser.y"
{ yyval.n = fts_parsetree_new( TK_SMALLER_EQUAL, 0, yyvsp[-2].n, yyvsp[0].n); }
    break;
case 37:
#line 210 "parser.y"
{ yyval.n = fts_parsetree_new( TK_SQPAR, 0, yyvsp[-3].n, yyvsp[-1].n); }
    break;
case 39:
#line 215 "parser.y"
{ yyval.n = fts_parsetree_new( TK_DOLLAR, &(yyvsp[0].a), 0, 0); }
    break;
case 40:
#line 217 "parser.y"
{ yyval.n = fts_parsetree_new( TK_DOLLAR, &(yyvsp[0].a), 0, 0); }
    break;
case 41:
#line 221 "parser.y"
{ yyval.n = fts_parsetree_new( TK_COLON, &(yyvsp[0].a), fts_parsetree_new( TK_SYMBOL, &(yyvsp[-2].a), 0, 0), 0); }
    break;
case 42:
#line 223 "parser.y"
{ yyval.n = fts_parsetree_new( TK_COLON, &(yyvsp[0].a), 0, 0); }
    break;
case 44:
#line 228 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_plus); }
    break;
case 45:
#line 230 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_minus); }
    break;
case 46:
#line 232 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_times); }
    break;
case 47:
#line 234 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_div); }
    break;
case 48:
#line 236 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_percent); }
    break;
case 49:
#line 238 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_shift_left); }
    break;
case 50:
#line 240 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_shift_right); }
    break;
case 51:
#line 242 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_greater); }
    break;
case 52:
#line 244 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_greater_equal); }
    break;
case 53:
#line 246 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_smaller); }
    break;
case 54:
#line 248 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_smaller_equal); }
    break;
case 55:
#line 250 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_equal_equal); }
    break;
case 56:
#line 252 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_not_equal); }
    break;
case 57:
#line 254 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_logical_not); }
    break;
case 58:
#line 256 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_logical_or); }
    break;
case 59:
#line 258 "parser.y"
{ fts_set_symbol( &(yyval.a), fts_s_logical_and); }
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
#line 261 "parser.y"


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

  if (!tree)
    return;

  fprintf( stderr, "%d:", indent);

  for ( i = 0; i < indent; i++)
    fprintf( stderr, "   ");

  switch( tree->token) {
  case TK_COLON: fprintf( stderr, ": %s\n", fts_get_symbol( &tree->value)); break;
  case TK_COMMA: fprintf( stderr, ",\n"); break;
  case TK_TUPLE: fprintf( stderr, "TUPLE\n"); break;
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
}

#endif
