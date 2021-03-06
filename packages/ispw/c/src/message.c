/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

/* 
   New implementation of the messbox.

   It is based on the atomlist (a clearer, faster, and more (?)
   memory hungry implementation of the old binbuf concept).

   MDC
*/

#include <fts/fts.h>

#include <ftsconfig.h>

#if HAVE_ALLOCA_H
#include <alloca.h>
#endif

#include "naming.h"
#include "atomlist.h"

#define DEFAULT_DURATION 125.0f

typedef struct {
  fts_object_t o;
  fts_atom_list_t *atom_list;
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

static void
messbox_send(fts_object_t *o, int outlet, fts_object_t *target, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(s == fts_s_bang)
  {
    if(target)
      ispw_target_send(target, NULL, 0, NULL);
    else
      fts_outlet_bang(o, outlet);
  }
  else
  {
    if(target)
      ispw_target_send(target, s, ac, at);
    else
      fts_outlet_send(o, outlet, s, ac, at);
  }
}

static void
fts_eval_atom_list(messbox_t *this, fts_atom_list_t *list, int env_ac, const fts_atom_t *env_at, fts_object_t *default_dst, int outlet)
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

  fts_atom_t *ev_fp;		/* frame pointer */
  int ev_argc;
  fts_symbol_t ev_sym = 0;	/* the message symbol */
  fts_object_t *target = 0;

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
	  else if (fts_is_int(rd_out))
	    {
	      if (fts_get_int(rd_out) <= env_ac)
		{
		  lex_out_type = lex_type_value;
		  lex_out_value = &env_at[fts_get_int(rd_out) - 1];
		  lex_status = lex_list_read;
		}
	      else
		{
		  fts_post("messbox: invalid $ argument\n");
		  lex_out_type = lex_type_error;
		  lex_status = lex_list_read;
		}
	    }
	  else if (fts_is_symbol(rd_out))
	    {
	      if (fts_get_symbol(rd_out) == ev_s_star)
		{
		  lex_out_value = 0;
		  lex_out_type = lex_type_void;
		  lex_status = lex_env_read;

		  /* Set the reader to read from the beginning of the env */
		  rd_command = rd_read_env;
		  rd_env_count = 0;
		}
	      else
		{
		  lex_out_value = fts_name_get_value(fts_object_get_patcher((fts_object_t *)this), fts_get_symbol(rd_out));
		  
		  if(lex_out_value)
		    {
		      lex_out_type = lex_type_value;
		      lex_status = lex_list_read;
		    }
		  else
		    {
		      fts_post("messbox: undefined variable %s\n", fts_get_symbol(rd_out));
		      lex_out_type = lex_type_error;		  
		    }
		}
	    }
	  else
	    {
	      fts_post("messbox: syntax error after $\n");
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
		  fts_post("messbox: invalid void message destination after a ;\n");
		  break;
		case lex_type_value:
		  if (fts_is_symbol(lex_out_value))
		    {
		      fts_symbol_t target_name = fts_get_symbol(lex_out_value);

		      ev_status = ev_get_first_arg;

		      target = ispw_get_target(fts_object_get_patcher((fts_object_t *)this), target_name);

		      if (!target)
			fts_post("messbox: invalid message destination \"%s\"\n", target_name);
		    }
		  else
		    {
		      fts_post("messbox: invalid message destination\n");
		      ev_status = ev_end;
		    }
		  break;

		case lex_type_comma:
		  fts_post("messbox: invalid message destination \",\"\n");
		  ev_status = ev_end;
		  break;

		case lex_type_semi:
		  fts_post("messbox: invalid message destination  \";\"\n");
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
		      ev_sym = NULL;
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
		  messbox_send(default_dst, outlet, (fts_object_t *)target, ev_sym, ev_argc, ev_fp);
		  RESTORE_ATOM_STACK_FRAME(ev_fp);
		  ev_argc = 0;
		  ev_status = ev_get_first_arg;
		  break;
		case lex_type_semi:
		  messbox_send(default_dst, outlet, (fts_object_t *)target, ev_sym, ev_argc, ev_fp);
		  RESTORE_ATOM_STACK_FRAME(ev_fp);
		  ev_argc = 0;
		  ev_status = ev_get_dst;
		  break;
		case lex_type_end:
		  messbox_send(default_dst, outlet, (fts_object_t *)target, ev_sym, ev_argc, ev_fp);
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
		  ev_sym = NULL;
		  ev_status = ev_get_args;
		  break;
		case lex_type_comma:
		  messbox_send(default_dst, outlet, (fts_object_t *)target, ev_sym, ev_argc, ev_fp);
		  RESTORE_ATOM_STACK_FRAME(ev_fp);
		  ev_argc = 0;
		  ev_status = ev_get_first_arg;
		  break;
		case lex_type_semi:
		  messbox_send(default_dst, outlet, (fts_object_t *)target, ev_sym, ev_argc, ev_fp);
		  RESTORE_ATOM_STACK_FRAME(ev_fp);
		  ev_argc = 0;
		  ev_status = ev_get_dst;
		  break;
		case lex_type_end:
		  messbox_send(default_dst, outlet, (fts_object_t *)target, ev_sym, ev_argc, ev_fp);
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
	  fts_post("messbox: can't set value of type <%s>\n", fts_get_class_name(at + i));
	  return 0;
	}
    }

  return 1;
}

