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

/*
 * A jMax binary file format to XML converter
 *
 * Most of the code is copied from fts/loader.c and fts/vm.c
 */

/* define to print the opcodes and arguments as they are read */
/*  #define VM_DEBUG */

#ifdef VM_DEBUG
#define DEBUG_MSG(E) E
#else
#define DEBUG_MSG(E) 
#endif

#include <ftsconfig.h>

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <fts/fts.h>
#include <ftsprivate/bmaxfile.h>

/* **********************************************************************
 *
 * XML generation
 *
 */
#define INDENT 2

static void xml_indent( FILE *out, int *indent)
{
  int i;

  for ( i = 0; i < *indent; i++)
    fputc( ' ', out);
}

static void xml_args( FILE *out, int *indent, int ac, fts_atom_t *at)
{
  int i = 0;

  for ( i = 0; i < ac; i++, at++)
    {
      xml_indent( out, indent);
      fprintf( out, "<arg value=");
      if (fts_is_int( at))
	fprintf( out, "%d", fts_get_int( at));
      else if (fts_is_float( at))
	fprintf( out, "%g", fts_get_float( at));
      else if (fts_is_symbol( at))
	fprintf( out, "\"%s\"", fts_get_symbol( at));

      fprintf( out, " />\n");
    }
}

static void xml_begin_object( FILE *out, int *indent, int ac, fts_atom_t *at)
{
  xml_indent( out, indent);
  fprintf( out, "<object>\n");
  (*indent) += INDENT;
  xml_indent( out, indent);
  fprintf( out, "<descr>\n");
  (*indent) += INDENT;
  xml_args( out, indent, ac, at);
  (*indent) -= INDENT;
  xml_indent( out, indent);
  fprintf( out, "</descr>\n");
}

static void xml_end_object( FILE *out, int *indent)
{
  (*indent) -= INDENT;
  xml_indent( out, indent);
  fprintf( out, "</object>\n");
}

static void xml_id( FILE *out, int *indent, int id)
{
  xml_indent( out, indent);
  fprintf( out, "<id value=%d />\n", id);
}

static void xml_property( FILE *out, int *indent, fts_symbol_t name, fts_atom_t *at)
{
  xml_indent( out, indent);
  fprintf( out, "<property name=\"%s\" value=", name);
  if (fts_is_int( at))
    fprintf( out, "%d", fts_get_int( at));
  else if (fts_is_float( at))
    fprintf( out, "%g", fts_get_float( at));
  else if (fts_is_symbol( at))
    fprintf( out, "\"%s\"", fts_get_symbol( at));
  fprintf( out, " />\n");
}

static void xml_connect( FILE *out, int *indent, int src, int outlet, int dst, int inlet)
{
  xml_indent( out, indent);
  fprintf( out, "<connect src=%d outlet=%d dst=%d inlet=%d />\n", src, outlet, dst, inlet);
}

