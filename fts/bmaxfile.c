/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <ftsconfig.h>

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <fts/fts.h>
#include <ftsprivate/bmaxfile.h>
#include <ftsprivate/connection.h>
#include <ftsprivate/patcher.h>
#include <ftsprivate/object.h>
#include <ftsprivate/selection.h>
#include <ftsprivate/template.h>


/*
 * endianism stuff
 */

union swap_union_t {
  unsigned long l;
  char c[4];
};

static int 
has_to_swap(void)
{
  union swap_union_t u;

  u.l = 0x11223344;
  return u.c[0] != 0x11;
}

static void 
swap_long( unsigned long *p)
{
  union swap_union_t *pu;
  char tmp;

  pu = (union swap_union_t *)p;
  tmp = pu->c[0];
  pu->c[0] = pu->c[3];
  pu->c[3] = tmp;
  tmp = pu->c[1];
  pu->c[1] = pu->c[2];
  pu->c[2] = tmp;
}

/* **********************************************************************
 *
 * bmax file loader virtual machine
 *
 */

/*
  The virtual machine contains:

   1- An evaluation stack
   2- An object stack
   3- A local symbol table.
   4- A local object table stack.
   5- A local object table.

   Format and Instructions:

   All the instructions are a byte
   and may be followed by one or more arguments;
   argument size is always 4 bytes (MSB first) for floats,
   and is 1, 2 or 4 bytes long for ints and idx.
   Some instructions have specific rules.

   Each opcode can be followed by _B (1 byte argument), _S (short
   argument, 2 bytes) or _L (long argument, 4 bytes).
   The execution engine is anyway based on atoms and words, i.e. all ints are 4 bytes
   in the registers.

   The instructions are:

   PUSH_INT   <int>   -- push an int value in the argument stack
   PUSH_FLOAT <float> -- push a float value in the argument stack
   PUSH_SYM   <int>   -- push a symbol from the file symbol table (by idx) value in the argument stack

   SET_INT   <int>       -- set an int value as the top of the argument stack
   SET_FLOAT   <float>   -- set an float value as the top of the argument stack
   SET_SYM   <int>   -- set a symbol from the file symbol table (by idx) value as the top of the argument stack

   POP_ARGS    <int>   -- pop n values  from the argument stack

   PUSH_OBJ   <int>   -- Push an object table entry in the object stack
   MV_OBJ     <objidx>    -- Copy the top of object stack to an object table entry
   POP_OBJS    <int>   -- Push n objects from the object stack
   

   MAKE_OBJ   <nargs>   -- Take <nargs> argument from the evaluation stack
                       -- and make an object with top of the object stack as parent.
		       -- Push the new object in the object stack
		       -- Don't touch the argument stack.

   MAKE_TOP_OBJ   <nargs>   -- Take <nargs> + lambda argument from the evaluation stack
                       -- and make an object with top of the object stack as parent.
		       -- Push the new object in the object stack
		       -- Don't touch the argument stack.
		       -- lambda is the value of a special internal vm register
		       -- that is filled loading templates; it is used to
		       -- add arguments to the first object created from the vm invocation
		       -- environment

   PUT_PROP   <sym> -- Put the top of the evaluation stack value
                    -- as property <sym> for the object at the top 
		    -- of the object stack
		    -- don't touch either stack.

   OBJ_MESS   <inlet> <sel> <nargs>  -- Send a message <sel> to the inlet <in> object top of the object stack
                         -- with <nargs> arguments from the evaluation stack
			 -- don't touch the stacks
			 -- ONLY LONG ARGUMENTS HERE (rare opcode, no optimization)


   PUSH_OBJ_TABLE <int> -- Push an object table of N values
   POP_OBJ_TABLE        -- Pop an object table

   CONNECT            -- connect outlet (Top of evaluation stack) of object (top of obejct stack)
                      -- with inlet (Top - 1 of evaluation stack) of object (top - 1 of obejct stack)

   RETURN            -- Return from the current VM execution
                     -- the current top of the object stack is returned
		     -- by the C function; the stack is popped by one.
   */

/* #define  VM_DEBUG */

#ifdef DEBUG
#define VM_SAFE
#endif

#define EVAL_STACK_DEPTH   8192
#define OBJECT_STACK_DEPTH 8192
#define OBJECT_TABLE_STACK_DEPTH 8192

static fts_atom_t eval_stack[EVAL_STACK_DEPTH];
static int        eval_tos = EVAL_STACK_DEPTH;

static fts_object_t *object_stack[OBJECT_STACK_DEPTH];
static int           object_tos = OBJECT_STACK_DEPTH;

static fts_object_t **object_table_stack[OBJECT_TABLE_STACK_DEPTH];
static int            object_table_size_stack[OBJECT_TABLE_STACK_DEPTH];
static int            object_table_tos = OBJECT_TABLE_STACK_DEPTH;
static fts_object_t **object_table = 0;


/* Macros to do checks operations */

#ifdef  VM_SAFE
#define CHECK_OBJ_STACK   \
      { \
	if (object_tos < 0) \
	 post("While loading: Object Stack overflow\n"); \
      else if (object_tos > OBJECT_STACK_DEPTH) \
         post("While Loading: Object Stack underflow\n"); \
      }

#define CHECK_EVAL_STACK   \
      { \
	if (eval_tos <  0) \
         post("While loading: Eval Stack overflow\n"); \
      else if (eval_tos > OBJECT_STACK_DEPTH) \
         post("While loading: Eval Stack underflow\n"); \
      }

#else

#define CHECK_OBJ_STACK     {}
#define CHECK_EVAL_STACK    {}

#endif


/* The parent argument is pushed to the object stack, so to be used
   as parent by the make operations; at the end, return the top of the
   object stack.

   */

#define GET_B(p)    ((signed char) p[0])
#define GET_S(p)    ((short) (((unsigned int) p[0]) << 8 | ((unsigned int) p[1])))
#define GET_L(p)    ((int) (((unsigned int) p[0]) << 24 | ((unsigned int) p[1]) << 16 | ((unsigned int) p[2]) << 8 | p[3]))


static float GET_F(unsigned char *p)
{
  unsigned int fx;
  float f;

  fx = GET_L(p);
  f =  *((float *)&fx);

  return f;
}

static void fts_object_push_assignement(fts_symbol_t name, fts_atom_t *value, void *data)
{
  fts_object_t *obj = (fts_object_t *)data;
  fts_atom_t a;

  eval_tos--;
  eval_stack[eval_tos] = *value;

  fts_set_symbol(&a, fts_s_equal);
  eval_tos--;
  eval_stack[eval_tos] = a;

  fts_set_symbol(&a, name);
  eval_tos--;
  eval_stack[eval_tos] = a;
}

