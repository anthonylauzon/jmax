/* An experiment to load binary files.

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
   PUSH_BUILTIN_SYM   <int>   // push a builtin symbol (by idx) value in the argument stack (only byte argument)

   SET_INT   <int>       // set an int value as the top of the argument stack
   SET_FLOAT   <float>   // set an float value as the top of the argument stack
   SET_SYM   <int>   // set a symbol from the file symbol table (by idx) value as the top of the argument stack
   SET_BUILTIN_SYM   <int>   // set a builtin symbol value as top of the argument stack (only byte argument)

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

   PUT_BUILTIN_PROP   <sym> // Like PUT_PROP, but using a builtin symbol as property name (only byte argument)

   OBJ_MESS   <inlet> <sel> <nargs>  // Send a message <sel> to the inlet <in> object top of the object stack
                         // with <nargs> arguments from the evaluation stack
			 // don't touch the stacks
			 // ONLY LONG ARGUMENTS HERE (rare opcode, no optimization)


   OBJ_BUILTIN_MESS   <inlet> <sel> <nargs>
                         // Send a message <sel> (builtin-symbol) to the inlet <in>
			 // object top of the object stack
                         // with <nargs> arguments from the evaluation stack
			 // don't touch the stacks
			 // ONLY LONG ARGUMENTS HERE  apart from <sel> that is a byte

   PUSH_OBJ_TABLE <int> // Push an object table of N values
   POP_OBJ_TABLE        // Pop an object table

   CONNECT            // connect outlet (Top of evaluation stack) of object (top of obejct stack)
                      // with inlet (Top - 1 of evaluation stack) of object (top - 1 of obejct stack)

   RETURN            // Return from the current VM execution
                     // the current top of the object stack is return 
		     // by the C function
   */

/* #define  VM_DEBUG */

#ifdef DEBUG
#define  VM_SAFE
#endif


#include "sys.h"
#include "lang/mess.h"
#include "lang/mess/vm.h"


#define EVAL_STACK_DEPTH   4096
#define OBJECT_STACK_DEPTH 4096
#define OBJECT_TABLE_STACK_DEPTH 4096

static fts_atom_t eval_stack[EVAL_STACK_DEPTH];
static fts_atom_t *eval_tos = eval_stack + EVAL_STACK_DEPTH;

static fts_object_t *object_stack[OBJECT_STACK_DEPTH];
static fts_object_t **object_tos = object_stack + OBJECT_STACK_DEPTH;

static fts_object_t **object_table_stack[OBJECT_TABLE_STACK_DEPTH];
static fts_object_t ***object_table_tos = object_table_stack + OBJECT_TABLE_STACK_DEPTH;
static fts_object_t **object_table = 0;

/* Macros to do checks operations */

#ifdef  VM_SAFE
#define CHECK_OBJ_STACK   \
      { \
	if (object_tos < object_stack) \
         fprintf(stderr, "Object Stack overflow\n"); \
      else if (object_tos > object_stack + OBJECT_STACK_DEPTH) \
         fprintf(stderr, "Object Stack underflow\n"); \
      }