/* **********************************************************************
 *
 * Binary format interpreter
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


#define EVAL_STACK_DEPTH   8192
#define OBJECT_STACK_DEPTH 8192
#define OBJECT_TABLE_STACK_DEPTH 8192

static fts_atom_t eval_stack[EVAL_STACK_DEPTH];
static int        eval_tos = EVAL_STACK_DEPTH;

static int object_stack[OBJECT_STACK_DEPTH];
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

static int run_vm( unsigned char *program, fts_symbol_t symbol_table[], FILE *out)
{
  int i;
  unsigned char cmd;
  unsigned char *p;
  int indent = 0;

  /* setting arguments and parent */

  object_tos--;
  object_stack[object_tos] = 0;

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
	    DEBUG_MSG( fprintf( stderr, "PUSH_INT_B %d\n", GET_B(p)));

	    eval_tos--;
	    fts_set_int(&eval_stack[eval_tos], GET_B(p));
	    p++;
	  }
	break;

	case FVM_PUSH_INT_S:
	  {
	    /* PUSH_INT_S   <int> */
	    DEBUG_MSG( fprintf( stderr, "PUSH_INT_S %d\n", GET_S(p)));

	    eval_tos--;
	    fts_set_int(&eval_stack[eval_tos], GET_S(p));
	    p += 2;
	  }
	break;

	case FVM_PUSH_INT_L:
	  {
	    /* PUSH_INT_L   <int> */
	    DEBUG_MSG( fprintf( stderr, "PUSH_INT_L %d\n", GET_L(p)));

	    eval_tos--;
	    fts_set_int(&eval_stack[eval_tos], GET_L(p));
	    p += 4;
	  }
	break;

	case FVM_PUSH_FLOAT:
	  {
	    /* PUSH_FLOAT <float> */
	    DEBUG_MSG( fprintf( stderr, "PUSH_FLOAT %f\n", GET_F(p)));
	    
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
	    DEBUG_MSG( fprintf( stderr, "PUSH_SYM_B %d (%s)\n", GET_B(p), s));

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
	    DEBUG_MSG( fprintf( stderr, "PUSH_SYM_S %s\n", s));

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
	    DEBUG_MSG( fprintf( stderr, "PUSH_SYM_L %s\n", s));

	    eval_tos--;
	    fts_set_symbol(&eval_stack[eval_tos], s);
	    p += 4;
	  }
	break;

	case FVM_SET_INT_B:
	  {
	    /* SET_INT_B   <int> */
	    DEBUG_MSG( fprintf( stderr, "SET_INT_B %d\n", GET_B(p)));

	    fts_set_int(&eval_stack[eval_tos], GET_B(p));
	    p++;
	  }
	break;

	case FVM_SET_INT_S:
	  {
	    /* SET_INT_S   <int> */
	    DEBUG_MSG( fprintf( stderr, "SET_INT_S %d\n", GET_S(p)));

	    fts_set_int(&eval_stack[eval_tos], GET_S(p));
	    p += 2;
	  }
	break;

	case FVM_SET_INT_L:
	  {
	    /* SET_INT_L   <int> */
	    DEBUG_MSG( fprintf( stderr, "SET_INT_L %d\n", GET_L(p)));

	    fts_set_int(&eval_stack[eval_tos], GET_L(p));
	    p += 4;
	  }
	break;

	case FVM_SET_FLOAT:
	  {
	    /* SET_FLOAT <float> */
	    DEBUG_MSG( fprintf( stderr, "SET_FLOAT %f\n", GET_F(p)));
	    
	    fts_set_float(&eval_stack[eval_tos], GET_F(p));
	    p += 4;
	  }
	break;

	case FVM_SET_SYM_B:
	  {
	    /* SET_SYM_B   <idx> */
	    DEBUG_MSG( fprintf( stderr, "SET_SYM_B %d\n", GET_B(p)));

	    fts_set_symbol(&eval_stack[eval_tos], symbol_table[GET_B(p)]);
	    p++;
	  }
	break;

	case FVM_SET_SYM_S:
	  {
	    /* SET_SYM_S   <idx> */
	    DEBUG_MSG( fprintf( stderr, "SET_SYM_S %d\n", GET_S(p)));

	    fts_set_symbol(&eval_stack[eval_tos], symbol_table[GET_S(p)]);
	    p += 2;
	  }
	break;

	case FVM_SET_SYM_L:
	  {
	    /* SET_SYM_L   <idx> */
	    DEBUG_MSG( fprintf( stderr, "SET_SYM_L %d\n", GET_L(p)));

	    fts_set_symbol(&eval_stack[eval_tos], symbol_table[GET_L(p)]);
	    p += 4;
	  }
	break;

	case FVM_POP_ARGS_B:
	  {
	    /* POP_ARGS_B    <int> */
	    DEBUG_MSG( fprintf( stderr, "POP_ARGS_B %d\n", GET_B(p)));

	    eval_tos += GET_B(p);
	    p++;
	  }
	break;

	case FVM_POP_ARGS_S:
	  {
	    /* POP_ARGS_S    <int> */
	    DEBUG_MSG( fprintf( stderr, "POP_ARGS_S %d\n", GET_S(p)));

	    eval_tos += GET_S(p);
	    p += 2;
	  }
	break;

	case FVM_POP_ARGS_L:
	  {
	    /* POP_ARGS_L    <int> */
	    DEBUG_MSG( fprintf( stderr, "POP_ARGS_L %d\n", GET_L(p)));

	    eval_tos += GET_L(p);
	    p += 4;
	  }
	break;


	/* Object stack manipulation */

	case FVM_PUSH_OBJ_B:
	  {
	    /* PUSH_OBJ_B   <objidx> */
	    DEBUG_MSG( fprintf( stderr, "PUSH_OBJ_B %d\n", GET_B(p)));

	    object_tos--;
	    object_stack[object_tos] = GET_B(p);
	    p += 1;
	  }
	break;

	case FVM_PUSH_OBJ_S:
	  {
	    /* PUSH_OBJ_S   <objidx> */
	    DEBUG_MSG( fprintf( stderr, "PUSH_OBJ_S %d\n", GET_S(p)));

	    object_tos--;
	    object_stack[object_tos] = GET_S(p);

	    p += 2;
	  }
	break;

	case FVM_PUSH_OBJ_L:
	  {
	    /* PUSH_OBJ_L   <objidx> */
	    DEBUG_MSG( fprintf( stderr, "PUSH_OBJ_L %d\n", GET_L(p)));

	    object_tos--;
	    object_stack[object_tos] = GET_L(p);

	    p += 4;
	  }
	break;

	case FVM_MV_OBJ_B:
	  {
	    int id;

	    /* MV_OBJ_B     <objidx> */
	    DEBUG_MSG( fprintf( stderr, "MV_OBJ_B %d\n", GET_B(p)));

	    id = GET_B(p);
/*  	    object_table[GET_B(p)] = object_stack[object_tos]; */

	    xml_id( out, &indent, id);

	    p += 1;
	  }
	break;

	case FVM_MV_OBJ_S:
	  {
	    int id;

	    /* MV_OBJ_S     <objidx> */
	    DEBUG_MSG( fprintf( stderr, "MV_OBJ_S %d\n", GET_S(p)));

	    id = GET_S(p);
/*  	    object_table[GET_S(p)] = object_stack[object_tos]; */

	    xml_id( out, &indent, id);

	    p += 2;
	  }
	break;

	case FVM_MV_OBJ_L:
	  {
	    int id;

	    /* MV_OBJ_L     <objidx> */
	    DEBUG_MSG( fprintf( stderr, "MV_OBJ_L %d\n", GET_L(p)));

	    id = GET_L(p);
/*  	    object_table[GET_L(p)] = object_stack[object_tos]; */

	    xml_id( out, &indent, id);

	    p += 4;
	  }
	break;


	case FVM_POP_OBJS_B:
	  {
	    int n;

	    /* POP_OBJS_B    <int> */
	    DEBUG_MSG( fprintf( stderr, "POP_OBJ_B %d\n", GET_B(p)));

	    n = GET_B(p);
	    if ( n == 1)
	      xml_end_object( out, &indent);

	    object_tos += GET_B(p);
	    p += 1;
	  }
	break;

	case FVM_POP_OBJS_S:
	  {
	    int n;

	    /* POP_OBJS_S    <int> */
	    DEBUG_MSG( fprintf( stderr, "POP_OBJ_S %d\n", GET_S(p)));

	    n = GET_B(p);
	    if ( n == 1)
	      xml_end_object( out, &indent);

	    object_tos += GET_S(p);
	    p += 2;
	  }
	break;


	case FVM_POP_OBJS_L:
	  {
	    int n;

	    /* POP_OBJS_L    <int> */
	    DEBUG_MSG( fprintf( stderr, "POP_OBJ_L %d\n", GET_L(p)));

	    n = GET_B(p);
	    if ( n == 1)
	      xml_end_object( out, &indent);

	    object_tos += GET_L(p);
	    p += 4;
	  }
	break;

	/* Object Manipulatation */

	case FVM_MAKE_OBJ_B:
	  {
	    /* MAKE_OBJ_B   <nargs> */

	    fts_object_t *new = 0;
	    int nargs = GET_B(p);

	    DEBUG_MSG( fprintf( stderr, "MAKE_OBJ_B %d\n", nargs));

	    xml_begin_object( out, &indent, nargs, &eval_stack[eval_tos]);

	    /* Push the object in the object stack */

	    object_tos--;
	    object_stack[object_tos] = 0;
	    p += 1;
	  }
	  break;	

	case FVM_MAKE_OBJ_S:
	  {
	    /* MAKE_OBJ_S   <nargs> */

	    fts_object_t *new = 0;
	    int nargs = GET_S(p);

	    DEBUG_MSG( fprintf( stderr, "MAKE_OBJ_S %d\n", nargs));

	    xml_begin_object( out, &indent, nargs, &eval_stack[eval_tos]);

	    /* Push the object in the object stack */
	    object_tos--;
	    object_stack[object_tos] = 0;
	    p += 2;
	  }
	  break;	


	case FVM_MAKE_OBJ_L:
	  {
	    /* MAKE_OBJ_L   <nargs> */

	    fts_object_t *new = 0;
	    int nargs = GET_L(p);

	    DEBUG_MSG( fprintf( stderr, "MAKE_OBJ_L %d\n", nargs));

	    xml_begin_object( out, &indent, nargs, &eval_stack[eval_tos]);

	    /* Push the object in the object stack */

	    object_tos--;
	    object_stack[object_tos] = 0;
	    p += 4;
	  }
	  break;	

	case FVM_MAKE_TOP_OBJ_B:
	  {
	    /* MAKE_TOP_OBJ_B   <nargs> */

	    fts_object_t *new = 0;
	    int nargs = GET_B(p);

	    DEBUG_MSG( fprintf( stderr, "MAKE_TOP_OBJ_B %d\n", nargs));

	    /* Push the object in the object stack */

	    object_tos--;
	    object_stack[object_tos] = 0;
	    p += 1;
	  }
	  break;	

	case FVM_MAKE_TOP_OBJ_S:
	  {
	    /* MAKE_TOP_OBJ_S   <nargs> */

	    fts_object_t *new = 0;
	    int nargs = GET_S(p);

	    DEBUG_MSG( fprintf( stderr, "MAKE_TOP_OBJ_S %d\n", nargs));

	    /* Push the object in the object stack */

	    object_tos--;
	    object_stack[object_tos] = 0;
	    p += 2;
	  }
	  break;	


	case FVM_MAKE_TOP_OBJ_L:
	  {
	    /* MAKE_TOP_OBJ_L   <nargs> */

	    fts_object_t *new = 0;
	    int nargs = GET_L(p);

	    DEBUG_MSG( fprintf( stderr, "MAKE_OBJ_L %d\n", nargs));

	    /* Push the object in the object stack */
	    object_tos--;
	    object_stack[object_tos] = 0;
	    p += 4;
	  }
	  break;	

	case FVM_PUT_PROP_B:
	  {
	    fts_symbol_t prop = symbol_table[GET_B(p)];;

	    /* PUT_PROP_B_   <sym> */
	    DEBUG_MSG( fprintf( stderr, "PUT_PROP_B %s\n", prop));

	    xml_property( out, &indent, prop, &eval_stack[eval_tos]);

	    p += 1;
	  }
	break;

	case FVM_PUT_PROP_S:
	  {
	    fts_symbol_t prop = symbol_table[GET_S(p)];

	    /* PUT_PROP_S_   <sym> */
	    DEBUG_MSG( fprintf( stderr, "PUT_PROP_S %s\n", prop));

	    xml_property( out, &indent, prop, &eval_stack[eval_tos]);

	    p += 2;
	  }
	break;

	case FVM_PUT_PROP_L:
	  {
	    fts_symbol_t prop = symbol_table[GET_L(p)];

	    /* PUT_PROP_L_   <sym> */
	    DEBUG_MSG( fprintf( stderr, "PUT_PROP_S %s\n", prop));

	    xml_property( out, &indent, prop, &eval_stack[eval_tos]);

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

	    DEBUG_MSG( fprintf( stderr, "OBJ_MESS %d %s %d\n", inlet, sel, nargs));

/*  	    fts_send_message(object_stack[object_tos], inlet, sel, nargs, &eval_stack[eval_tos]); */
	  }
	break;

	case FVM_CONNECT:
	  {
	    int src, dst, inlet, outlet;

	    /* CONNECT */
	    DEBUG_MSG( fprintf( stderr, "CONNECT\n"));

	    src = object_stack[object_tos];
	    dst = object_stack[object_tos + 1];
	    outlet = fts_get_int(&eval_stack[eval_tos]);
	    inlet = fts_get_int(&eval_stack[eval_tos + 1]);
/*  	    fts_connection_new(FTS_NO_ID, object_stack[object_tos], fts_get_int(&eval_stack[eval_tos]), */
/*  			       object_stack[object_tos + 1], fts_get_int(&eval_stack[eval_tos + 1])); */
	    xml_connect( out, &indent, src, outlet, dst, inlet);
	  }
	break;

	/* Object table stack */

	case FVM_PUSH_OBJ_TABLE_B:
	  {
	    /* PUSH_OBJ_TABLE_B <int> */

	    int size;

	    DEBUG_MSG( fprintf( stderr, "PUSH_OBJ_TABLE_B %d\n", GET_B(p)));

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
	    DEBUG_MSG( fprintf( stderr, "PUSH_OBJ_TABLE_S %d\n", GET_S(p)));

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
	    DEBUG_MSG( fprintf( stderr, "PUSH_OBJ_TABLE_L %d\n", GET_L(p)));

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
	    DEBUG_MSG( fprintf( stderr, "POP_OBJ_TABLE\n"));

	    if (object_table)
	      fts_free(object_table);

	    object_table_tos++;
	    object_table = object_table_stack[object_table_tos];
	  }
	break;

	case FVM_RETURN:
	  {

	    /* RETURN */
	    DEBUG_MSG( fprintf( stderr, "RETURN\n"));

	    object_tos++;	/* take the parent away from the object stack */
	    
	    return 0;
	  }
	}

      CHECK_OBJ_STACK;
      CHECK_EVAL_STACK;
    }
}