static fts_memorystream_t *message_memory_stream ;

static fts_memorystream_t * message_get_memory_stream()
{
  if( !message_memory_stream)
    message_memory_stream = (fts_memorystream_t *)fts_object_create( fts_memorystream_class, 0, 0);
  return message_memory_stream;
}

static void 
messbox_update(fts_object_t *o)
{
  messbox_t *this = (messbox_t *) o;
  fts_memorystream_t *stream = message_get_memory_stream();
  int size = fts_atom_list_get_size(this->atom_list);
  fts_atom_t *a = alloca( size * sizeof( fts_atom_t));
  
  fts_atom_list_get_atoms( this->atom_list, a);
  
  fts_memorystream_reset( stream);      
  fts_spost_object_description_args( (fts_bytestream_t *)stream, size, a);
  fts_bytestream_output_char((fts_bytestream_t *)stream,'\0');
  
  fts_client_start_message(o, fts_s_set);
  fts_client_add_string( o, fts_memorystream_get_bytes( stream));
  fts_client_done_message(o);
}

/************************************************************
 *
 *  system methods
 *
 */

static void messbox_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  messbox_t *this = (messbox_t *) o;

  this->atom_list = (fts_atom_list_t *)fts_object_create(atomlist_type, 0, 0);

  this->value = 0;
}


static void messbox_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  messbox_t *this = (messbox_t *) o;

  fts_send_message_varargs((fts_object_t *)this->atom_list, fts_s_delete, 0, 0);
}


static void messbox_upload(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  messbox_update(o);
}

static void messbox_dump_gui(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  messbox_t *this = (messbox_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *) fts_get_object(at);

  fts_atom_list_dump(this->atom_list, dumper, (fts_object_t *) this);
}


static void messbox_save_dotpat(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  messbox_t *this = (messbox_t *) o;
  FILE *file;
  int x, y, w, font_index;
  fts_atom_t a;
  fts_atom_list_iterator_t *iterator;
  int state;

  file = (FILE *)fts_get_pointer( at);

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
	      fprintf( file, " \\%s", s);
	    else if (s == fts_s_dollar)
	      state = 1;
	    else
	      fprintf( file, " %s",  s);
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
	      fprintf( file, " $ \\%s", s);
	    else
	      fprintf( file, " $ %s", s);
	    }

	break;
      }

      fts_atom_list_iterator_next( iterator);
    }

  fts_atom_list_iterator_free( iterator);

  fprintf( file, ";\n");
}

