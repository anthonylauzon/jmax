/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */


/*
 * Currently the maximum number of variables (inlets) that are supported
 * is 10.
 */

#define	MAX_VARS	9
#define	MINODES		10 /* was 200 */

/* terminal defines */

/*
 * operations
 * (x<<16|y) x defines the level of precedence,
 * the lower the number the lower the precedence
 * separators are defines as operators just for convenience
 */

#define	OP_SEMI		((long)(1<<16|1))		/* ; */
#define	OP_COMMA	((long)(2<<16|2))		/* , */
#define	OP_LOR		((long)(3<<16|3))		/* || */
#define	OP_LAND		((long)(4<<16|4))		/* && */
#define	OP_OR		((long)(5<<16|5))		/* | */
#define	OP_XOR		((long)(6<<16|6))		/* ^ */
#define	OP_AND		((long)(7<<16|7))		/* & */
#define	OP_NE		((long)(8<<16|8))		/* != */
#define	OP_EQ		((long)(8<<16|9))		/* == */
#define	OP_GE		((long)(9<<16|10))		/* >= */
#define	OP_GT		((long)(9<<16|11))		/* > */
#define	OP_LE		((long)(9<<16|12))		/* <= */
#define	OP_LT		((long)(9<<16|13))		/* < */
#define	OP_SR		((long)(10<<16|14))		/* >> */
#define	OP_SL		((long)(10<<16|15))		/* << */
#define	OP_SUB		((long)(11<<16|16))		/* - */
#define	OP_ADD		((long)(11<<16|17))		/* + */
#define	OP_MOD		((long)(12<<16|18))		/* % */
#define	OP_DIV		((long)(12<<16|19))		/* / */
#define	OP_MUL		((long)(12<<16|20))		/* * */
#define	OP_UMINUS	((long)(13<<16|21))		/* - unary minus */
#define	OP_NEG		((long)(13<<16|22))		/* ~ one complement */
#define	OP_NOT		((long)(13<<16|23))		/* ! */
#define	OP_RB		((long)(14<<16|24))		/* ] */
#define	OP_LB		((long)(14<<16|25))		/* [ */
#define	OP_RP		((long)(14<<16|26))		/* ) */
#define	OP_LP		((long)(14<<16|27))		/* ( */
#define	HI_PRE		((long)(100<<16))	/* infinite precedence */
#define	PRE_MASK	((long)0xffff0000)	/* precedence level mask */

struct ex_ex;

typedef struct ex_funcs {
  char *f_name;					/* function name */
  void (*f_func)(long, struct ex_ex *, struct ex_ex *);				/* the real function performing the function (void, no return!!!) */
  long f_argc;					/* number of arguments */
} t_ex_func;

#define	name_ok(c)	(((c)=='_') || ((c)>='a' && (c)<='z') || \
			((c)>='A' && (c)<='Z') || ((c) >= '0' && (c) <= '9'))
#define unary_op(x)	((x) == OP_NOT || (x) == OP_NEG || (x) == OP_UMINUS)

struct ex_ex {
	union {
		long v_int;
		float v_flt;
		long op;
		char *ptr;
	} ex_cont;		/* content */
#define	ex_int		ex_cont.v_int
#define	ex_flt		ex_cont.v_flt
#define	ex_op		ex_cont.op
#define	ex_ptr		ex_cont.ptr
	long ex_type;		/* type of the node */
};
#define	exNULL	((struct ex_ex *)0)

/* defines for ex_type */
#define	ET_INT		0x1		/* an int */
#define	ET_FLT		0x2		/* a float */
#define ET_OP		0x3		/* operator */
#define	ET_STR		0x4		/* string */
#define ET_TBL		0x5		/* a table, the content is a pointer */
#define	ET_FUNC		0x6		/* a function */
#define	ET_SYM		0x7		/* symbol ("string") */
#define	ET_VSYM		0x8		/* variable symbol ("$s?") */
				/* we treat parenthesis and brackets */
				/* special to keep a pointer to their */
				/* match in the content */
#define	ET_LP		0x9		/* left parenthesis */
#define	ET_LB		0x10		/* left bracket */
#define	ET_II		0x11		/* and integer inlet */
#define	ET_FI		0x12		/* float inlet */
#define	ET_SI		0x13		/* string inlet */

/*
 * CHANGE we really should have a pointer to the ascii string that we
 *        as parsing so that we can manage the memory, and also we need 
 *	  another pointer to show the position of parsing.  In this way
 *	  we can run multiple expr parsing even if one of the expr
 *	  objects processes is preemted while parsing the string.  Of course
 *	  that will not be for the near future.
 */

typedef struct expr {
	fts_object_t exp_ob;
	void *exp_outlet;
	struct ex_ex *exp_stack;
	struct ex_ex exp_var[MAX_VARS];
	struct ex_ex exp_res;		/* the result of last evaluation */
} t_expr;