static fts_object_t *fts_run_mess_vm( fts_object_t *parent, unsigned char *program, fts_symbol_t symbol_table[], int ac, const fts_atom_t *at)
{
  int i;
  unsigned char cmd;
  unsigned char *p;

  /* setting arguments and parent */

  object_tos--;
  object_stack[object_tos] = parent;

  for (i = ac - 1; i >= 0; i--)
    {
      /* Push the atoms in the value stack */

      eval_tos--;
      eval_stack[eval_tos] = at[i];
    }

  /* Do the evaluation */

  p = program;

  while (1)
    {
      cmd = *(p++);

      switch (cmd)
	{
	  /* Eval stack manipulation */

	case FVM_PUSH_INT_B:
	  {
	    /* PUSH_INT_B   <int> */

#ifdef VM_DEBUG
	    fts_log( "PUSH_INT_B %d\n", GET_B(p));
#endif

	    eval_tos--;
	    fts_set_int(&eval_stack[eval_tos], GET_B(p));
	    p++;
	  }
	break;

	case FVM_PUSH_INT_S:
	  {
	    /* PUSH_INT_S   <int> */

#ifdef VM_DEBUG
	    fts_log( "PUSH_INT_S %d\n", GET_S(p));
#endif

	    eval_tos--;
	    fts_set_int(&eval_stack[eval_tos], GET_S(p));
	    p += 2;
	  }
	break;

	case FVM_PUSH_INT_L:
	  {
	    /* PUSH_INT_L   <int> */

#ifdef VM_DEBUG
	    fts_log( "PUSH_INT_L %d\n", GET_L(p));
#endif

	    eval_tos--;
	    fts_set_int(&eval_stack[eval_tos], GET_L(p));
	    p += 4;
	  }
	break;

	case FVM_PUSH_FLOAT:
	  {
	    /* PUSH_FLOAT <float> */

#ifdef VM_DEBUG
	    fts_log( "PUSH_FLOAT %f\n", GET_F(p));
#endif
	    
	    eval_tos--;
	    fts_set_float(&eval_stack[eval_tos], GET_F(p));
	    p += 4;
	  }
	break;

	case FVM_PUSH_SYM_B:
	  {
	    /* PUSH_SYM_B   <idx> */

	    fts_symbol_t s;

	    s  = symbol_table[GET_B(p)];

#ifdef VM_DEBUG
	    fts_log( "PUSH_SYM_B %d (%s)\n", GET_B(p), s);
#endif

	    eval_tos--;
	    fts_set_symbol(&eval_stack[eval_tos], s);
	    p++;
	  }
	break;

	case FVM_PUSH_SYM_S:
	  {
	    /* PUSH_SYM_S   <idx> */

	    fts_symbol_t s;

	    s  = symbol_table[GET_S(p)];

#ifdef VM_DEBUG
	    fts_log( "PUSH_SYM_S %s\n", s);
#endif

	    eval_tos--;
	    fts_set_symbol(&eval_stack[eval_tos], s);
	    p += 2;
	  }
	break;

	case FVM_PUSH_SYM_L:
	  {
	    /* PUSH_SYM_L   <idx> */

	    fts_symbol_t s;

	    s  = symbol_table[GET_L(p)];

#ifdef VM_DEBUG
	    fts_log( "PUSH_SYM_L %s\n", s);
#endif

	    eval_tos--;
	    fts_set_symbol(&eval_stack[eval_tos], s);
	    p += 4;
	  }
	break;

	case FVM_SET_INT_B:
	  {
	    /* SET_INT_B   <int> */

#ifdef VM_DEBUG
	    fts_log( "SET_INT_B %d\n", GET_B(p));
#endif

	    fts_set_int(&eval_stack[eval_tos], GET_B(p));
	    p++;
	  }
	break;

	case FVM_SET_INT_S:
	  {
	    /* SET_INT_S   <int> */

#ifdef VM_DEBUG
	    fts_log( "SET_INT_S %d\n", GET_S(p));
#endif

	    fts_set_int(&eval_stack[eval_tos], GET_S(p));
	    p += 2;
	  }
	break;

	case FVM_SET_INT_L:
	  {
	    /* SET_INT_L   <int> */

#ifdef VM_DEBUG
	    fts_log( "SET_INT_L %d\n", GET_L(p));
#endif

	    fts_set_int(&eval_stack[eval_tos], GET_L(p));
	    p += 4;
	  }
	break;

	case FVM_SET_FLOAT:
	  {
	    /* SET_FLOAT <float> */

#ifdef VM_DEBUG
	    fts_log( "SET_FLOAT %f\n", GET_F(p));
#endif
	    
	    fts_set_float(&eval_stack[eval_tos], GET_F(p));
	    p += 4;
	  }
	break;

	case FVM_SET_SYM_B:
	  {
	    /* SET_SYM_B   <idx> */

#ifdef VM_DEBUG
	    fts_log( "SET_SYM_B %d\n", GET_B(p));
#endif

	    fts_set_symbol(&eval_stack[eval_tos], symbol_table[GET_B(p)]);
	    p++;
	  }
	break;

	case FVM_SET_SYM_S:
	  {
	    /* SET_SYM_S   <idx> */

#ifdef VM_DEBUG
	    fts_log( "SET_SYM_S %d\n", GET_S(p));
#endif

	    fts_set_symbol(&eval_stack[eval_tos], symbol_table[GET_S(p)]);
	    p += 2;
	  }
	break;

	case FVM_SET_SYM_L:
	  {
	    /* SET_SYM_L   <idx> */

#ifdef VM_DEBUG
	    fts_log( "SET_SYM_L %d\n", GET_L(p));
#endif

	    fts_set_symbol(&eval_stack[eval_tos], symbol_table[GET_L(p)]);
	    p += 4;
	  }
	break;

	case FVM_POP_ARGS_B:
	  {
	    /* POP_ARGS_B    <int> */

#ifdef VM_DEBUG
	    fts_log( "POP_ARGS_B %d\n", GET_B(p));
#endif

	    eval_tos += GET_B(p);
	    p++;
	  }
	break;

	case FVM_POP_ARGS_S:
	  {
	    /* POP_ARGS_S    <int> */

#ifdef VM_DEBUG
	    fts_log( "POP_ARGS_S %d\n", GET_S(p));
#endif

	    eval_tos += GET_S(p);
	    p += 2;
	  }
	break;

	case FVM_POP_ARGS_L:
	  {
	    /* POP_ARGS_L    <int> */

#ifdef VM_DEBUG
	    fts_log( "POP_ARGS_L %d\n", GET_L(p));
#endif

	    eval_tos += GET_L(p);
	    p += 4;
	  }
	break;


	/* Object stack manipulation */

	case FVM_PUSH_OBJ_B:
	  {
	    /* PUSH_OBJ_B   <objidx> */

#ifdef VM_DEBUG
	    fts_log( "PUSH_OBJ_B %d\n", GET_B(p));
#endif

	    object_tos--;
	    object_stack[object_tos] = object_table[GET_B(p)];
	    p += 1;
	  }
	break;

	case FVM_PUSH_OBJ_S:
	  {
	    /* PUSH_OBJ_S   <objidx> */

#ifdef VM_DEBUG
	    fts_log( "PUSH_OBJ_S %d\n", GET_S(p));
#endif

	    object_tos--;
	    object_stack[object_tos] = object_table[GET_S(p)];

	    p += 2;
	  }
	break;

	case FVM_PUSH_OBJ_L:
	  {
	    /* PUSH_OBJ_L   <objidx> */

#ifdef VM_DEBUG
	    fts_log( "PUSH_OBJ_L %d\n", GET_L(p));
#endif

	    object_tos--;
	    object_stack[object_tos] = object_table[GET_L(p)];

	    p += 4;
	  }
	break;

	case FVM_MV_OBJ_B:
	  {
	    /* MV_OBJ_B     <objidx> */

#ifdef VM_DEBUG
	    fts_log( "MV_OBJ_B %d\n", GET_B(p));
#endif

	    object_table[GET_B(p)] = object_stack[object_tos];
	    p += 1;
	  }
	break;

	case FVM_MV_OBJ_S:
	  {
	    /* MV_OBJ_S     <objidx> */

#ifdef VM_DEBUG
	    fts_log( "MV_OBJ_S %d\n", GET_S(p));
#endif

	    object_table[GET_S(p)] = object_stack[object_tos];
	    p += 2;
	  }
	break;

	case FVM_MV_OBJ_L:
	  {
	    /* MV_OBJ_L     <objidx> */

#ifdef VM_DEBUG
	    fts_log( "MV_OBJ_L %d\n", GET_L(p));
#endif

	    object_table[GET_L(p)] = object_stack[object_tos];
	    p += 4;
	  }
	break;


	case FVM_POP_OBJS_B:
	  {
	    /* POP_OBJS_B    <int> */

#ifdef VM_DEBUG
	    fts_log( "POP_OBJ_B %d\n", GET_B(p));
#endif

	    object_tos += GET_B(p);
	    p += 1;
	  }
	break;

	case FVM_POP_OBJS_S:
	  {
	    /* POP_OBJS_S    <int> */

#ifdef VM_DEBUG
	    fts_log( "POP_OBJ_S %d\n", GET_S(p));
#endif

	    object_tos += GET_S(p);
	    p += 2;
	  }
	break;


	case FVM_POP_OBJS_L:
	  {
	    /* POP_OBJS_L    <int> */

#ifdef VM_DEBUG
	    fts_log( "POP_OBJ_L %d\n", GET_L(p));
#endif

	    object_tos += GET_L(p);
	    p += 4;
	  }
	break;

	/* Object Manipulatation */

	case FVM_MAKE_OBJ_B:
	  {
	    /* MAKE_OBJ_B   <nargs> */

	    fts_object_t *new;
	    int nargs = GET_B(p);

#ifdef VM_DEBUG
	    fts_log( "MAKE_OBJ_B %d\n", nargs);
#endif

	    new  = fts_eval_object_description((fts_patcher_t *) object_stack[object_tos], nargs, &eval_stack[eval_tos]);

#ifdef VM_DEBUG
	    fts_log( "\t");
	    fprintf_object(stderr, new);
	    fts_log( "\n");
#endif

	    /* Push the object in the object stack */

	    object_tos--;
	    object_stack[object_tos] = new;
	    p += 1;
	  }
	  break;	

	case FVM_MAKE_OBJ_S:
	  {
	    /* MAKE_OBJ_S   <nargs> */

	    fts_object_t *new;
	    int nargs = GET_S(p);

#ifdef VM_DEBUG
	    fts_log( "MAKE_OBJ_S %d\n", nargs);
#endif

	    new  = fts_eval_object_description((fts_patcher_t *) object_stack[object_tos], nargs, &eval_stack[eval_tos]);

	    /* Push the object in the object stack */

	    object_tos--;
	    object_stack[object_tos] = new;
	    p += 2;
	  }
	  break;	


	case FVM_MAKE_OBJ_L:
	  {
	    /* MAKE_OBJ_L   <nargs> */

	    fts_object_t *new;
	    int nargs = GET_L(p);

#ifdef VM_DEBUG
	    fts_log( "MAKE_OBJ_L %d\n", nargs);
#endif

	    new  = fts_eval_object_description((fts_patcher_t *) object_stack[object_tos], nargs, &eval_stack[eval_tos]);

	    /* Push the object in the object stack */

	    object_tos--;
	    object_stack[object_tos] = new;
	    p += 4;
	  }
	  break;	

	case FVM_MAKE_TOP_OBJ_B:
	  {
	    /* MAKE_TOP_OBJ_B   <nargs> */

	    fts_object_t *new;
	    int nargs = GET_B(p);

#ifdef VM_DEBUG
	    fts_log( "MAKE_TOP_OBJ_B %d\n", nargs);
#endif

	    new  = fts_eval_object_description((fts_patcher_t *) object_stack[object_tos], nargs + ac,
				  &eval_stack[eval_tos]);

	    /* Push the object in the object stack */

	    object_tos--;
	    object_stack[object_tos] = new;
	    p += 1;
	  }
	  break;	

	case FVM_MAKE_TOP_OBJ_S:
	  {
	    /* MAKE_TOP_OBJ_S   <nargs> */

	    fts_object_t *new;
	    int nargs = GET_S(p);

#ifdef VM_DEBUG
	    fts_log( "MAKE_TOP_OBJ_S %d\n", nargs);
#endif

	    new  = fts_eval_object_description((fts_patcher_t *) object_stack[object_tos], nargs + ac,
				  &eval_stack[eval_tos]);

	    /* Push the object in the object stack */

	    object_tos--;
	    object_stack[object_tos] = new;
	    p += 2;
	  }
	  break;	


	case FVM_MAKE_TOP_OBJ_L:
	  {
	    /* MAKE_TOP_OBJ_L   <nargs> */

	    fts_object_t *new;
	    int nargs = GET_L(p);

#ifdef VM_DEBUG
	    fts_log( "MAKE_OBJ_L %d\n", nargs);
#endif
	    new  = fts_eval_object_description((fts_patcher_t *) object_stack[object_tos], nargs + ac,
				  &eval_stack[eval_tos]);

	    /* Push the object in the object stack */

	    object_tos--;
	    object_stack[object_tos] = new;
	    p += 4;
	  }
	  break;	

	case FVM_PUT_PROP_B:
	  {
	    /* PUT_PROP_B_   <sym> */

	    fts_symbol_t prop;

	    prop = symbol_table[GET_B(p)];

#ifdef VM_DEBUG
	    fts_log( "PUT_PROP_B %s\n", prop);
#endif

	    fts_object_put_prop(object_stack[object_tos], prop, &eval_stack[eval_tos]);
	    p += 1;
	  }
	break;

	case FVM_PUT_PROP_S:
	  {
	    /* PUT_PROP_S_   <sym> */

	    fts_symbol_t prop;

	    prop = symbol_table[GET_S(p)];

#ifdef VM_DEBUG
	    fts_log( "PUT_PROP_S %s\n", prop);
#endif
	    fts_object_put_prop(object_stack[object_tos], prop, &eval_stack[eval_tos]);
	    p += 2;
	  }
	break;

	case FVM_PUT_PROP_L:
	  {
	    /* PUT_PROP_L_   <sym> */

	    fts_symbol_t prop;

	    prop = symbol_table[GET_L(p)];

#ifdef VM_DEBUG
	    fts_log( "PUT_PROP_S %s\n", prop);
#endif

	    fts_object_put_prop(object_stack[object_tos], prop, &eval_stack[eval_tos]);
	    p += 4;
	  }
	break;

	case FVM_OBJ_MESS:
	  {
	    /* OBJ_MESS   <inlet> <sel> <nargs> */

	    int inlet;
	    fts_symbol_t sel;
	    int nargs;

	    inlet = GET_L(p);
	    p += 4;
	    sel = symbol_table[GET_L(p)];
	    p += 4;
	    nargs = GET_L(p);
	    p += 4;

#ifdef VM_DEBUG
	    fts_log( "OBJ_MESS %d %s %d\n", inlet, sel, nargs);
#endif

	    fts_send_message(object_stack[object_tos], inlet, sel, nargs, &eval_stack[eval_tos]);
	  }
	break;

	case FVM_CONNECT:
	  {
	    /* CONNECT */

#ifdef VM_DEBUG
	    fts_log( "CONNECT\n");
#endif
	    fts_connection_new(object_stack[object_tos], fts_get_int(&eval_stack[eval_tos]),
			       object_stack[object_tos + 1], fts_get_int(&eval_stack[eval_tos + 1]), fts_c_anything);
	  }
	break;

	/* Object table stack */

	case FVM_PUSH_OBJ_TABLE_B:
	  {
	    /* PUSH_OBJ_TABLE_B <int> */

	    int size;
#ifdef VM_DEBUG
	    fts_log( "PUSH_OBJ_TABLE_B %d\n", GET_B(p));
#endif

	    size = GET_B(p);
	    p += 1;

	    if (size == 0)
	      object_table = 0;
	    else
	      object_table = fts_malloc(sizeof(fts_object_t *) * size);

	    object_table_tos--;
	    object_table_stack[object_table_tos] = object_table;
	    object_table_size_stack[object_table_tos] = size;
	  }
	break;

	case FVM_PUSH_OBJ_TABLE_S:
	  {
	    /* PUSH_OBJ_TABLE_S <int> */

	    int size;
#ifdef VM_DEBUG
	    fts_log( "PUSH_OBJ_TABLE_S %d\n", GET_S(p));
#endif

	    size = GET_S(p);
	    p += 2;

	    object_table = fts_malloc(sizeof(fts_object_t *) * size);

	    object_table_tos--;
	    object_table_stack[object_table_tos] = object_table;
	    object_table_size_stack[object_table_tos] = size;
	  }
	break;



	case FVM_PUSH_OBJ_TABLE_L:
	  {
	    /* PUSH_OBJ_TABLE_L <int> */

	    int size;
#ifdef VM_DEBUG
	    fts_log( "PUSH_OBJ_TABLE_L %d\n", GET_L(p));
#endif

	    size = GET_L(p);
	    p += 1;

	    object_table = fts_malloc(sizeof(fts_object_t *) * size);

	    object_table_tos--;
	    object_table_stack[object_table_tos] = object_table;
	    object_table_size_stack[object_table_tos] = size;
	  }
	break;

	case FVM_POP_OBJ_TABLE:
	  {
	    /* POP_OBJ_TABLE */

#ifdef VM_DEBUG
	    fts_log( "POP_OBJ_TABLE\n");
#endif

	    if (object_table)
	      fts_free(object_table);

	    object_table_tos++;
	    object_table = object_table_stack[object_table_tos];
	  }
	break;

	case FVM_RETURN:
	  {
	    fts_object_t *obj = 0;

	    /* RETURN */

#ifdef VM_DEBUG
	    fts_log( "RETURN\n");
#endif

	    /* Rewind the template/patcher argument */

	    eval_tos += ac;

	    /* This test is usefull only on copy/paste */

	    if (object_tos < OBJECT_STACK_DEPTH - 1)
	      obj = object_stack[object_tos++];

	    object_tos++;	/* take the parent away from the object stack */

	    return obj;
	  }
	}

      CHECK_OBJ_STACK;
      CHECK_EVAL_STACK;
    }
}

