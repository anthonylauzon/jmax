/*
 *                      Copyright (c) 1993 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *  This file contains the protocol parser and the message dispatcher
 *  for incoming messages
 *
 */

#include <string.h>

#include "protocol.h"
#include "sys.h"
#include "lang.h"
#include "runtime/sched.h"
#include "runtime/devices.h"
#include "runtime/client/client.h"

/* forward declarations */


static void fts_client_mess_dispatch(char type,  int ac, const fts_atom_t *av);
static void init_parser(void);
static void (* mess_dispatch_table[256])(int, const fts_atom_t *);
static void fts_client_mess_unknown(int ac, const  fts_atom_t *av);

/* Init function  */

void
fts_client_incoming_init(void)
{
  int i;

  for (i = 0; i < 256; i++)
    mess_dispatch_table[i] = fts_client_mess_unknown;

  init_parser();
}


void
fts_client_incoming_restart(void)
{
  init_parser();
}

   
/******************************************************************************/
/*                                                                            */
/*             CLIENT ==> FTS Communication and parsing functions             */
/*                                                                            */
/******************************************************************************/

/* the state of an input channel from a host program */

enum parser_state
{
  waiting_type,
  waiting_value,
  in_long,
  in_neg_long,
  in_float,
  in_string,
  in_string_quoted,
  in_object,
  in_data,
  in_connection,
  end_of_message, 
  input_error
};

#define INIT_MESSAGE_BUF_LENGTH 1024
#define INIT_NARGS               256

static struct inmess
{
  enum parser_state status;
  char type;

  int buf_size;
  int buf_fill_p;
  char *buf;

  int av_size;
  int ac;
  fts_atom_t *av;
} parser;


/* The message parser */

static void
init_parser(void)
{
  parser.status = waiting_type;
  parser.buf_size = INIT_MESSAGE_BUF_LENGTH;
  parser.av_size = INIT_NARGS;
  parser.buf = (char *) fts_malloc(parser.buf_size * sizeof(char));
  parser.av  = (fts_atom_t *) fts_malloc(parser.av_size * sizeof(fts_atom_t));
}

static void
double_av(void)
{
  int new_size;
  fts_atom_t *new_av;
  int i;

  new_size = parser.av_size * 2;
  new_av = (fts_atom_t *) fts_malloc(new_size * sizeof(fts_atom_t));

  for (i = 0; i < parser.av_size; i++)
    new_av[i] = parser.av[i];

  fts_free(parser.av);
  parser.av = new_av;
  parser.av_size = new_size;
}

static void
double_buf(void)
{
  int new_size;
  char *new_buf;
  int i;

  new_size = parser.buf_size * 2;
  new_buf  = (char *) fts_malloc(new_size * sizeof(char));

  for (i = 0; i < parser.buf_size; i++)
    new_buf[i] = parser.buf[i];

  fts_free(parser.buf);
  parser.buf = new_buf;
  parser.buf_size = new_size;
}
     

/* Convert a string to long, taking in account the sign,
   if any */

static long
dtol(char *s)
{
  long n = 0;
  int  sign = 1;

  if (*s == '-')
    {
      s++;
      sign = -1;
    }
  else if (*s == '+')
    s++;

  for (; *s; s++)
    if (('0' <= *s) && (*s <= '9'))
      n = n * 10 + (*s - '0');

  return n * sign;
}

/* take a character of input from host */

/* New version, new protocol compliant
 * Implemented as a finite state automata;
 * it include string dequoting.
 * All messages with an error of any kind are
 * skipped (i.e. all the chars until the next EOM_CODE`
 * are thrown away).
 */

/* 
  Should simplify more the code.
*/

static enum parser_state
get_status_for(char c)
{
  if (c == LONG_POS_CODE)
    return in_long;
  else if (c == FLOAT_CODE)
    return  in_float;
  else if (c == OBJECT_CODE)
    return in_object;
  else if (c == DATA_CODE)
    return in_data;
  else if (c == CONNECTION_CODE)
    return in_connection;
  else if (c == STRING_START_CODE)
    return in_string;
  else if (c == EOM_CODE)
    return  end_of_message;
  else
    return  input_error;
}

