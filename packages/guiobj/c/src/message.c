/* 
   New implementation of the message.

   It is based on the atomlist (a clearer, faster, and more (?)
   memory hungry implementation of the old binbuf concept).

   MDC
   */

#include "fts.h"

static void init_eval(void);

/********************* message ************************/

typedef struct 
{
  fts_object_t o;
  fts_atom_list_t *atom_list;
} message_t;

static void fts_eval_atom_list(message_t *this,
			       fts_atom_list_t *list,
			       int env_ac, const fts_atom_t *env_at,
			       fts_object_t *default_dst, int outlet);

static void
message_update(fts_object_t *o)
{
  message_t *this = (message_t *) o;
  fts_atom_list_iterator_t *iterator;

  iterator = fts_atom_list_iterator_new(this->atom_list);

  fts_client_mess_start_msg(CLIENTMESS_CODE);
  fts_client_mess_add_object((fts_object_t *)this);

  while (! fts_atom_list_iterator_end(iterator))
    {
      fts_client_mess_add_atoms(1, fts_atom_list_iterator_current(iterator));
      fts_atom_list_iterator_next(iterator);
    }

  fts_client_mess_send_msg();
  fts_atom_list_iterator_free(iterator);
}

static void
message_upload(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  message_update(o);
}


/* message_eval is installed for bang, int, float and list.
   It is not installed for anything, because the semantic
   of message do not allow other messages to trigger the
   evalution of the box content.
*/

static void
message_eval(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{  
  message_t *this = (message_t *) o;

  fts_eval_atom_list(this, this->atom_list, ac, at, o, 0);
}

static void
message_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  message_t *this = (message_t *) o;

  fts_atom_list_set(this->atom_list, ac, at);

  if (fts_object_patcher_is_open((fts_object_t *) this))
    message_update(o);
}

/* Set without update, for the editing */

static void
message__set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  message_t *this = (message_t *) o;

  fts_atom_list_set(this->atom_list, ac, at);
}

static void
message_append(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  message_t *this = (message_t *) o;

  fts_atom_list_append(this->atom_list, ac, at);

  if (fts_object_patcher_is_open((fts_object_t *) this))
    message_update(o);
}

static void
message_append_noupdate(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  message_t *this = (message_t *) o;

  fts_atom_list_append(this->atom_list, ac, at);
}


static void
message_clear_noupdate(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  message_t *this = (message_t *) o;

  fts_atom_list_clear(this->atom_list);
}


static void
message_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  message_t *this = (message_t *) o;

  this->atom_list = fts_atom_list_new();
}


static void
message_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  message_t *this = (message_t *) o;

  fts_atom_list_free(this->atom_list);
}


static void
message_save_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  message_t *this = (message_t *)o;
  fts_bmax_file_t *f = (fts_bmax_file_t *) fts_get_ptr(at);

  fts_atom_list_save_bmax(this->atom_list, f, (fts_object_t *) this);
}


static void
message_find(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  message_t *this = (message_t *) o;
  fts_object_set_t *set = (fts_object_set_t *)fts_get_data(at);

  if (fts_atom_list_is_subsequence(this->atom_list, ac - 1, at + 1))
    fts_object_set_add(set, o);
}



static fts_status_t
message_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(message_t), 1, 1, 0);

  a[0] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, message_init, 1, a);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, message_delete, 0, 0);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, message__set);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_find, message_find);

  /* Atom list saving/loading/update support */

  fts_method_define(cl, fts_SystemInlet, fts_s_upload, message_upload, 0, 0);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_append,  message_append_noupdate);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_clear,  message_clear_noupdate);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_save_bmax, message_save_bmax, 1, a);

  /* Application methods */

  fts_method_define(cl, 0, fts_s_bang, message_eval, 0, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, message_eval, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, message_eval, 1, a);

  a[0] = fts_s_symbol;
  fts_method_define(cl, 0, fts_s_symbol, message_eval, 1, a);


  fts_method_define_varargs(cl, 0, fts_s_list, message_eval);

  fts_method_define_varargs(cl, 0, fts_s_set, message_set);

  fts_method_define_varargs(cl, 0, fts_s_append,  message_append);

  return fts_Success;
}


void
message_config(void)
{
  init_eval();

  fts_metaclass_create(fts_new_symbol("messbox"), message_instantiate, fts_always_equiv);
}


/*********************************************************************/
/*                                                                   */
/*      The eval function: local because not of general utility      */
/*                                                                   */
/*********************************************************************/