/* Support for dynamic object redefinition:
   Since the variable dependency tracking system can 
   re-instantiate objects, we may end up with pointers
   to invalid objects in the vm status; for this reason,
   this function will look for the old and new object
   in the whole vm state and substitute it.
   Probabily, there is a more efficent way to do it,
   but the vm state dimention is proportional to the depth
   of the patch, not to the number of objects, so it is never
   enormous.

   Note also that the vm state is changed across vm invocations,
   i.e. across patchers and templates/abstractions, because dependecy
   are across file barriers.
   */

void fts_vm_substitute_object(fts_object_t *old, fts_object_t *new)
{
  int i;

  /* First, the object stack */

  for (i = object_tos ; i < OBJECT_STACK_DEPTH; i++)
    if (object_stack[i] == old)
      object_stack[i] = new;

  /* Second, the object table stack */

  for (i = object_table_tos ; i < OBJECT_TABLE_STACK_DEPTH; i++)
    {
      int j;
      fts_object_t **table;

      table = object_table_stack[i];

      for (j = 0; j < object_table_size_stack[i]; j++)
	if (table[j] == old)
	  table[j] = new;
    }
}


/* **********************************************************************
 * 
 * bmax file loader
 *
 */

/* Converted by MDC to use FILE * instead of file,
   in order to integrate it better with the loader;
   in particular, the clipboard will is now an unlinked, invisible
   file, never closed until exit, with fseek/fread/fwrite
   operations on it (to avoid leaving .clipboard file arounds)
   */

