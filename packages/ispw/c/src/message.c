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
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

/* 
   New implementation of the messbox.

   It is based on the atomlist (a clearer, faster, and more (?)
   memory hungry implementation of the old binbuf concept).

   MDC
*/

#include <fts/fts.h>
#include "naming.h"

#define DEFAULT_DURATION 125.0f

typedef struct {
  fts_object_t o;
  fts_atom_list_t *atom_list;
  fts_alarm_t alarm;
  int value;
} messbox_t;

/************************************************************
 *
 *  utils
 *
 */

/* 
   atom list eval (the eval function is local because not of general utility)

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

static void init_eval(void)
{
  ev_s_star  = fts_new_symbol("*");
}

/*
   Atom stack: this module can become pubblic, if needed;
   it provide stack frame based allocation of atoms arrays.

   No overflow checks are done, but a check macro is provided.
*/

#define ATOM_STACK_SIZE (1024 * 16)

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
static void fts_eval_atom_list(messbox_t *this, fts_atom_list_t *list, int env_ac, const fts_atom_t *env_at,
		   fts_object_t *default_dst, int outlet)
{
  /* reader command and status */

  enum {rd_idle, rd_read_list, rd_read_env} rd_command;

  fts_atom_list_iterator_t *iter; /* iterator to read the list */

  int rd_env_count = 0;	/* counter for reading the env  */

  const fts_atom_t *rd_out = 0;	/* reader output; NULL if end of the list or end of the env */

  /* lexical machine status */

  enum {lex_list_read, lex_eval, lex_quoting, lex_env_read} lex_status; /* the lex machine status */

  enum {lex_type_void, lex_type_value, lex_type_comma, lex_type_semi, lex_type_end, lex_type_error} lex_out_type = lex_type_void;
  const fts_atom_t *lex_out_value = 0;

  /* eval machine status, including message under construction */

  enum {ev_get_dst, ev_get_first_arg, ev_get_args, ev_got_first, ev_end} ev_status; /* the ev machine  */

  int ev_dest_is_object = 0;
  fts_atom_t *ev_fp;		/* frame pointer */
  int ev_argc;
  fts_symbol_t ev_sym = 0;	/* the message symbol */
  fts_object_t *target = 0;

  /* LOCAL MACRO for the evaluation engine */

#define SEND_MESSAGE \
  if (ev_dest_is_object) \
    { if(target) fts_message_send((fts_object_t *)target, 0, ev_sym, ev_argc, ev_fp); } \
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
		  post("messbox: invalid $ argument\n"); /*ERROR: should be an event ? */
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
		  lex_out_value = fts_variable_get_value(((fts_object_t *)this)->patcher, fts_get_symbol(rd_out));

		  if(!fts_is_void(lex_out_value))
		    {
		      lex_out_type = lex_type_value;
		      lex_status = lex_list_read;
		    }
		  else
		    lex_out_type = lex_type_error;	
		}
	      else
		lex_out_type = lex_type_error;
	      
	      if(lex_out_type == lex_type_error)
		{
		  fts_symbol_t var_name = fts_get_symbol(rd_out);
		  
		  post("messbox: undefined variable %s\n", fts_symbol_name(var_name)); /*ERROR: should be an event ? */
		}
	    }
	  else
	    {
	      post("messbox: syntax error after $\n"); /*ERROR: should be an event ? */
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
		  post("messbox: invalid void message destination after a ;\n");
		  break;
		case lex_type_value:
		  if (fts_is_symbol(lex_out_value))
		    {
		      fts_symbol_t target_name = fts_get_symbol(lex_out_value);

		      ev_dest_is_object = 1;
		      ev_status = ev_get_first_arg;

		      target = ispw_get_target(fts_object_get_patcher((fts_object_t *)this), target_name);

		      if (!target)
			post("messbox: invalid message destination \"%s\"\n", fts_symbol_name(target_name));
		    }
		  else
		    {
		      post("messbox: invalid message destination\n");
		      ev_status = ev_end;
		    }
		  break;

		case lex_type_comma:
		  post("messbox: invalid message destination \",\"\n");
		  ev_status = ev_end;
		  break;

		case lex_type_semi:
		  post("messbox: invalid message destination  \";\"\n");
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
		  else
		    {
		      ev_sym = fts_get_selector(lex_out_value);
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

static int messbox_list_is_primitive(int ac, const fts_atom_t *at)
{
  int i;

  for(i=0; i<ac; i++)
    {
      if(fts_is_object(at + i))
	{
	  post("messbox: can't set value of type <%s>\n", fts_symbol_name(fts_get_selector(at + i)));
	  return 0;
	}
    }

  return 1;
}

static void messbox_update(fts_object_t *o)
{
  messbox_t *this = (messbox_t *) o;

  fts_client_send_message_from_atom_list(o, fts_s_set, this->atom_list);
}

/************************************************************
 *
 *  tick
 *
 */

static void messbox_tick(fts_alarm_t *alarm, void *calldata)
{
  messbox_t *this = (messbox_t *)calldata;

  this->value = 0;
  fts_object_ui_property_changed((fts_object_t *)this, fts_s_value);
}


/************************************************************
 *
 *  system methods
 *
 */

static void messbox_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messbox_t *this = (messbox_t *) o;

  this->atom_list = fts_atom_list_new();
  fts_alarm_init(&(this->alarm), 0, messbox_tick, this);
  this->value = 0;
}


static void messbox_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messbox_t *this = (messbox_t *) o;

  fts_atom_list_free(this->atom_list);
  fts_alarm_reset(&(this->alarm));
}