#define CHECK_EVAL_STACK   \
      { \
	if (eval_tos < eval_stack) \
         fprintf(stderr, "Eval Stack overflow\n"); \
      else if (eval_tos > eval_stack + OBJECT_STACK_DEPTH) \
         fprintf(stderr, "Eval Stack underflow\n"); \
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
  *eval_tos = *value;

  fts_set_symbol(&a, fts_s_assign);
  eval_tos--;
  *eval_tos = a;

  fts_set_symbol(&a, name);
  eval_tos--;
  *eval_tos = a;
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

  /* initialize the vm; this part of the code is not recursive, but it should,
   because the vm can be called recursively for templates*/


  /* eval_tos = eval_stack + EVAL_STACK_DEPTH; */  /* point to the actually filled cell */
  /* object_tos = object_stack + OBJECT_STACK_DEPTH; */
  /* object_table_tos = object_table_stack + OBJECT_TABLE_STACK_DEPTH; */

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
  *object_tos = parent;

  for (i = ac - 1; i >= 0; i--)
    {
      /* Push the atoms in the value stack */

      eval_tos--;
      *eval_tos = at[i];
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
	    fprintf(stderr, "PUSH_INT_B %d\n", GET_B(p));
#endif

	    eval_tos--;
	    fts_set_int(eval_tos, GET_B(p));
	    p++;
	  }
	break;

	case FVM_PUSH_INT_S:
	  {
	    /* PUSH_INT_S   <int> */

#ifdef VM_DEBUG
	    fprintf(stderr, "PUSH_INT_S %d\n", GET_S(p));
#endif

	    eval_tos--;
	    fts_set_int(eval_tos, GET_S(p));
	    p += 2;
	  }
	break;

	case FVM_PUSH_INT_L:
	  {
	    /* PUSH_INT_L   <int> */

#ifdef VM_DEBUG
	    fprintf(stderr, "PUSH_INT_L %d\n", GET_L(p));
#endif

	    eval_tos--;
	    fts_set_int(eval_tos, GET_L(p));
	    p += 4;
	  }
	break;

	case FVM_PUSH_FLOAT:
	  {
	    /* PUSH_FLOAT <float> */

#ifdef VM_DEBUG
	    fprintf(stderr, "PUSH_FLOAT %f\n", GET_F(p));
#endif
	    
	    eval_tos--;
	    fts_set_float(eval_tos, GET_F(p));
	    p += 4;
	  }
	break;

	case FVM_PUSH_BUILTIN_SYM:
	  {
	    /* PUSH_BUILTIN_SYM   <idx> */

	    fts_symbol_t s;

	    s  = fts_get_builtin_symbol(GET_B(p));

#ifdef VM_DEBUG
	    fprintf(stderr, "PUSH_BUILTIN_SYM %s\n", fts_symbol_name(s));
#endif

	    eval_tos--;
	    fts_set_symbol(eval_tos, s);
	    p++;
	  }
	break;


	case FVM_PUSH_SYM_B:
	  {
	    /* PUSH_SYM_B   <idx> */

	    fts_symbol_t s;

	    s  = symbol_table[GET_B(p)];

#ifdef VM_DEBUG
	    fprintf(stderr, "PUSH_SYM_B %s\n", fts_symbol_name(s));
#endif

	    eval_tos--;
	    fts_set_symbol(eval_tos, s);
	    p++;
	  }
	break;

	case FVM_PUSH_SYM_S:
	  {
	    /* PUSH_SYM_S   <idx> */

	    fts_symbol_t s;

	    s  = symbol_table[GET_S(p)];

#ifdef VM_DEBUG
	    fprintf(stderr, "PUSH_SYM_S %s\n", fts_symbol_name(s));
#endif

	    eval_tos--;
	    fts_set_symbol(eval_tos, s);
	    p += 2;
	  }
	break;

	case FVM_PUSH_SYM_L:
	  {
	    /* PUSH_SYM_L   <idx> */

	    fts_symbol_t s;

	    s  = symbol_table[GET_L(p)];

#ifdef VM_DEBUG
	    fprintf(stderr, "PUSH_SYM_L %s\n", fts_symbol_name(s));
#endif

	    eval_tos--;
	    fts_set_symbol(eval_tos, s);
	    p += 4;
	  }
	break;

	case FVM_SET_INT_B:
	  {
	    /* SET_INT_B   <int> */

#ifdef VM_DEBUG
	    fprintf(stderr, "SET_INT_B %d\n", GET_B(p));
#endif

	    fts_set_int(eval_tos, GET_B(p));
	    p++;
	  }
	break;

	case FVM_SET_INT_S:
	  {
	    /* SET_INT_S   <int> */

#ifdef VM_DEBUG
	    fprintf(stderr, "SET_INT_S %d\n", GET_S(p));
#endif

	    fts_set_int(eval_tos, GET_S(p));
	    p += 2;
	  }
	break;

	case FVM_SET_INT_L:
	  {
	    /* SET_INT_L   <int> */

#ifdef VM_DEBUG
	    fprintf(stderr, "SET_INT_L %d\n", GET_L(p));
#endif

	    fts_set_int(eval_tos, GET_L(p));
	    p += 4;
	  }
	break;

	case FVM_SET_FLOAT:
	  {
	    /* SET_FLOAT <float> */

#ifdef VM_DEBUG
	    fprintf(stderr, "SET_FLOAT %f\n", GET_F(p));
#endif
	    
	    fts_set_float(eval_tos, GET_F(p));
	    p += 4;
	  }
	break;

	case FVM_SET_BUILTIN_SYM:
	  {
	    /* SET_BUILTIN_SYM   <idx> */

#ifdef VM_DEBUG
	    fprintf(stderr, "SET_BUILTIN_SYM %d\n", GET_B(p));
#endif

	    fts_set_symbol(eval_tos, fts_get_builtin_symbol(GET_B(p)));
	    p++;
	  }
	break;


	case FVM_SET_SYM_B:
	  {
	    /* SET_SYM_B   <idx> */

#ifdef VM_DEBUG
	    fprintf(stderr, "SET_SYM_B %d\n", GET_B(p));
#endif

	    fts_set_symbol(eval_tos, symbol_table[GET_B(p)]);
	    p++;
	  }
	break;

	case FVM_SET_SYM_S:
	  {
	    /* SET_SYM_S   <idx> */

#ifdef VM_DEBUG
	    fprintf(stderr, "SET_SYM_S %d\n", GET_S(p));
#endif

	    fts_set_symbol(eval_tos, symbol_table[GET_S(p)]);
	    p += 2;
	  }
	break;

	case FVM_SET_SYM_L:
	  {
	    /* SET_SYM_L   <idx> */

#ifdef VM_DEBUG
	    fprintf(stderr, "SET_SYM_L %d\n", GET_L(p));
#endif

	    fts_set_symbol(eval_tos, symbol_table[GET_L(p)]);
	    p += 4;
	  }
	break;

	case FVM_POP_ARGS_B:
	  {
	    /* POP_ARGS_B    <int> */

#ifdef VM_DEBUG
	    fprintf(stderr, "POP_ARGS_B %d\n", GET_B(p));
#endif

	    eval_tos += GET_B(p);
	    p++;
	  }
	break;

	case FVM_POP_ARGS_S:
	  {
	    /* POP_ARGS_S    <int> */

#ifdef VM_DEBUG
	    fprintf(stderr, "POP_ARGS_S %d\n", GET_S(p));
#endif

	    eval_tos += GET_S(p);
	    p += 2;
	  }
	break;

	case FVM_POP_ARGS_L:
	  {
	    /* POP_ARGS_L    <int> */

#ifdef VM_DEBUG
	    fprintf(stderr, "POP_ARGS_L %d\n", GET_L(p));
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
	    fprintf(stderr, "PUSH_OBJ_B %d\n", GET_B(p));
#endif

	    object_tos--;
	    *object_tos =  object_table[GET_B(p)];
	    p += 1;
	  }
	break;

	case FVM_PUSH_OBJ_S:
	  {
	    /* PUSH_OBJ_S   <objidx> */

#ifdef VM_DEBUG
	    fprintf(stderr, "PUSH_OBJ_S %d\n", GET_S(p));
#endif

	    object_tos--;
	    *object_tos =  object_table[GET_S(p)];
	    p += 2;
	  }
	break;

	case FVM_PUSH_OBJ_L:
	  {
	    /* PUSH_OBJ_L   <objidx> */

#ifdef VM_DEBUG
	    fprintf(stderr, "PUSH_OBJ_L %d\n", GET_L(p));
#endif

	    object_tos--;
	    *object_tos =  object_table[GET_L(p)];
	    p += 4;
	  }
	break;

	case FVM_MV_OBJ_B:
	  {
	    /* MV_OBJ_B     <objidx> */

#ifdef VM_DEBUG
	    fprintf(stderr, "MV_OBJ_B %d\n", GET_B(p));
#endif

	    object_table[GET_B(p)] = *object_tos;
	    p += 1;
	  }
	break;

	case FVM_MV_OBJ_S:
	  {
	    /* MV_OBJ_S     <objidx> */

#ifdef VM_DEBUG
	    fprintf(stderr, "MV_OBJ_S %d\n", GET_S(p));
#endif

	    object_table[GET_S(p)] = *object_tos;
	    p += 2;
	  }
	break;

	case FVM_MV_OBJ_L:
	  {
	    /* MV_OBJ_L     <objidx> */

#ifdef VM_DEBUG
	    fprintf(stderr, "MV_OBJ_L %d\n", GET_L(p));
#endif

	    object_table[GET_L(p)] = *object_tos;
	    p += 4;
	  }
	break;


	case FVM_POP_OBJS_B:
	  {
	    /* POP_OBJS_B    <int> */

#ifdef VM_DEBUG
	    fprintf(stderr, "POP_OBJ_B %d\n", GET_B(p));
#endif

	    object_tos += GET_B(p);
	    p += 1;
	  }
	break;

	case FVM_POP_OBJS_S:
	  {
	    /* POP_OBJS_S    <int> */

#ifdef VM_DEBUG
	    fprintf(stderr, "POP_OBJ_S %d\n", GET_S(p));
#endif

	    object_tos += GET_S(p);
	    p += 2;
	  }
	break;


	case FVM_POP_OBJS_L:
	  {
	    /* POP_OBJS_L    <int> */

#ifdef VM_DEBUG
	    fprintf(stderr, "POP_OBJ_L %d\n", GET_L(p));
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
	    fprintf(stderr, "MAKE_OBJ_B %d\n", nargs);
#endif

	    new  = fts_object_new((fts_patcher_t *) (*object_tos), nargs, eval_tos);

	    /* Push the object in the object stack */

	    object_tos--;
	    *object_tos = new;
	    p += 1;
	  }
	  break;	

	case FVM_MAKE_OBJ_S:
	  {
	    /* MAKE_OBJ_S   <nargs> */

	    fts_object_t *new;
	    int nargs = GET_S(p);

#ifdef VM_DEBUG
	    fprintf(stderr, "MAKE_OBJ_S %d\n", nargs);
#endif

	    new  = fts_object_new((fts_patcher_t *) (*object_tos), nargs, eval_tos);

	    /* Push the object in the object stack */

	    object_tos--;
	    *object_tos = new;
	    p += 2;
	  }
	  break;	


	case FVM_MAKE_OBJ_L:
	  {
	    /* MAKE_OBJ_L   <nargs> */

	    fts_object_t *new;
	    int nargs = GET_L(p);

#ifdef VM_DEBUG
	    fprintf(stderr, "MAKE_OBJ_L %d\n", nargs);
#endif

	    new  = fts_object_new((fts_patcher_t *) (*object_tos), nargs, eval_tos);

	    /* Push the object in the object stack */

	    object_tos--;
	    *object_tos = new;
	    p += 4;
	  }
	  break;	

	case FVM_MAKE_TOP_OBJ_B:
	  {
	    /* MAKE_TOP_OBJ_B   <nargs> */

	    fts_object_t *new;
	    int nargs = GET_B(p);

#ifdef VM_DEBUG
	    fprintf(stderr, "MAKE_TOP_OBJ_B %d\n", nargs);
#endif

	    new  = fts_object_new((fts_patcher_t *) (*object_tos), nargs + lambda, eval_tos);

	    /* Push the object in the object stack */

	    object_tos--;
	    *object_tos = new;
	    p += 1;
	  }
	  break;	

	case FVM_MAKE_TOP_OBJ_S:
	  {
	    /* MAKE_TOP_OBJ_S   <nargs> */

	    fts_object_t *new;
	    int nargs = GET_S(p);

#ifdef VM_DEBUG
	    fprintf(stderr, "MAKE_TOP_OBJ_S %d\n", nargs);
#endif

	    new  = fts_object_new((fts_patcher_t *) (*object_tos), nargs + lambda, eval_tos);

	    /* Push the object in the object stack */

	    object_tos--;
	    *object_tos = new;
	    p += 2;
	  }
	  break;	


	case FVM_MAKE_TOP_OBJ_L:
	  {
	    /* MAKE_TOP_OBJ_L   <nargs> */

	    fts_object_t *new;
	    int nargs = GET_L(p);

#ifdef VM_DEBUG
	    fprintf(stderr, "MAKE_OBJ_L %d\n", nargs);
#endif

	    new  = fts_object_new((fts_patcher_t *) (*object_tos), nargs + lambda, eval_tos);

	    /* Push the object in the object stack */

	    object_tos--;
	    *object_tos = new;
	    p += 4;
	  }
	  break;	

	case FVM_PUT_PROP_B:
	  {
	    /* PUT_PROP_B_   <sym> */

	    fts_symbol_t prop;

	    prop = symbol_table[GET_B(p)];

#ifdef VM_DEBUG
	    fprintf(stderr, "PUT_PROP_B %s\n", fts_symbol_name(prop));
#endif

	    fts_object_put_prop(*object_tos, prop, eval_tos);
	    p += 1;
	  }
	break;

	case FVM_PUT_PROP_S:
	  {
	    /* PUT_PROP_S_   <sym> */

	    fts_symbol_t prop;

	    prop = symbol_table[GET_S(p)];

#ifdef VM_DEBUG
	    fprintf(stderr, "PUT_PROP_S %s\n", fts_symbol_name(prop));
#endif

	    fts_object_put_prop(*object_tos, prop, eval_tos);
	    p += 2;
	  }
	break;

	case FVM_PUT_PROP_L:
	  {
	    /* PUT_PROP_L_   <sym> */

	    fts_symbol_t prop;

	    prop = symbol_table[GET_L(p)];

#ifdef VM_DEBUG
	    fprintf(stderr, "PUT_PROP_S %s\n", fts_symbol_name(prop));
#endif

	    fts_object_put_prop(*object_tos, prop, eval_tos);
	    p += 4;
	  }
	break;

	case FVM_PUT_BUILTIN_PROP:
	  {
	    /* PUT_BUILTIN_PROP   <sym> */

	    fts_symbol_t prop;

	    prop = fts_get_builtin_symbol(GET_B(p++));

#ifdef VM_DEBUG
	    fprintf(stderr, "PUT_BUILTIN_PROP %s\n", fts_symbol_name(prop));
#endif

	    fts_object_put_prop(*object_tos, prop, eval_tos);
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
	    nargs = GET_L(p++);
	    p += 4;

#ifdef VM_DEBUG
	    fprintf(stderr, "OBJ_MESS %d %s %d\n", inlet, fts_symbol_name(sel), nargs);
#endif

	    fts_send_message(*object_tos, inlet, sel, nargs, eval_tos);
	  }
	break;

	case FVM_OBJ_BUILTIN_MESS:
	  {
	    /* OBJ_BUILTIN_MESS   <inlet> <sel> <nargs> */

	    int inlet;
	    fts_symbol_t sel;
	    int nargs;

	    inlet = GET_L(p);
	    p += 4;
	    sel = fts_get_builtin_symbol(GET_B(p));
	    p += 1;
	    nargs = GET_L(p);
	    p += 4;

#ifdef VM_DEBUG
	    fprintf(stderr, "OBJ_BUILTIN_MESS %d %s %d\n", inlet, fts_symbol_name(sel), nargs);
#endif

	    fts_send_message(*object_tos, inlet, sel, nargs, eval_tos);
	  }
	break;

	case FVM_CONNECT:
	  {
	    /* CONNECT */

#ifdef VM_DEBUG
	    fprintf(stderr, "CONNECT\n");
#endif
	    fts_object_connect(FTS_NO_ID, *object_tos, fts_get_int(eval_tos),
			       *(object_tos + 1), fts_get_int((eval_tos + 1)));
	  }
	break;

	/* Object table stack */

	case FVM_PUSH_OBJ_TABLE_B:
	  {
	    /* PUSH_OBJ_TABLE_B <int> */

	    int size;
#ifdef VM_DEBUG
	    fprintf(stderr, "PUSH_OBJ_TABLE_B %d\n", GET_B(p));
#endif

	    size = GET_B(p);
	    p += 1;

	    object_table = fts_malloc(sizeof(fts_object_t *) * size);

	    object_table_tos--;
	    (* object_table_tos) = object_table;
	  }
	break;

	case FVM_PUSH_OBJ_TABLE_S:
	  {
	    /* PUSH_OBJ_TABLE_S <int> */

	    int size;
#ifdef VM_DEBUG
	    fprintf(stderr, "PUSH_OBJ_TABLE_S %d\n", GET_S(p));
#endif

	    size = GET_S(p);
	    p += 2;

	    object_table = fts_malloc(sizeof(fts_object_t *) * size);

	    object_table_tos--;
	    (* object_table_tos) = object_table;
	  }
	break;



	case FVM_PUSH_OBJ_TABLE_L:
	  {
	    /* PUSH_OBJ_TABLE_L <int> */

	    int size;
#ifdef VM_DEBUG
	    fprintf(stderr, "PUSH_OBJ_TABLE_L %d\n", GET_L(p));
#endif

	    size = GET_L(p);
	    p += 1;

	    object_table = fts_malloc(sizeof(fts_object_t *) * size);

	    object_table_tos--;
	    (* object_table_tos) = object_table;
	  }
	break;

	case FVM_POP_OBJ_TABLE:
	  {
	    /* POP_OBJ_TABLE */

#ifdef VM_DEBUG
	    fprintf(stderr, "POP_OBJ_TABLE\n");
#endif

	    fts_free(object_table);
	    object_table_tos++;
	    object_table = *object_table_tos;
	  }
	break;

	case FVM_RETURN:
	  {
	    /* RETURN */

#ifdef VM_DEBUG
	    fprintf(stderr, "RETURN\n");
#endif

	    if (object_tos == object_stack + OBJECT_STACK_DEPTH)
	      return 0;
	    else
	      return *object_tos;
	  }
	}

      CHECK_OBJ_STACK;
      CHECK_EVAL_STACK;
    }
}
