/* Private structure */

typedef struct fts_binary_file_desc_t
{
  unsigned char *code;
  fts_symbol_t *symbols;
} fts_binary_file_desc_t;

static int fts_binary_file_map( FILE *f, fts_binary_file_desc_t *desc)
{
  fts_binary_file_header_t header;

  /* read the header */

  if (fread( &header, sizeof( header), 1, f) < 1)
    {
      return -1;
    }

  if (has_to_swap())
    {
      swap_long( &header.magic_number);
      swap_long( &header.code_size);
      swap_long( &header.n_symbols);
    }

  if (header.magic_number != FTS_BINARY_FILE_MAGIC)
    return -1;

  /* allocate code */

  desc->code = (unsigned char *)fts_malloc( header.code_size);
  if (!desc->code)
    return -1;

  /* read the code */

  if (fread( desc->code, sizeof(char), header.code_size, f) < header.code_size)
    {
      fts_free( desc->code);
      return -1;
    }

  /* allocate code and read symbols */

  if (header.n_symbols > 0)
    {
      int i = 0;
      char buf[1024]; /* max  symbol size */
      unsigned int symbolIndex = 0;

      desc->symbols = (fts_symbol_t *)fts_malloc( header.n_symbols * sizeof( fts_symbol_t));

      /* In case of corrupted file, we initialize the
	 table with the error symbol, so to have some hope
	 of opening the result */

      for (symbolIndex = 0; symbolIndex < header.n_symbols; symbolIndex++)
	desc->symbols[symbolIndex] = fts_s_error;

      symbolIndex = 0;
      while (! feof(f) && (symbolIndex < header.n_symbols))
	{
	  buf[i] = getc(f);

	  if (buf[i] == 0)
	    {
	      desc->symbols[symbolIndex]= fts_new_symbol_copy(buf);
	      symbolIndex++;
	      i = 0;
	    }
	  else
	    i++;
	}
    }

  return 1;
}

static void fts_binary_file_dispose( fts_binary_file_desc_t *desc)
{
  fts_free( desc->code);
  fts_free( desc->symbols);
}

/* Return the top of the object stack, usually the last object created
   at top level (again, usually the top level patcher, but can be different
   for clipboards).
   */
fts_object_t *fts_binary_file_load( const char *name, fts_object_t *parent, int ac, const fts_atom_t *at)
{
  FILE *f;
  fts_object_t *obj;
  fts_binary_file_desc_t desc;

  if ( !(f = fopen( name, "rb")))
    return 0;

  if (fts_binary_file_map(f, &desc) < 0)
    {
      fclose(f);
      post("fts_binary_file_load: Cannot load jMax max file %s\n", name);
      return 0;
    }

  fclose(f);

  obj = fts_run_mess_vm(parent, desc.code, desc.symbols, ac, at);

  fts_binary_file_dispose( &desc);

  return obj;
}


/* Return the top of the object stack, usually the last object created
   at top level (again, usually the top level patcher, but can be different
   for clipboards).
   */

fts_object_t *fts_binary_filedesc_load( FILE *f, fts_object_t *parent, int ac, const fts_atom_t *at)
{
  fts_object_t *obj;
  fts_binary_file_desc_t desc;

  /* Rewind the file */

  fseek(f, 0, SEEK_SET);

  /* Read it */

  if (fts_binary_file_map(f, &desc) < 0)
    return 0;

  /* Eval it */

  obj = fts_run_mess_vm(parent, desc.code, desc.symbols, ac, at);

  fts_binary_file_dispose( &desc);

  return obj;
}