static void messbox_upload(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messbox_update(o);
}

static void messbox_set_noupdate(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messbox_t *this = (messbox_t *) o;
  
  if(messbox_list_is_primitive(ac, at))
    fts_atom_list_set(this->atom_list, ac, at);
}

static void messbox_append_noupdate(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messbox_t *this = (messbox_t *) o;

  if(messbox_list_is_primitive(ac, at))
    fts_atom_list_append(this->atom_list, ac, at);
}


static void messbox_clear_noupdate(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messbox_t *this = (messbox_t *) o;

  fts_atom_list_clear(this->atom_list);
}


static void messbox_save_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messbox_t *this = (messbox_t *)o;
  fts_bmax_file_t *f = (fts_bmax_file_t *) fts_get_ptr(at);

  fts_atom_list_save_bmax(this->atom_list, f, (fts_object_t *) this);
}


static void messbox_save_dotpat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messbox_t *this = (messbox_t *) o;
  FILE *file;
  int x, y, w, font_index;
  fts_atom_t a;
  fts_atom_list_iterator_t *iterator;
  int state;

  file = (FILE *)fts_get_ptr( at);

  fts_object_get_prop( o, fts_s_x, &a);
  x = fts_get_int( &a);
  fts_object_get_prop( o, fts_s_y, &a);
  y = fts_get_int( &a);
  fts_object_get_prop( o, fts_s_width, &a);
  w = fts_get_int( &a);
  font_index = 1;

  fprintf( file, "#P message %d %d %d %d", x, y, w, font_index);

  iterator = fts_atom_list_iterator_new( this->atom_list);

  state = 0;

  while (! fts_atom_list_iterator_end( iterator))
    {
      fts_atom_t *p = fts_atom_list_iterator_current( iterator);

      switch ( state ) {

      case 0:
	if ( fts_is_int( p))
	  fprintf( file, " %d", fts_get_int( p));
	else if ( fts_is_float( p) )
	  fprintf( file, " %f", fts_get_float( p));
	else if ( fts_is_symbol( p) )
	  {
	    fts_symbol_t s = fts_get_symbol( p);

	    if (s == fts_s_semi || s == fts_s_comma)
	      fprintf( file, " \\%s", fts_symbol_name( s));
	    else if (s == fts_s_dollar)
	      state = 1;
	    else
	      fprintf( file, " %s", fts_symbol_name( s));
	  }

	break;

      case 1:
	state = 0;

	if (fts_is_int( p))
	  fprintf( file, " \\$%d", fts_get_int( p));
	else if ( fts_is_float( p) )
	  fprintf( file, " $ %f", fts_get_float( p));
	else if ( fts_is_symbol( p) )
	  {
	    fts_symbol_t s = fts_get_symbol( p);

	    if (s == fts_s_semi || s == fts_s_comma)
	      fprintf( file, " $ \\%s", fts_symbol_name( s));
	    else
	      fprintf( file, " $ %s", fts_symbol_name( s));
	    }

	break;
      }

      fts_atom_list_iterator_next( iterator);
    }

  fts_atom_list_iterator_free( iterator);

  fprintf( file, ";\n");
}