/* 
   atom list eval.

   Inspired to the binbuf eval, but with a clean structure.

   The evaluation rule is the following:

   A list of symbols "'" followed by any atom A evalute to a list
   of "'" shorted by one, followed by the atom A (crazy rule for backward
   compatibility).

   The symbol "$" followed by a long N evaluate to the Nth atom in the env.
   >> NO: Followed by a name evaluate to the value of the value Object with the same name.
   Followed by "*" evalute to the whole list of argument (substituted in place).

   The symbol ";" followed by a symbol S set to S the destination of the next message computed.
   It also start a message, i.e. arguments to the message follow the destination.

   The symbol "," start a new message, with the same destination of the previous one.

   A long atom F, as the first and only argument of a message is converted to the atom "int" followed
   by the long atom F.

   A float atom F, as the first and only argument of a message is converted to the atom "float" followed
   by the float atom F.

   If atom A is the first argument of a message with more than one arguments is not a symbol,
   then it evaluate to the atom "list" followed by the atom A.

*/



/* init function: essentially, initialize some atoms */

static fts_symbol_t ev_s_star;

static void
init_eval(void)
{
  ev_s_star  = fts_new_symbol("*");
}

/*
   Atom stack: this module can become pubblic, if needed;
   it provide stack frame based allocation of atoms arrays.

   No overflow checks are done, but a check macro is provided.
*/

#define ATOM_STACK_SIZE (1024*16)

static fts_atom_t  atom_stack[ATOM_STACK_SIZE];
static fts_atom_t *atom_stack_pointer = &atom_stack[0];	/* next usable value */

#define GET_ATOM_STACK_FRAME(p)       ((p) = atom_stack_pointer)
#define RESTORE_ATOM_STACK_FRAME(p)   (atom_stack_pointer = (p))
#define PUSH_ATOM(a)                  (*(atom_stack_pointer++) = *a)
#define STACK_GOT_ROOM(n)             ((atom_stack_pointer + n) <(atom_stack + ATOM_STACK_SIZE))

/* evaluator:
   the API is quite  dependent from the message box; it should probabily be more
   generic (in particular, we should be able to specify also an object
   with its inlet, not only the outlet.


   The evaluator is written as two finite state machine in pipeline;
   the first cope with quoting and variable evaluation ('\' and '$'
   operators), the other with the message creation and routing (',' and
   ';' operators.

   The first do a kind of lexical analys, and produce a lexical
   token that is eaten by the second; in a given cicle, the machine
   can decide to don't produce a token, or also to don't read the
   input (in particular, for the implementation of the $* construct).

   The second do the actual evaluation; so they are called lexical
   machine (somehow improperly) and eval machine; all the relative variables
   and enums constant use the ev_ and lex_ prefix.

   To cope with this, the first finite state machine is driven by a
   "reader engine" controlled by specific opcode.
   
 */