static int
is_token_char(char c)
{
  return ((c == LONG_POS_CODE) ||
	  (c == FLOAT_CODE)    ||
	  (c == OBJECT_CODE)   ||
	  (c == DATA_CODE)     ||
	  (c == CONNECTION_CODE) ||
	  (c == STRING_START_CODE) ||
	  (c == EOM_CODE));
}

static void
add_char(char c)
{
  if (parser.buf_fill_p >= parser.buf_size)
    double_buf();

  parser.buf[parser.buf_fill_p] = c;
  parser.buf_fill_p++;
}


static void
add_arg(fts_atom_t *a)
{
  if (parser.ac >= parser.av_size) 
    double_av();

  parser.av[parser.ac] = *a;
  (parser.ac)++;
}


void
fts_client_parse_char(char c)
{
  fts_atom_t a;

  switch (parser.status)
    {
    case waiting_type:
      parser.type = c;
      parser.status = waiting_value;
      parser.ac   = 0;
      break;

    case waiting_value:
      if (is_token_char(c))
	{
	  parser.buf_fill_p = 0; 
	  parser.status = get_status_for(c);
	}
      else
	parser.status = input_error;
      break;

    case in_long:
      if (is_token_char(c))
	{
	  /* compute next state  */

	  parser.status = get_status_for(c);

	  /* parse the  value  */

	  add_char('\0');
	  fts_set_long(&a, dtol(parser.buf));
	  add_arg(&a);

	  parser.buf_fill_p = 0;
	}
      else
	add_char(c);
      break;

    case in_float:
      if (is_token_char(c))
	{
	  float f;

	  /* compute next state  */

	  parser.status = get_status_for(c);

	  /* parse the  value  */

	  add_char('\0');
	  sscanf(parser.buf, "%f", &f);
	  fts_set_float(&a, f);
	  add_arg(&a);

	  parser.buf_fill_p = 0;
	}
      else
	add_char(c);
      break;

    case in_object:
      if (is_token_char(c))
	{
	  /* compute next state  */

	  parser.status = get_status_for(c);

	  /* parse the  value  */

	  add_char('\0');
	  fts_set_object(&a, fts_object_table_get(dtol(parser.buf)));
	  add_arg(&a);

	  parser.buf_fill_p = 0;
	}
      else
	add_char(c);
      break;

    case in_data:
      if (is_token_char(c))
	{
	  /* compute next state  */

	  parser.status = get_status_for(c);

	  /* parse the  value  */

	  add_char('\0');
	  fts_set_data(&a, fts_data_id_get(dtol(parser.buf)));
	  add_arg(&a);

	  parser.buf_fill_p = 0;
	}
      else
	add_char(c);
      break;


    case in_connection:
      if (is_token_char(c))
	{
	  /* compute next state  */

	  parser.status = get_status_for(c);

	  /* parse the  value  */

	  add_char('\0');
	  fts_set_connection(&a, fts_connection_table_get(dtol(parser.buf)));
	  add_arg(&a);

	  parser.buf_fill_p = 0;
	}
      else
	add_char(c);
      break;

    case in_string:
      if (c == STRING_END_CODE)
	{
	  add_char('\0');
	  fts_set_symbol(&a, fts_new_symbol_copy(parser.buf));
	  add_arg(&a);

	  parser.status = waiting_value;
	}
      else
	add_char(c);
      break;

    case input_error:
      if (c == EOM_CODE)
	parser.status = waiting_type;
      break;
    }

  if (parser.status == end_of_message)
    {
      fts_client_mess_dispatch(parser.type, parser.ac, parser.av);
      parser.status = waiting_type;
    }
}


/*
   Client message dispatch subsystem.
*/

/* dispatching table housekeeping */


static void
fts_client_mess_unknown(int ac, const  fts_atom_t *av)
{
  /* Should raise an error of some kind @@@ */
}


void
fts_client_mess_install(char type, void (* fun) (int, const fts_atom_t *))
{
  mess_dispatch_table[(int) type] = fun;
}




/* The real dispatcher */

static void
fts_client_mess_dispatch(char type, int ac, const fts_atom_t *av)
{
  (* mess_dispatch_table[(int) type]) (ac, av);
}









