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


/* The jmax patch binary file loader

   Virtual Machine:

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

   PUSH_INT   <int>   // push an int value in the argument stack
   PUSH_FLOAT <float> // push a float value in the argument stack
   PUSH_SYM   <int>   // push a symbol from the file symbol table (by idx) value in the argument stack

   SET_INT   <int>       // set an int value as the top of the argument stack
   SET_FLOAT   <float>   // set an float value as the top of the argument stack
   SET_SYM   <int>   // set a symbol from the file symbol table (by idx) value as the top of the argument stack

   POP_ARGS    <int>   // pop n values  from the argument stack

   PUSH_OBJ   <int>   // Push an object table entry in the object stack
   MV_OBJ     <objidx>    // Copy the top of object stack to an object table entry
   POP_OBJS    <int>   // Push n objects from the object stack
   

   MAKE_OBJ   <nargs>   // Take <nargs> argument from the evaluation stack
                       // and make an object with top of the object stack as parent.
		       // Push the new object in the object stack
		       // Don't touch the argument stack.

   MAKE_TOP_OBJ   <nargs>   // Take <nargs> + lambda argument from the evaluation stack
                       // and make an object with top of the object stack as parent.
		       // Push the new object in the object stack
		       // Don't touch the argument stack.
		       // lambda is the value of a special internal vm register
		       // that is filled loading templates; it is used to
		       // add arguments to the first object created from the vm invocation
		       // environment

   PUT_PROP   <sym> // Put the top of the evaluation stack value
                    // as property <sym> for the object at the top 
		    // of the object stack
		    // don't touch either stack.

   OBJ_MESS   <inlet> <sel> <nargs>  // Send a message <sel> to the inlet <in> object top of the object stack
                         // with <nargs> arguments from the evaluation stack
			 // don't touch the stacks
			 // ONLY LONG ARGUMENTS HERE (rare opcode, no optimization)


   PUSH_OBJ_TABLE <int> // Push an object table of N values
   POP_OBJ_TABLE        // Pop an object table

   CONNECT            // connect outlet (Top of evaluation stack) of object (top of obejct stack)
                      // with inlet (Top - 1 of evaluation stack) of object (top - 1 of obejct stack)

   RETURN            // Return from the current VM execution
                     // the current top of the object stack is returned
		     // by the C function; the stack is popped by one.
   */


/* #define  VM_DEBUG */

#ifdef DEBUG
#define VM_SAFE
#endif

#include <fts/fts.h>
#include <ftsprivate/OLDexpression.h>
#include <ftsprivate/connection.h>
#include <ftsprivate/vm.h>

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

   Also, if the expression argument is not null, the assignement are put 
   in the stack before, with the assignement syntax and constant values.
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

fts_object_t *fts_run_mess_vm(fts_object_t *parent,
			      unsigned char *program,
			      fts_symbol_t symbol_table[],
			      int ac, const fts_atom_t *at,
			      fts_expression_state_t *e)
{
  int i;
  unsigned char cmd;
  unsigned char *p;
  int lambda;

  /* if there is an expression argument, push the assignement description
     in the stack, so that templates will accept argument by name
     Compute the total number of pushed atoms in lambda.
     */

  if (e)
    lambda = fts_expression_map_to_assignements(e, fts_object_push_assignement, 0);
  else
    lambda = 0;

  lambda = 3 * lambda + ac;

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

	    new  = fts_eval_object_description((fts_patcher_t *) object_stack[object_tos], nargs + lambda,
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

	    new  = fts_eval_object_description((fts_patcher_t *) object_stack[object_tos], nargs + lambda,
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
	    new  = fts_eval_object_description((fts_patcher_t *) object_stack[object_tos], nargs + lambda,
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
	    fts_connection_new(FTS_NO_ID, object_stack[object_tos], fts_get_int(&eval_stack[eval_tos]),
			       object_stack[object_tos + 1], fts_get_int(&eval_stack[eval_tos + 1]));
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

	    eval_tos += lambda;

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