static void
fts_eval_atom_list(message_t *this, fts_atom_list_t *list, int env_ac, const fts_atom_t *env_at,
		   fts_object_t *default_dst, int outlet)
{
  /* reader command and status */

  enum {rd_idle, rd_read_list, rd_read_env} rd_command;

  fts_atom_list_iterator_t *iter; /* iterator to read the list */

  int       rd_env_count;	/* counter for reading the env  */

  const fts_atom_t *rd_out;	/* reader output; NULL if end of the list or end of the
				   env */

  /* lexical machine status */

  enum {lex_list_read, lex_eval, lex_quoting, lex_env_read} lex_status; /* the lex machine status */

  enum {lex_type_void, lex_type_value, lex_type_comma, lex_type_semi, lex_type_end, lex_type_error} lex_out_type; 
  const fts_atom_t *lex_out_value;

  /* eval machine status, including message under construction */

  enum {ev_get_dst, ev_get_first_arg, ev_get_args, ev_got_first, ev_end} ev_status; /* the ev machine  */

  fts_symbol_t ev_dest_name = 0;
  int ev_dest_is_name = 0;
  fts_atom_t   *ev_fp;		/* frame pointer */
  int           ev_argc;
  fts_symbol_t ev_sym;	/* the message symbol */

  /* LOCAL MACRO for the evaluation engine */

#define SEND_MESSAGE \
  if (ev_dest_is_name) \
    fts_named_object_send(ev_dest_name, ev_sym, ev_argc, ev_fp); \
  else \
    fts_outlet_send(default_dst, outlet, ev_sym, ev_argc, ev_fp);

  /* Init the reader */

  rd_command = rd_read_list;
  iter = fts_atom_list_iterator_new(list);

  /* Init the lexical machine */

  lex_status = lex_list_read;

  /* Init the eval machine */

  ev_status = ev_get_first_arg;
  GET_ATOM_STACK_FRAME(ev_fp);
  ev_argc = 0;

  /* loop thru the reader, lexical machine and
     eval machine until finished */

  while (ev_status != ev_end)
    {
      /* reader */

      switch(rd_command)
	{
	case rd_idle:
	  /* don't read anything */ 
	  rd_out = 0;
	  break;

	case rd_read_list:
	  /* read from the list: check the iterator,
	     and if not at the end, read the current value
	     and then increment the iterator.
	     */
	     
	  if (fts_atom_list_iterator_end(iter))
	    rd_out = 0;
	  else
	    {
	      rd_out = fts_atom_list_iterator_current(iter);
	      fts_atom_list_iterator_next(iter);
	    }
	  break;

	case rd_read_env:
	  if (rd_env_count < env_ac)
	    rd_out = &env_at[rd_env_count++];
	  else
	    rd_out = 0;
	  break;
	}

      /* lexical machine */

      switch (lex_status)
	{
	case lex_list_read:
	  if (! rd_out)
	    lex_out_type = lex_type_end;
	  else if (fts_is_symbol(rd_out))
	    {
	      if (fts_get_symbol(rd_out) == fts_s_dollar)
		{
		  lex_status = lex_eval;
		  lex_out_type = lex_type_void;
		}
	      else if (fts_get_symbol(rd_out) == fts_s_quote)
		{
		  lex_status = lex_quoting;
		  lex_out_type = lex_type_void;
		}
	      else if (fts_get_symbol(rd_out) == fts_s_comma)
		lex_out_type = lex_type_comma;
	      else if (fts_get_symbol(rd_out) == fts_s_semi)
		lex_out_type = lex_type_semi;
	      else
		{
		  lex_out_type  = lex_type_value;
		  lex_out_value = rd_out;
		}
	    }
	  else
	    {
	      lex_out_type  = lex_type_value;
	      lex_out_value = rd_out;
	    }
	  break;

	case lex_eval:
	  if (! rd_out)
	    lex_out_type = lex_type_end;
	  else if (fts_is_long(rd_out))
	    {
	      if (fts_get_long(rd_out) <= env_ac)
		{
		  lex_out_type = lex_type_value;
		  lex_out_value = &env_at[fts_get_long(rd_out) - 1];
		  lex_status = lex_list_read;
		}
	      else
		{
		  post("Invalid $ argument in message box\n"); /*ERROR: should be an event ? */
		  lex_out_type = lex_type_error;
		  lex_status = lex_list_read;
		}
	    }
	  else if (fts_is_symbol(rd_out))
	    {
	      if (fts_get_symbol(rd_out) == ev_s_star)
		{
		  lex_out_type = lex_type_void;
		  lex_status   = lex_env_read;

		  /* Set the reader to read from the beginning of the env */

		  rd_command   = rd_read_env;
		  rd_env_count = 0;
		}
	      else if (fts_variable_get_value(((fts_object_t *)this)->patcher, fts_get_symbol(rd_out)))
		{
		  lex_out_type = lex_type_value;
		  lex_out_value = fts_variable_get_value(((fts_object_t *)this)->patcher, fts_get_symbol(rd_out));
		  lex_status = lex_list_read;
		}
	      else
		{
		  post("Invalid symbol after a $ in message box\n"); /*ERROR: should be an event ? */
		  lex_out_type = lex_type_error;
		}
	    }
	  else
	    {
	      post("Invalid float value after a $ in message box\n"); /*ERROR: should be an event ? */
	      lex_out_type = lex_type_error;
	    }
	  break;

	case lex_quoting:
	  if (! rd_out)
	    {
	      lex_out_type = lex_type_end;
	      lex_out_value = rd_out;
	      lex_status = lex_list_read;
	    }
	  else if (fts_is_symbol(rd_out))
	    {
	      if (fts_get_symbol(rd_out) == fts_s_quote)
		lex_status = lex_quoting;
	      else
		lex_status = lex_list_read;

	      lex_out_value = rd_out;
	      lex_out_type = lex_type_value;
	    }
	  else
	    {
	      lex_out_value = rd_out;
	      lex_out_type = lex_type_value;
	      lex_status = lex_list_read;
	    }
	  break;

	case lex_env_read:
	  if (rd_out)
	    {
	      lex_out_type = lex_type_value;
	      lex_out_value = rd_out;
	      lex_status = lex_env_read;
	    }
	  else
	    {
	      lex_out_type = lex_type_void;
	      lex_status = lex_list_read;
	      rd_command = rd_read_list;
	    }
	  break;
	}

      /* eval machine: the eval machine is runned only
	 if the lex_out_type is not void*/

      if (lex_out_type == lex_type_error)
	ev_status = ev_end;
      else if (lex_out_type != lex_type_void)
	{
	  switch(ev_status)
	    {
	    case ev_get_dst:
	      switch (lex_out_type)
		{
		case lex_type_void:
		  post("Invalid void message destination after a ; in message box\n");
		  break;
		case lex_type_value:
		  if (fts_is_symbol(lex_out_value))
		    {
		      fts_symbol_t target_name = fts_get_symbol(lex_out_value);

		      ev_dest_is_name = 1;
		      ev_dest_name = target_name;
		      ev_status = ev_get_first_arg;

		      if (! fts_named_object_exists(target_name))
			post("Invalid message destination \"%s\" after a ; in message box\n",
			     fts_symbol_name(target_name));
		    }
		  else
		    {
		      post("Invalid message destination type after a ; in message box\n");
		      ev_status = ev_end;
		    }
		  break;

		case lex_type_comma:
		  post("Invalid comma as  message destination  after a ; in message box\n");
		  ev_status = ev_end;
		  break;

		case lex_type_semi:
		  post("Invalid semi colon as  message destination  after a ; in message box\n");
		  ev_status = ev_end;
		  break;
		case lex_type_end:
		  ev_status = ev_end; /* just quit, end of the job */
		  break;
		case lex_type_error:
		  /* this neve happen */
		  break;
		}
	      break;

	    case ev_get_first_arg:
	      switch(lex_out_type)
		{
		case lex_type_value:
		  if (fts_is_symbol(lex_out_value))
		    {
		      ev_sym = fts_get_symbol(lex_out_value);
		      ev_status = ev_get_args;
		    }
		  else if (fts_is_float(lex_out_value))
		    {
		      ev_sym = fts_s_float;
		      PUSH_ATOM(lex_out_value);
		      ev_argc++;
		      ev_status = ev_got_first;
		    }
		  else if (fts_is_long(lex_out_value))
		    {
		      ev_sym = fts_s_int;
		      PUSH_ATOM(lex_out_value);
		      ev_argc++;
		      ev_status = ev_got_first;
		    }
		  break;
		case lex_type_comma:
		  /* comma at the beginning of a message is skipped with no actions */
		  break;
		case lex_type_semi:
		  /* a semicolumn here is a message with a destination but without content.
		     The message is simply ignored */
		  ev_status = ev_get_dst;
		  break;
		case lex_type_end:
		  /* end of the atom_list before the message. just quit */
		  ev_status = ev_end;
		  break;
		default:
		  break;
		}
	      break;

	    case ev_get_args:
	      switch(lex_out_type)
		{
		case lex_type_value:
		  PUSH_ATOM(lex_out_value);
		  ev_argc++;
		  break;
		case lex_type_comma:
		  SEND_MESSAGE;
		  RESTORE_ATOM_STACK_FRAME(ev_fp);
		  ev_argc = 0;
		  ev_status = ev_get_first_arg;
		  break;
		case lex_type_semi:
		  SEND_MESSAGE;
		  RESTORE_ATOM_STACK_FRAME(ev_fp);
		  ev_argc = 0;
		  ev_status = ev_get_dst;
		  break;
		case lex_type_end:
		  SEND_MESSAGE;
		  ev_status = ev_end;
		  break;
		default:
		  break;
		}
	      break;

	    case ev_got_first:
	      switch(lex_out_type)
		{
		case lex_type_value:
		  PUSH_ATOM(lex_out_value);
		  ev_argc++;
		  ev_sym = fts_s_list;
		  ev_status = ev_get_args;
		  break;
		case lex_type_comma:
		  SEND_MESSAGE;
		  RESTORE_ATOM_STACK_FRAME(ev_fp);
		  ev_argc = 0;
		  ev_status = ev_get_first_arg;
		  break;
		case lex_type_semi:
		  SEND_MESSAGE;
		  RESTORE_ATOM_STACK_FRAME(ev_fp);
		  ev_argc = 0;
		  ev_status = ev_get_dst;
		  break;
		case lex_type_end:
		  SEND_MESSAGE;
		  ev_status = ev_end;
		  break;
		default:
		  break;
		}
	      break;
	    default:
	      break;
	    }
	}
    }

  RESTORE_ATOM_STACK_FRAME(ev_fp);
  fts_atom_list_iterator_free(iter);
}