static void messbox_find(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  messbox_t *this = (messbox_t *) o;

  fts_objectset_t *set = (fts_objectset_t *)fts_get_object(at);

  if (fts_atom_list_is_subsequence(this->atom_list, ac - 1, at + 1))
    {
      fts_objectset_add(set, o);
    }
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

static void messbox_set(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  messbox_t *this = (messbox_t *) o;

  if(messbox_list_is_primitive(ac, at))
    fts_atom_list_set(this->atom_list, ac, at);

  if (fts_patcher_is_open( fts_object_get_patcher( (fts_object_t *) this)) )
    messbox_update(o);
}

static void messbox_clear(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  messbox_t *this = (messbox_t *) o;

  fts_atom_list_clear(this->atom_list);

  if (fts_patcher_is_open( fts_object_get_patcher( (fts_object_t *) this)))
    messbox_update(o);
}

static void messbox_append(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  messbox_t *this = (messbox_t *) o;
  int winlet = fts_object_get_message_inlet(o);

  if(messbox_list_is_primitive(ac, at))
    fts_atom_list_append(this->atom_list, ac, at);

  if (winlet == 0 && fts_patcher_is_open( fts_object_get_patcher( (fts_object_t *) this)))
    messbox_update(o);
}

static void messbox_off(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  messbox_t *this = (messbox_t *)o;

  this->value = 0;
  fts_update_request((fts_object_t *)this);
}


static void messbox_eval(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  messbox_t *this = (messbox_t *) o;

  fts_eval_atom_list(this, this->atom_list, ac, at, o, 0);
}

static void messbox_click(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  messbox_t *this = (messbox_t *) o;

  if(this->value == 0)
  {
    this->value = 1;
    fts_update_request(o);

    fts_timebase_add_call(fts_get_timebase(), o, messbox_off, 0, DEFAULT_DURATION);
  }

  fts_eval_atom_list(this, this->atom_list, ac, at, o, 0);
}

static void messbox_update_gui(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  messbox_update(o);
}

static void messbox_update_real_time(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  messbox_t *this = (messbox_t *) o;
  fts_atom_t a;

  fts_set_int( &a, this->value);  
  fts_client_send_message_real_time(o, fts_s_value, 1, &a);
}

static void
messbox_spost_description(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  int descr_ac = fts_object_get_description_size(o);
  fts_atom_t *descr_at = fts_object_get_description_atoms(o);
  
  fts_spost_object_description_args( (fts_bytestream_t *)fts_get_object(at), descr_ac - 1, descr_at + 1);
}

/************************************************************
 *
 *  class
 *
 */

static void messbox_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(messbox_t), messbox_init, messbox_delete);

  fts_class_message_varargs(cl, fts_s_find, messbox_find);

  /* atom list saving/loading/update support */
  fts_class_message_varargs(cl, fts_s_upload, messbox_upload);

  fts_class_message_varargs(cl, fts_s_dump_gui, messbox_dump_gui);
  fts_class_message_varargs(cl, fts_s_save_dotpat, messbox_save_dotpat); 

  fts_class_message_varargs(cl, fts_s_update_gui, messbox_update_gui); 
  fts_class_message_varargs(cl, fts_s_update_real_time, messbox_update_real_time); 
  fts_class_message_varargs(cl, fts_s_spost_description, messbox_spost_description); 

  fts_class_message_varargs(cl, fts_new_symbol("click"), messbox_click);
  
  fts_class_message_varargs(cl, fts_s_set, messbox_set);
  fts_class_message_varargs(cl, fts_s_append, messbox_append);
  fts_class_message_varargs(cl, fts_s_clear, messbox_clear);

  fts_class_inlet_int(cl, 0, messbox_eval);
  fts_class_inlet_float(cl, 0, messbox_eval);
  fts_class_inlet_symbol(cl, 0, messbox_eval);
  fts_class_inlet_varargs(cl, 0, messbox_eval);

  fts_class_outlet_thru(cl, 0);
  fts_class_outlet_varargs(cl, 0);
}

void messbox_config(void)
{
  init_eval();

  fts_class_install(fts_new_symbol("messbox"), messbox_instantiate);
}