/* **********************************************************************
 *
 * bmax file saver
 *
 */


/* saver dumper utility */
static fts_metaclass_t *saver_dumper_type = 0;
static fts_symbol_t s_saver_dumper = 0;

typedef struct _saver_dumper
{
  fts_dumper_t head;
  fts_bmax_file_t *file;
} saver_dumper_t;

#define saver_dumper_set_file(d, f) ((d)->file = (f))

static saver_dumper_t *saver_dumper = 0;

#define DEFAULT_SYMBOL_TABLE_SIZE 4*1024

static int 
fts_bmax_file_open_fd( fts_bmax_file_t *f, FILE *file, fts_symbol_t *symbol_table, int symbol_table_size)
{
  fts_binary_file_header_t header;

  /* allocate a bmax descriptor, and initialize it */
  if (symbol_table)
    {
      f->symbol_table_static = 1;
      f->symbol_table_size = symbol_table_size;
      f->symbol_table = symbol_table;
    }
  else
    {
      f->symbol_table_static = 0;
      f->symbol_table_size = DEFAULT_SYMBOL_TABLE_SIZE;;
      f->symbol_table = (fts_symbol_t *)fts_malloc( f->symbol_table_size * sizeof( fts_symbol_t));
    }

  /* Initialize the header */
  f->header.magic_number = FTS_BINARY_FILE_MAGIC;
  f->header.code_size = 0;
  f->header.n_symbols = 0;

  f->file = file;

  /* write the init header to the file */
  header = f->header;
  if (has_to_swap())
    {
      swap_long( &header.magic_number);
      swap_long( &header.code_size);
      swap_long( &header.n_symbols);
    }

  if ( fwrite( &header, sizeof(fts_binary_file_header_t), 1, f->file) < 1)
    {
      perror("fts_open_bmax_filedesc_for_writing: write header ");
      return -1;
    }

  if(!saver_dumper)
    {
      /* create dumper */
      saver_dumper = (saver_dumper_t *)fts_object_create(saver_dumper_type, 0, 0);
      fts_object_refer(saver_dumper);
    }

  saver_dumper_set_file(saver_dumper, f);

  f->dumper = saver_dumper;

  return 0;
}

int 
fts_bmax_file_open( fts_bmax_file_t *f, const char *name, int dobackup, fts_symbol_t *symbol_table, int symbol_table_size)
{
  FILE *file;

  if (dobackup)
    {
      char backup[1024];	/* should use the correct posix constant */

      /* Get the backup file name */
      sprintf(backup, "%s.backup", name);

      /* if the file exists, rename the old file, to keep a backup; ignore the error,
	 if we cannot autosave, we need anyway to try saving ! also, ignoring
	 the error catch the case where "name" do not exists.
	 */
      rename(name, backup);
    }

  /* allocate a bmax descriptor, and initialize it */
  file = fopen(name, "wb");
  if (file == 0)
    {
      post("Cannot save to: %s\n", name);
      return -1;
    }

  return fts_bmax_file_open_fd( f, file, symbol_table, symbol_table_size);
}


static void fts_bmax_file_sync( fts_bmax_file_t *f)
{
  unsigned int i;
  char c;
  fts_binary_file_header_t header;

  /* Write the symbol table */

  for ( i = 0; i < f->header.n_symbols; i++)
    {
      fwrite(f->symbol_table[i], sizeof(char),
	     strlen(f->symbol_table[i])+1, f->file);
    }


  /* Write the ending zero */
  c = '\0';
  fwrite(&c, sizeof(char), 1, f->file);

  /* seek to the beginning and rewrite the header */

  fseek(f->file, 0, SEEK_SET);

  header = f->header;
  if (has_to_swap())
    {
      swap_long( &header.magic_number);
      swap_long( &header.code_size);
      swap_long( &header.n_symbols);
    }

  fwrite( &header, sizeof(fts_binary_file_header_t), 1, f->file);

  /* free the bmax file descriptor */
  if (f->symbol_table_static == 0)
      fts_free(f->symbol_table);
}


void fts_bmax_file_close( fts_bmax_file_t *f)
{
  fts_bmax_file_sync( f);
  fclose( f->file);
}


/* Aux functions  for file building */

static int
fts_bmax_find_objidx_in_selection(fts_object_t *obj, fts_selection_t *sel)
{
  /* As object idx, i.e. the location where to store
     the object in the object frame, we compute the
     position of the object in the object list for the selection,
     without counting null pointers.
     */

  int i, idx;

  idx = 0;

  for (i = 0; i < sel->objects_size; i++)
    {
      if (obj == sel->objects[i])
	return idx;
      
      if (sel->objects[i])
	idx++;
    }

  /* If we exit from here, a big coding error have been done,
     or a message system inconsistency discovered !!! */
  return -1;
}


static int
fts_bmax_find_objidx(fts_object_t *obj)
{
  /* As object idx, i.e. the location where to store
     the object in the object frame, we compute the
     position of the object in the child list of a patcher.
     Not so easy to store in the object, because change
     with editing operations, so at the end may be simpler
     to compute it, or at most to have an hash table, or to cache
     the value in a object property or slot
     */

  int i;
  fts_patcher_t *patcher = fts_object_get_patcher(obj);
  fts_object_t *p;

  i = 0;
  for (p = patcher->objects; p ; p = p->next_in_patcher)
    {
      if (obj == p)
	return i;

      i++;
    }

  /* If we exit from here, a big coding error have been done,
     or a message system inconsistency discovered !!! */
  return -1;
}

static int 
fts_bmax_add_symbol(fts_bmax_file_t *f, fts_symbol_t sym)
{
  unsigned int i;

  /* First, search for the symbol in the symbol table */
  for (i = 0; i < f->header.n_symbols; i++)
    if (f->symbol_table[i] == sym)
      return i;
  
  /* is not there, add it, resizing the table if there is no place */
  if (f->header.n_symbols >= f->symbol_table_size)
    {
      /* resize symbol table  */
      fts_symbol_t *new_table;
      int new_size;

      new_size  = (f->symbol_table_size * 3) / 2;
      new_table = (fts_symbol_t *) fts_malloc(new_size * sizeof(fts_symbol_t)); 

      for (i = 0; i < f->symbol_table_size; i++)
	new_table[i] = f->symbol_table[i];

      if (f->symbol_table_static == 0)
	fts_free(f->symbol_table);


      f->symbol_table = new_table;
      f->symbol_table_size = new_size;
      f->symbol_table_static = 0;
    }

  f->symbol_table[f->header.n_symbols] = sym;

  return f->header.n_symbols ++ ; /* !!! POST increment here !!! */
}
				 

/* Utilities */
static unsigned char 
fts_bmax_get_argcode(int value)
{
  if ((127 >= value) && (value >= -128))
    return FVM_B_ARG;
  else if ((32767 >= value) && (value >= -32768))
    return FVM_S_ARG;
  else
    return FVM_L_ARG;
}

static void 
fts_bmax_write_opcode_for(fts_bmax_file_t *f, unsigned char opcode, int value)
{
  unsigned char c = opcode | fts_bmax_get_argcode(value);

  fwrite(&c, sizeof(char), 1, f->file);  
  f->header.code_size++;
}

static void 
fts_bmax_write_opcode(fts_bmax_file_t *f, unsigned char opcode)
{
  unsigned char c = opcode;
  
  fwrite(&c, sizeof(char), 1, f->file);  
  f->header.code_size++;
}