static void messbox_find(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messbox_t *this = (messbox_t *) o;
  fts_object_set_t *set = (fts_object_set_t *)fts_get_data(at);

  if (fts_atom_list_is_subsequence(this->atom_list, ac - 1, at + 1))
    fts_object_set_add(set, o);
}

/************************************************************
 *
 *  user methods
 *
 */

/* message_eval is installed for bang, int, float and list.
   It is not installed for anything, because the semantic
   of message do not allow other messages to trigger the
   evalution of the box content.
*/

static void messbox_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messbox_t *this = (messbox_t *) o;

  if(messbox_list_is_primitive(ac, at))
    fts_atom_list_set(this->atom_list, ac, at);

  if (fts_object_patcher_is_open((fts_object_t *) this))
    messbox_update(o);
}

static void messbox_append(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messbox_t *this = (messbox_t *) o;

  if(messbox_list_is_primitive(ac, at))
    fts_atom_list_append(this->atom_list, ac, at);

  if (fts_object_patcher_is_open((fts_object_t *) this))
    messbox_update(o);
}

static void messbox_eval_and_update(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{  
  messbox_t *this = (messbox_t *) o;

  this->value = 1;
  fts_object_ui_property_changed(o, fts_s_value);

  fts_alarm_set_delay(&(this->alarm), DEFAULT_DURATION);

  fts_eval_atom_list(this, this->atom_list, ac, at, o, 0);
}

static void messbox_eval(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{  
  messbox_t *this = (messbox_t *) o;

  fts_eval_atom_list(this, this->atom_list, ac, at, o, 0);
}

/************************************************************
 *
 *  class
 *
 */

/* daemon to get the "value" property: the value property is set to 
 * one to actually bang the message, and reset to zero after a timer elapse;
 * it is used to make the message box flash like the messages; java is not
 * reialable for this kind of real time flashing.
 */
static void messbox_get_value(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  messbox_t *this = (messbox_t *)obj;

  fts_set_int(value, this->value);
}


static fts_status_t messbox_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(messbox_t), 1, 1, 0);

  a[0] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, messbox_init, 1, a);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, messbox_delete, 0, 0);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, messbox_set);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_find, messbox_find);
  fts_method_define(cl, fts_SystemInlet, fts_s_bang, messbox_eval_and_update, 0, 0);

  /* Atom list saving/loading/update support */

  fts_method_define(cl, fts_SystemInlet, fts_s_upload, messbox_upload, 0, 0);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_append,  messbox_append_noupdate);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_clear,  messbox_clear_noupdate);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_save_bmax, messbox_save_bmax, 1, a);

  a[0] = fts_s_ptr;
  fts_method_define( cl, fts_SystemInlet, fts_s_save_dotpat, messbox_save_dotpat, 1, a); 

  /* Application methods */

  fts_method_define(cl, 0, fts_s_bang, messbox_eval, 0, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, messbox_eval, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, messbox_eval, 1, a);

  a[0] = fts_s_symbol;
  fts_method_define(cl, 0, fts_s_symbol, messbox_eval, 1, a);

  fts_method_define_varargs(cl, 0, fts_s_list, messbox_eval);

  fts_method_define_varargs(cl, 0, fts_s_set, messbox_set);

  fts_method_define_varargs(cl, 0, fts_s_append,  messbox_append);

  /* value daemons */

  fts_class_add_daemon(cl, obj_property_get, fts_s_value, messbox_get_value);

  return fts_Success;
}

void messbox_config(void)
{
  init_eval();

  fts_class_install(fts_new_symbol("messbox"), messbox_instantiate);
}