/* **********************************************************************
 *
 * File loading
 *
 */

typedef struct fts_binary_file_desc_t {
  unsigned char *code;
  fts_symbol_t *symbols;
} fts_binary_file_desc_t;

union swap_union_t {
  long l;
  char c[4];
};

static int has_to_swap(void)
{
  union swap_union_t u;

  u.l = 0x11223344;
  return u.c[0] != 0x11;
}

static void swap_long( unsigned long *p)
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

static int process_file( FILE *in, FILE *out)
{
  unsigned char *code;
  fts_symbol_t *symbols = 0;
  fts_binary_file_header_t header;

  /* read the header */
  if (fread( &header, sizeof( header), 1, in) < 1)
    return -1;

  if (has_to_swap())
    {
      swap_long( &header.magic_number);
      swap_long( &header.code_size);
      swap_long( &header.n_symbols);
    }

  if (header.magic_number != FTS_BINARY_FILE_MAGIC)
    return -1;

  /* allocate code */
  code = (unsigned char *)fts_malloc( header.code_size);
  if (!code)
    return -1;

  /* read the code */
  if (fread( code, sizeof(char), header.code_size, in) < header.code_size)
    return -1;

  /* allocate code and read symbols */
  if (header.n_symbols > 0)
    {
      int i = 0;
      char buf[1024]; /* max  symbol size */
      unsigned int symbolIndex = 0;

      symbols = (fts_symbol_t *)fts_malloc( header.n_symbols * sizeof( fts_symbol_t));

      /* In case of corrupted file, we initialize the
	 table with the error symbol, so to have some hope
	 of opening the result */

      for (symbolIndex = 0; symbolIndex < header.n_symbols; symbolIndex++)
	symbols[symbolIndex] = fts_s_error;

      symbolIndex = 0;
      while (! feof(in) && (symbolIndex < header.n_symbols))
	{
	  buf[i] = getc(in);

	  if (buf[i] == 0)
	    {
	      symbols[symbolIndex]= fts_new_symbol_copy(buf);
	      symbolIndex++;
	      i = 0;
	    }
	  else
	    i++;
	}
    }

  if (run_vm( code, symbols, out) < 0)
    return -1;

  return 0;
}


/* **********************************************************************
 *
 * main
 *
 */
static void usage( void)
{
  fprintf( stderr, "Usage: jmax2xml JMAX_FILE [XML_FILE]\n");
  fprintf( stderr, "       if JMAX_FILE is -, reads standard input\n");
  fprintf( stderr, "       if XML_FILE is omitted, writes to standard output\n");
  exit( 1);
}

int main( int argc, char **argv)
{
  FILE *in, *out;

  if (argc < 2)
    usage();

  fts_init( 1, argv);

  if ( !strcmp( argv[1], "-"))
    in = stdin;
  else 
    {
      in = fopen( argv[1], "rb");

      if (in == NULL)
	return 1;
    }

  if (argc <= 2)
    out = stdout;
  else
    {
      out = fopen( argv[2], "w");

      if (out == NULL)
	return 1;
    }

  fprintf( out, "<?xml version=\"1.0\"?>\n\n");

  process_file( in, out);

  if (in != stdin)
    fclose( in);
  if (out != stdout)
    fclose( out);

  return 0;
}