static void 
fts_bmax_write_b_int(fts_bmax_file_t *f, int value)
{
  unsigned char c;

  c = (char) ((unsigned int) (value & 0x000000ff));
  fwrite(&c, sizeof(char), 1, f->file);  
  f->header.code_size++;
}

static void 
fts_bmax_write_s_int(fts_bmax_file_t *f, int value)
{
  unsigned char c[2];
      
  c[1] = (unsigned char) ((unsigned int) value & 0x000000ff);
  c[0] = (unsigned char) (((unsigned int) value & 0x0000ff00) >> 8);

  fwrite(c, sizeof(char), 2, f->file);        
  f->header.code_size += 2;
}

static void 
fts_bmax_write_l_int(fts_bmax_file_t *f, int value)
{
  unsigned char c[4];
      
  c[3] = (unsigned char) (((unsigned int) value & 0x000000ff) >> 0);
  c[2] = (unsigned char) (((unsigned int) value & 0x0000ff00) >> 8);
  c[1] = (unsigned char) (((unsigned int) value & 0x00ff0000) >> 16);
  c[0] = (unsigned char) (((unsigned int) value & 0xff000000) >> 24);

  fwrite(c, sizeof(char), 4, f->file);        
  f->header.code_size += 4;
}

static void 
fts_bmax_write_int(fts_bmax_file_t *f, int value)
{
  unsigned char argcode  = fts_bmax_get_argcode(value);

  if (argcode == FVM_B_ARG)
    fts_bmax_write_b_int(f, value);
  else if (argcode == FVM_S_ARG)
    fts_bmax_write_s_int(f, value);
  else if (argcode == FVM_L_ARG)
    fts_bmax_write_l_int(f, value);
}

static void 
fts_bmax_write_float(fts_bmax_file_t *f, float value)
{
  float fv = value;
  unsigned int fx = *((unsigned int *)&fv);
  unsigned char c[4];
      
  c[3] = (unsigned char) (((unsigned int) fx & 0x000000ff) >> 0);
  c[2] = (unsigned char) (((unsigned int) fx & 0x0000ff00) >> 8);
  c[1] = (unsigned char) (((unsigned int) fx & 0x00ff0000) >> 16);
  c[0] = (unsigned char) (((unsigned int) fx & 0xff000000) >> 24);

  fwrite(c, sizeof(char), 4, f->file);        
  f->header.code_size += 4;
}


/* One functions for each opcode (without considering the argument length) */
void 
fts_bmax_code_return(fts_bmax_file_t *f)
{
  /* RETURN */

  fts_bmax_write_opcode(f, FVM_RETURN);
}

static void 
fts_bmax_code_push_int(fts_bmax_file_t *f, int value)
{
  /* PUSH_INT   <int>   */

  fts_bmax_write_opcode_for(f, FVM_PUSH_INT, value);
  fts_bmax_write_int(f, value);
}

static void 
fts_bmax_code_push_float(fts_bmax_file_t *f, float value)
{
  /* PUSH_FLOAT <float> */

  fts_bmax_write_opcode(f, FVM_PUSH_FLOAT);
  fts_bmax_write_float(f, value);
}

void 
fts_bmax_code_push_symbol(fts_bmax_file_t *f, fts_symbol_t sym)
{
  /* PUSH_SYM   <int>   */
  int value;

  value = fts_bmax_add_symbol(f, sym);

  fts_bmax_write_opcode_for(f, FVM_PUSH_SYM, value );
  fts_bmax_write_int(f, value);
}

static void 
fts_bmax_code_set_int(fts_bmax_file_t *f, int value)
{
  /* SET_INT   <int>   */

  fts_bmax_write_opcode_for(f, FVM_SET_INT, value);
  fts_bmax_write_int(f, value);
}

static void 
fts_bmax_code_set_float(fts_bmax_file_t *f, float value)
{
  /* SET_FLOAT <float> */

  fts_bmax_write_opcode(f, FVM_SET_FLOAT);
  fts_bmax_write_float(f, value);
}

static void 
fts_bmax_code_set_symbol(fts_bmax_file_t *f, fts_symbol_t sym)
{
  /* SET_SYM   <int>   */
  int value;

  value = fts_bmax_add_symbol(f, sym);

  fts_bmax_write_opcode_for(f, FVM_SET_SYM, value );
  fts_bmax_write_int(f, value);
}

void 
fts_bmax_code_pop_args(fts_bmax_file_t *f, int value)
{
  /* POP_ARGS    <int>   // pop n values  from the argument stack */

  fts_bmax_write_opcode_for(f, FVM_POP_ARGS, value );
  fts_bmax_write_int(f, value);
}

static void 
fts_bmax_code_push_obj(fts_bmax_file_t *f, int value)
{
  /* PUSH_OBJ   <objidx> */

  fts_bmax_write_opcode_for(f, FVM_PUSH_OBJ, value );
  fts_bmax_write_int(f, value);
}

static void 
fts_bmax_code_mv_obj(fts_bmax_file_t *f, int value)
{
  /* MV_OBJ     <objidx> */

  fts_bmax_write_opcode_for(f, FVM_MV_OBJ, value );
  fts_bmax_write_int(f, value);
}

static void 
fts_bmax_code_pop_objs(fts_bmax_file_t *f, int value)
{
  /* POP_OBJS    <int> */

  fts_bmax_write_opcode_for(f, FVM_POP_OBJS, value );
  fts_bmax_write_int(f, value);
}

static void 
fts_bmax_code_make_obj(fts_bmax_file_t *f, int value)
{
  /* MAKE_OBJ   <nargs> */

  fts_bmax_write_opcode_for(f, FVM_MAKE_OBJ, value );
  fts_bmax_write_int(f, value);
}

static void fts_bmax_code_make_top_obj(fts_bmax_file_t *f, int value)
{
  /* MAKE_TOP_OBJ   <nargs> */

  fts_bmax_write_opcode_for(f, FVM_MAKE_TOP_OBJ, value );
  fts_bmax_write_int(f, value);
}

static void 
fts_bmax_code_put_prop(fts_bmax_file_t *f, fts_symbol_t sym)
{
  /* PUT_PROP   <sym> */
  int value;

  value = fts_bmax_add_symbol(f, sym);

  fts_bmax_write_opcode_for(f, FVM_PUT_PROP, value );
  fts_bmax_write_int(f, value);
}

void fts_bmax_code_obj_mess(fts_bmax_file_t *f, int inlet, fts_symbol_t sel, int nargs)
{
  /* OBJ_MESS   <inlet> <sel> <nargs> */

  fts_bmax_write_opcode(f, FVM_OBJ_MESS);
  fts_bmax_write_l_int(f, inlet);
  fts_bmax_write_l_int(f, fts_bmax_add_symbol(f, sel));
  fts_bmax_write_l_int(f, nargs);
}

static void 
fts_bmax_code_push_obj_table(fts_bmax_file_t *f, int value)
{
  /* PUSH_OBJ_TABLE <int> */

  fts_bmax_write_opcode_for(f, FVM_PUSH_OBJ_TABLE, value);
  fts_bmax_write_int(f, value);
}

static void 
fts_bmax_code_pop_obj_table(fts_bmax_file_t *f)
{
  /* POP_OBJ_TABLE */

  fts_bmax_write_opcode(f, FVM_POP_OBJ_TABLE);
}

static void 
fts_bmax_code_connect(fts_bmax_file_t *f)
{
  /* CONNECT */

  fts_bmax_write_opcode(f, FVM_CONNECT);
}

