/* An experiment to load binary files.

   Virtual Machine:

   1- An evaluation stack
   2- An object stack
   3- A local symbol table.
   4- A local object table stack.
   5- A local object table.

   Format and Instructions:

   All the instructions are a fts_word_t long ,
   and may be followed by one or more arguments;
   The instructions are:

   PUSH_INT   <int>   // push an int value in the argument stack
   PUSH_FLOAT <float> // push a float value in the argument stack
   PUSH_SYM   <int>   // push a symbol (by idx) value in the argument stack
   POP_ARG    <int>   // pop n values  from the argument stack

   PUSH_OBJ   <int>   // Push an object table entry in the object stack
   MV_OBJ     <objidx>    // Copy the top of object stack to an object table entry
   POP_OBJ    <int>   // Push n objects from the object stack
   

   MAKE_OBJ   <nargs>   // Take <nargs> argument from the evaluation stack
                       // and make an object in the patcher top of the object stack.
		       // Push the new object in the object stack
		       // Don't touch the argument stack.

   PUT_PROP   <sym> // Put the top of the evaluation stack value
                    // as property <sym> for the object at the top 
		    // of the object stack
		    // don't touch either stack.

   OBJ_MESS   <inlet> <sel> <nargs>  // Send a message <sel> to the inlet <in> object top of the object stack
                         // with <nargs> arguments from the evaluation stack
			 // don't touch the stacks

   PUSH_OBJ_TABLE <int> // Push an object table of N values
   POP_OBJ_TABLE        // Pop an object table

   CONNECT            // connect outlet (Top of evaluation stack) of object (top of obejct stack)
                      // with inlet (Top - 1 of evaluation stack) of object (top - 1 of obejct stack)

   RETURN            // Return from the current VM execution
   */

#include "sys.h"
#include "lang/mess.h"
#include "lang/mess/vm.h"


#define EVAL_STACK_DEPTH   1024
#define OBJECT_STACK_DEPTH 1024
#define OBJECT_TABLE_STACK_DEPTH 1024

static fts_atom_t eval_stack[EVAL_STACK_DEPTH];
static fts_atom_t *eval_tos;

static fts_object_t *object_stack[OBJECT_STACK_DEPTH];
static fts_object_t **object_tos;

static fts_object_t **object_table_stack[OBJECT_TABLE_STACK_DEPTH];
static fts_object_t ***object_table_tos;
static fts_object_t **object_table;


void fts_run_mess_vm(fts_word_t *program, fts_symbol_t symbol_table[])
{

  int cmd;
  fts_word_t *p;

  /* initialize the vm; this part of the code is not recursive, of course ! */

  p = program;
  eval_tos = eval_stack + EVAL_STACK_DEPTH; /* point to the actually filled cell */
  object_tos = object_stack + OBJECT_STACK_DEPTH;
  object_table_tos = object_table_stack + OBJECT_TABLE_STACK_DEPTH;
  object_table = 0;

  while (1)
    {
      cmd = fts_word_get_int(p++);

      switch (cmd)
	{
	  /* Eval stack manipulation */

	case PUSH_INT:
	  {
	    /* PUSH_INT   <int> */

	    eval_tos--;
	    fts_set_int(eval_tos, fts_word_get_int(p));
	    p++;
	  }
	break;

	case PUSH_FLOAT:
	  {
	    /* PUSH_FLOAT <float> */
	    
	    eval_tos--;
	    fts_set_float(eval_tos, fts_word_get_float(p));
	    p++;
	  }
	break;

	case PUSH_SYM:
	  {
	    /* PUSH_SYM   <idx> */

	    eval_tos--;
	    fts_set_symbol(eval_tos, symbol_table[fts_word_get_int(p)]);
	    p++;
	  }
	break;

	case POP_ARG:
	  {
	    /* POP_ARG    <int> */

	    eval_tos += fts_word_get_int(p);
	    p++;
	  }
	break;

	/* Object stack manipulation */

	case PUSH_OBJ:
	  {
	    /* PUSH_OBJ   <objidx> */

	    object_tos--;
	    *object_tos =  object_table[fts_word_get_int(p)];
	    p++;
	  }
	break;

	case MV_OBJ:
	  {
	    /* MV_OBJ     <objidx> */

	    object_table[fts_word_get_int(p)] = *object_tos;
	  }
	break;


	case POP_OBJ:
	  {
	    /* POP_OBJ    <int> */

	    object_tos += fts_word_get_int(p);
	    p++;
	  }
	break;

	/* Object Manipulatation */

	case MAKE_OBJ:
	  {
	    /* MAKE_OBJ   <nargs> */

	    fts_object_t *new;
	    int nargs = fts_word_get_int(p++);

	    new  = fts_object_new((fts_patcher_t *) (*object_tos), FTS_NO_ID, nargs, eval_tos);

	    /* Push the object in the object stack */

	    object_tos--;
	    *object_tos = new;
	  }
	  break;	

	case PUT_PROP:
	  {
	    /* PUT_PROP   <sym> */

	    fts_symbol_t prop = symbol_table[fts_word_get_int(p++)];
	    fts_object_put_prop(*object_tos, prop, eval_tos);
	  }
	break;

	case OBJ_MESS:
	  {
	    /* OBJ_MESS   <inlet> <sel> <nargs> */

	    int inlet = fts_word_get_int(p++);
	    fts_symbol_t sel = symbol_table[fts_word_get_int(p++)];
	    int nargs = fts_word_get_int(p++);

	    fts_message_send(*object_tos, inlet, sel, nargs, eval_tos);
	  }
	break;

	case CONNECT:
	  {
	    /* CONNECT */

	    fts_object_connect(*object_tos, fts_get_int(eval_tos), *(object_tos + 1), fts_get_int((eval_tos + 1)));
	  }
	break;

	/* Object table stack */

	case PUSH_OBJ_TABLE:
	  {
	    /* PUSH_OBJ_TABLE <int> */

	    int size;

	    size = fts_word_get_int(p);
	    p++;

	    object_table = fts_malloc(sizeof(fts_object_t *) * size);
	    object_table[0] = 0;

	    object_table_tos--;
	    (* object_table_tos) = object_table;
	  }
	break;

	case POP_OBJ_TABLE:
	  {
	    /* POP_OBJ_TABLE */
	    fts_free(object_table);
	    object_table_tos++;
	    object_table = *object_table_tos;
	  }
	break;

	case RETURN:
	  return;
	}

    }
}


