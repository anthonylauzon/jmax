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
#include <stdio.h> 

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

static struct inmess
{
  enum
    {
      waiting_type,
      waiting_value,
      in_long,
      in_neg_long,
      in_float,
      in_string,
      in_string_quoted,
      in_object,
      in_connection,
      input_error
    } status;

  char type;
  char buf[MAX_MESSAGE_LENGTH];
  char *buf_fill_p;
  int ac;
  fts_atom_t av[MAX_NARGS];
} parser;


/* The message parser */

static void
init_parser(void)
{
  parser.status = waiting_type;
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


void
fts_client_parse_char(char c)
{
  int eom = 0;

  switch (parser.status)
    {
    case waiting_type:
      {
	parser.type = c;
	parser.status = waiting_value;
	parser.ac   = 0;
      }
      break;

    case waiting_value:
      {
	if (c == LONG_POS_CODE)
	  {
	    parser.status = in_long;
	    parser.buf_fill_p = parser.buf;
	  }
	else if (c == FLOAT_CODE)
	  {
	    parser.status = in_float;
	    parser.buf_fill_p = parser.buf;
	  }
	else if (c == OBJECT_CODE)
	  {
	    parser.status = in_object;
	    parser.buf_fill_p = parser.buf;
	  }
	else if (c == CONNECTION_CODE)
	  {
	    parser.status = in_connection;
	    parser.buf_fill_p = parser.buf;
	  }
	else if (c == STRING_START_CODE)
	  {
	    parser.status = in_string;
	    parser.buf_fill_p = parser.buf;
	  }
	else if (c == EOM_CODE)
	  eom = 1;
	else
	  parser.status = input_error;

	break;
      }

    case in_long:
      {
	int value_found = 0;
	
	if (c == LONG_POS_CODE)
	  {
	    value_found = 1;
	    parser.status = in_long;
	  }
	else if (c == FLOAT_CODE)
	  {
	    value_found = 1;
	    parser.status = in_float;
	  }
	else if (c == OBJECT_CODE)
	  {
	    value_found = 1;
	    parser.status = in_object;
	  }
	else if (c == CONNECTION_CODE)
	  {
	    value_found = 1;
	    parser.status = in_connection;
	  }
	else if (c == STRING_START_CODE)
	  {
	    value_found = 1;
	    parser.status = in_string;
	  }
	else if (c == EOM_CODE)
	  {
	    value_found = 1;
	    eom = 1;
	  }
	else
	  {
	    if (parser.buf_fill_p >= parser.buf + MAX_MESSAGE_LENGTH)
	      parser.status = input_error;
	    else
	      *((parser.buf_fill_p)++) = c;
	  }

	if (value_found)
	  {
	    *(parser.buf_fill_p) = '\0';

	    fts_set_long(&(parser.av[parser.ac]), dtol(parser.buf));

	    (parser.ac)++;

	    if (parser.ac >= MAX_NARGS) 
	      parser.status = input_error;

	    parser.buf_fill_p = parser.buf;
	  }
      }
      break;

    case in_float:
      {
	int value_found = 0;

	if (c == LONG_POS_CODE)
	  {
	    value_found = 1;
	    parser.status = in_long;
	  }
	else if (c == FLOAT_CODE)
	  {
	    value_found = 1;
	    parser.status = in_float;
	  }
	else if (c == OBJECT_CODE)
	  {
	    value_found = 1;
	    parser.status = in_object;
	  }
	else if (c == CONNECTION_CODE)
	  {
	    value_found = 1;
	    parser.status = in_connection;
	  }
	else if (c == STRING_START_CODE)
	  {
	    value_found = 1;
	    parser.status = in_string;
	  }
	else if (c == EOM_CODE)
	  {
	    value_found = 1;
	    eom = 1;
	  }
	else
	  {
	    if (parser.buf_fill_p == parser.buf + MAX_MESSAGE_LENGTH)
	      parser.status = input_error;
	    else
	      *((parser.buf_fill_p)++) = c;
	  }

	if (value_found)
	  {
	    float f;
	    *(parser.buf_fill_p) = '\0';
	    sscanf(parser.buf, "%f", &f);

	    fts_set_float(&(parser.av[parser.ac]), f);
	    (parser.ac)++;

	    if (parser.ac >= MAX_NARGS) 
	      parser.status = input_error;

	    parser.buf_fill_p = parser.buf;
	  }
      }

      break;

    case in_object:
      {
	int value_found = 0;
	
	if (c == LONG_POS_CODE)
	  {
	    value_found = 1;
	    parser.status = in_long;
	  }
	else if (c == FLOAT_CODE)
	  {
	    value_found = 1;
	    parser.status = in_float;
	  }
	else if (c == OBJECT_CODE)
	  {
	    value_found = 1;
	    parser.status = in_object;
	  }
	else if (c == CONNECTION_CODE)
	  {
	    value_found = 1;
	    parser.status = in_connection;
	  }
	else if (c == STRING_START_CODE)
	  {
	    value_found = 1;
	    parser.status = in_string;
	  }
	else if (c == EOM_CODE)
	  {
	    value_found = 1;
	    eom = 1;
	  }
	else
	  {
	    if (parser.buf_fill_p == parser.buf + MAX_MESSAGE_LENGTH)
	      parser.status = input_error;
	    else
	      *((parser.buf_fill_p)++) = c;
	  }

	if (value_found)
	  {
	    int id;

	    *(parser.buf_fill_p) = '\0';

	    id = dtol(parser.buf);
	    fts_set_object(&(parser.av[parser.ac]), fts_object_table_get(id));

	    (parser.ac)++;

	    if (parser.ac >= MAX_NARGS) 
	      parser.status = input_error;

	    parser.buf_fill_p = parser.buf;
	  }
      }
      break;

    case in_connection:
      {
	int value_found = 0;
	
	if (c == LONG_POS_CODE)
	  {
	    value_found = 1;
	    parser.status = in_long;
	  }
	else if (c == FLOAT_CODE)
	  {
	    value_found = 1;
	    parser.status = in_float;
	  }
	else if (c == OBJECT_CODE)
	  {
	    value_found = 1;
	    parser.status = in_object;
	  }
	else if (c == CONNECTION_CODE)
	  {
	    value_found = 1;
	    parser.status = in_connection;
	  }
	else if (c == STRING_START_CODE)
	  {
	    value_found = 1;
	    parser.status = in_string;
	  }
	else if (c == EOM_CODE)
	  {
	    value_found = 1;
	    eom = 1;
	  }
	else
	  {
	    if (parser.buf_fill_p == parser.buf + MAX_MESSAGE_LENGTH)
	      parser.status = input_error;
	    else
	      *((parser.buf_fill_p)++) = c;
	  }

	if (value_found)
	  {
	    int id;

	    *(parser.buf_fill_p) = '\0';

	    id = dtol(parser.buf);

	    fts_set_connection(&(parser.av[parser.ac]), fts_connection_table_get(id));

	    fprintf(stderr, "Got connection Id %d, connection %lx\n", id, fts_get_connection(&(parser.av[parser.ac])));

	    (parser.ac)++;

	    if (parser.ac >= MAX_NARGS) 
	      parser.status = input_error;

	    parser.buf_fill_p = parser.buf;
	  }
      }
      break;

    case in_string:
      if (c == STRING_END_CODE)
	{
	  *(parser.buf_fill_p) = '\0';
	  fts_set_symbol(&(parser.av[parser.ac]), fts_new_symbol_copy(parser.buf));

	  (parser.ac)++;

	  if (parser.ac >= MAX_NARGS) 
	    parser.status = input_error;

	  parser.status = waiting_value;
	}
      else
	{
	  if (parser.buf_fill_p == parser.buf + MAX_MESSAGE_LENGTH)
	    parser.status = input_error;
	  else
	    *((parser.buf_fill_p)++) = c;
	}
      break;

    case input_error:
      if (c == EOM_CODE)
	{
	  parser.status = waiting_type;
	}
      break;
    }

  if (eom)
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