/* push atom args: pushed backward, to have them in the right order in the stack at execution time */
void 
fts_bmax_code_push_atoms(fts_bmax_file_t *f, int ac, const fts_atom_t *at)
{
  int i;

  for (i = (ac - 1); i >= 0; i--)
    {
      const fts_atom_t *a = &(at[i]);

      if (fts_is_int(a))
	fts_bmax_code_push_int(f, fts_get_int(a));
      else if (fts_is_float(a))
	fts_bmax_code_push_float(f, fts_get_float(a));
      else if (fts_is_symbol(a))
	fts_bmax_code_push_symbol(f, fts_get_symbol(a));
    }
}

/* set atom arg: set an atom as top of the stack */
static void 
fts_bmax_code_set_atom(fts_bmax_file_t *f, const fts_atom_t *a)
{
  if (fts_is_int(a))
    fts_bmax_code_set_int(f, fts_get_int(a));
  else if (fts_is_float(a))
    fts_bmax_code_set_float(f, fts_get_float(a));
  else if (fts_is_symbol(a))
    fts_bmax_code_set_symbol(f, fts_get_symbol(a));
}

/* Objects, connections, patchers */
static void
fts_bmax_code_new_property(fts_bmax_file_t *f, fts_object_t *obj, fts_symbol_t prop)
{
  fts_atom_t value;
  
  fts_object_get_prop(obj, prop, &value);

  if(!fts_is_void(&value))
    {
      fts_bmax_code_set_atom(f, &value);
      fts_bmax_code_put_prop(f, prop);
    }
}

/* Code a new object, and leave him in the top of the object stack */
void fts_bmax_code_new_object(fts_bmax_file_t *f, fts_object_t *obj, int objidx)
{
  fts_atom_t a;

  /* Push the object arguments, make the object, and put it in the object table, then push the args 
   * and the objects (should we, or should we group the pop later ?).
   * 
   * The pop  of the arguments is done at the end, because we reuse
   * the top of the stack for properties (use set instead of push)
   */

  fts_bmax_code_push_atoms(f, obj->argc, obj->argv);

  fts_bmax_code_make_obj(f, obj->argc);

  if (objidx >= 0)
    fts_bmax_code_mv_obj(f, objidx);

  /* If the argc is zero, we push a zero value to the value
     stack, in order to use "set" and not "push/pop" in the
     property coding; in this case, we don't care about optimizing code size:
     an object with zero argument is an empty "error" object, left there by mistake
     during editing.
     */
  if (obj->argc == 0)
    fts_bmax_code_push_int(f, 0);

  /* Write persistent properties to the file.
     Here, it should use some property data base to find out
     the good properties.
     */
  fts_bmax_code_new_property(f, obj, fts_s_x);
  fts_bmax_code_new_property(f, obj, fts_s_y);
  fts_bmax_code_new_property(f, obj, fts_s_height);
  fts_bmax_code_new_property(f, obj, fts_s_width);

  fts_bmax_code_new_property(f, obj, fts_s_font);
  fts_bmax_code_new_property(f, obj, fts_s_fontSize);
  fts_bmax_code_new_property(f, obj, fts_s_fontStyle);

  /* slider properties */
  fts_bmax_code_new_property(f, obj, fts_s_orientation);
  fts_bmax_code_new_property(f, obj, fts_s_min_value);
  fts_bmax_code_new_property(f, obj, fts_s_max_value);

  fts_bmax_code_new_property(f, obj, fts_s_comment);
  fts_bmax_code_new_property(f, obj, fts_s_layer);
  
  /* button properties */
  fts_bmax_code_new_property(f, obj, fts_s_color);
  fts_bmax_code_new_property(f, obj, fts_s_flash);

  if (fts_object_is_patcher(obj))
    {
      fts_bmax_code_new_property(f, obj, fts_s_wx);
      fts_bmax_code_new_property(f, obj, fts_s_wy);
      fts_bmax_code_new_property(f, obj, fts_s_wh);
      fts_bmax_code_new_property(f, obj, fts_s_ww);

      if (fts_patcher_is_standard((fts_patcher_t *)obj))
	{
	  fts_bmax_code_new_property(f, obj, fts_s_ninlets);
	  fts_bmax_code_new_property(f, obj, fts_s_noutlets);
	}
    }

  /* If argc is zero, we pop the 0 value pushed above */
  if  (obj->argc == 0)
    fts_bmax_code_pop_args(f, 1);
  else
    fts_bmax_code_pop_args(f, obj->argc);

  /* send a dump message to the object to give it the opportunity to save its data */
  fts_object_get_prop(obj, fts_s_keep, &a);

  /* dump if there is no keep property or if keep=yes */
  if(fts_is_void(&a) || (fts_is_symbol(&a) && fts_get_symbol(&a) == fts_s_yes))
    {
      fts_set_object(&a, (fts_object_t *)saver_dumper);
      fts_send_message(obj, fts_system_inlet, fts_s_dump, 1, &a);
    }
}


/* Code the top level patcher object instantiation and properties */
static void
fts_bmax_code_new_top_object(fts_bmax_file_t *f, fts_object_t *obj, int objidx)
{
  fts_atom_t a;

  /* Push the object arguments, make the object, and put it in the object table, then push the args 
   * and the objects (should we, or should we group the pop later ?).
   * 
   * The pop  of the arguments is done at the end, because we reuse
   * the top of the stack for properties (use set instead of push)
   */

  fts_set_symbol(&a, fts_s_patcher);
  fts_bmax_code_push_atoms(f, 1, &a);

  fts_bmax_code_make_top_obj(f, 1);

  if (objidx >= 0)
    fts_bmax_code_mv_obj(f, objidx);

  fts_bmax_code_new_property(f, obj, fts_s_x);
  fts_bmax_code_new_property(f, obj, fts_s_y);
  fts_bmax_code_new_property(f, obj, fts_s_height);
  fts_bmax_code_new_property(f, obj, fts_s_width);

  fts_bmax_code_new_property(f, obj, fts_s_font);
  fts_bmax_code_new_property(f, obj, fts_s_fontSize);
  fts_bmax_code_new_property(f, obj, fts_s_fontStyle);

  fts_bmax_code_new_property(f, obj, fts_s_wx);
  fts_bmax_code_new_property(f, obj, fts_s_wy);
  fts_bmax_code_new_property(f, obj, fts_s_wh);
  fts_bmax_code_new_property(f, obj, fts_s_ww);
  fts_bmax_code_new_property(f, obj, fts_s_ninlets);
  fts_bmax_code_new_property(f, obj, fts_s_noutlets);

  fts_bmax_code_new_property(f, obj, fts_s_comment);
  fts_bmax_code_new_property(f, obj, fts_s_layer);
  fts_bmax_code_new_property(f, obj, fts_s_color);
  fts_bmax_code_new_property(f, obj, fts_s_flash);

  /* pop the argument */
  fts_bmax_code_pop_args(f, 1);
}


static void
fts_bmax_code_new_connection(fts_bmax_file_t *f, fts_connection_t *conn, int fromidx)
{
  if(fts_connection_get_type(conn) > fts_c_hidden)
    {
      /* Push the inlet and outlet (this order) in the evaluation stack */
      fts_bmax_code_push_int(f, conn->winlet);
      fts_bmax_code_push_int(f, conn->woutlet);
      
      /* Push the to object, push the from object in the object stack */
      fts_bmax_code_push_obj(f, fts_bmax_find_objidx(conn->dst));
      fts_bmax_code_push_obj(f, fromidx);
      
      /* code the connect command */
      fts_bmax_code_connect(f);
      
      /* Pop 2 values from the evaluation stack */
      fts_bmax_code_pop_args(f, 2);
      
      /* Pop 2 object from the object stack */
      fts_bmax_code_pop_objs(f, 2);
    }
}

static void
fts_bmax_code_new_connection_in_selection(fts_bmax_file_t *f, fts_connection_t *conn, fts_selection_t *sel)
{
  if(fts_connection_get_type(conn) > fts_c_hidden)
    {
      /* Push the inlet and outlet (this order) in the evaluation stack */
      fts_bmax_code_push_int(f, conn->winlet);
      fts_bmax_code_push_int(f, conn->woutlet);
      
      /* Push the to object, push the from object in the object stack */
      fts_bmax_code_push_obj(f, fts_bmax_find_objidx_in_selection(conn->dst, sel));
      fts_bmax_code_push_obj(f, fts_bmax_find_objidx_in_selection(conn->src, sel));
      
      /* code the connect command */
      fts_bmax_code_connect(f);
      
      /* Pop 2 values from the evaluation stack */
      fts_bmax_code_pop_args(f, 2);
      
      /* Pop 2 object from the object stack */
      fts_bmax_code_pop_objs(f, 2);
    }
}

/* Code a new patcher, and leave it in the top of the stack !!! */
static void
fts_bmax_code_new_patcher(fts_bmax_file_t *f, fts_object_t *obj, int idx, int top)
{
  int i;
  fts_patcher_t *patcher = (fts_patcher_t *) obj;
  fts_object_t *p;

  /* First generate the code to push the patcher in the top of the stack,
     asking for the top_level special code (template argument fetch and
     patcher object instead of 
     (no effect for not a template or no args).
     */
  if (top)
    fts_bmax_code_new_top_object(f, obj, idx);
  else
    fts_bmax_code_new_object(f, obj, idx);
    
  /* Allocate a new object table frame of the right dimension */
  fts_bmax_code_push_obj_table(f, fts_patcher_get_objects_count(patcher));

  /* Code all the objects */
  i = 0;
  for (p = patcher->objects; p ; p = p->next_in_patcher)
    {
      if (fts_object_is_patcher(p) &&
	  (! fts_object_is_abstraction(p)) &&
	  (! fts_object_is_template(p)))
	{
	  /* Save the object recursively as a patcher, and then pop it from the stack */

	  fts_bmax_code_new_patcher(f, p, i, 0);
	  fts_bmax_code_pop_objs(f, 1);
	}
      else
	{
	  /* Code a new object and pop it from the object stack */
	  fts_bmax_code_new_object(f, p, i);
	  fts_bmax_code_pop_objs(f, 1);
	}

      i++;
    }

  /* For each object, for each outlet, code all the connections */
  i = 0;
  for (p = patcher->objects; p ; p = p->next_in_patcher)
    {
      int outlet;

      for (outlet = 0; outlet < fts_object_get_outlets_number(p); outlet++)
	{
	  fts_connection_t *c;

	  for (c = p->out_conn[outlet]; c ; c = c->next_same_src)
	    fts_bmax_code_new_connection(f, c, i);
	}

      i++;
    }

  /* Finally, pop the object table */
  fts_bmax_code_pop_obj_table(f);
}

void 
fts_save_patcher_as_bmax(fts_symbol_t file, fts_object_t *patcher)
{
  fts_bmax_file_t f;

  if ( fts_bmax_file_open( &f, file, 1, 0, 0) < 0)
    return; /* !!! */

  fts_bmax_code_new_patcher( &f, patcher, -1, 1);

  /* code the return command */
  fts_bmax_code_return( &f);
  fts_bmax_file_close( &f);

  /* Recompute the template instances if needed */
  fts_template_file_modified(file);
}


/* Save_simple is a version that:
   1- get a char instead of a symbol (used in autosave to not 
      generate new symbols in panic situation).
   2- Do not update template instances, so to avoid allocating
      memory.
      

      Used *only* for autosave
   */

/* Use static structure; cannot save recursively; needed to avoid alloc during save
 as much as possible, to try autosaves in panic situations.
 */
#define STATIC_SYMBOL_TABLE_SIZE 32 * 1024
static fts_symbol_t static_symbol_table[STATIC_SYMBOL_TABLE_SIZE];

void 
fts_save_simple_as_bmax( const char *filename, fts_object_t *patcher)
{
  fts_bmax_file_t f;

  if ( fts_bmax_file_open( &f, filename, 1, static_symbol_table, STATIC_SYMBOL_TABLE_SIZE) < 0)
    return;

  fts_bmax_code_new_patcher( &f, patcher, -1, 1);

  /* code the return command */
  fts_bmax_code_return( &f);

  fts_bmax_file_close( &f);
}

static void
fts_bmax_code_new_selection(fts_bmax_file_t *f, fts_object_t *obj)
{
  int i, objidx;
  fts_selection_t *selection = (fts_selection_t *) obj;

  /* Allocate a new object table frame of the right dimension */
  fts_bmax_code_push_obj_table(f, selection->objects_count);

  /* Code all the objects */
  objidx = 0;
  for (i = 0; i < selection->objects_size; i++)
    {
      if (selection->objects[i])
	{
	  fts_object_t *p;

	  p = selection->objects[i];

	  if (fts_object_is_patcher(p) && fts_patcher_is_standard((fts_patcher_t *) p))
	    {
	      /* Save the object recursively as a patcher, and then pop it from the stack */
	      
	      fts_bmax_code_new_patcher(f, p, objidx, 0);
	      fts_bmax_code_pop_objs(f, 1);
	    }
	  else
	    {
	      if(fts_is_connection(p))
		{
		  fts_connection_t *c =(fts_connection_t *)p ;

		  if(fts_selection_connection_ends_selected(selection, c))
		    fts_bmax_code_new_connection_in_selection(f, c, selection);
		}
	      else
		{
		  /* Code a new object and pop it from the object stack */
		  fts_bmax_code_new_object(f, p, objidx);
		  fts_bmax_code_pop_objs(f, 1);
		}
	    }

	  objidx++;
	}
    }

  /* Finally, pop the object table */
  fts_bmax_code_pop_obj_table(f);
}

void fts_save_selection_as_bmax( FILE *file, fts_object_t *selection)
{
  fts_bmax_file_t f;

  fseek(file, 0, SEEK_SET);

  if ( fts_bmax_file_open_fd( &f, file, 0, 0) < 0)
    return; /* !!! */

  fts_bmax_code_new_selection( &f, selection);

  /* code the return command */
  fts_bmax_code_return( &f);
  fts_bmax_file_sync( &f);
}

/******************************************************
 *
 *  saver dumper utility
 *
 */

static void
saver_dumper_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  saver_dumper_t *this = (saver_dumper_t *)o;
  
  fts_bmax_code_push_atoms(this->file, ac, at);
  fts_bmax_code_obj_mess(this->file, fts_system_inlet, s, ac);
  fts_bmax_code_pop_args(this->file, ac);
}

static void
saver_dumper_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dumper_init((fts_dumper_t *)o, saver_dumper_send);
}
  
static void
saver_dumper_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dumper_destroy((fts_dumper_t *)o);
}
  
static fts_status_t
saver_dumper_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(saver_dumper_t), 0, 0, 0);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, saver_dumper_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, saver_dumper_delete);

  return fts_ok;
}

void
fts_saver_config(void)
{
  s_saver_dumper = fts_new_symbol("saver_dumper");

  saver_dumper_type = fts_class_install(s_saver_dumper, saver_dumper_instantiate);
}
