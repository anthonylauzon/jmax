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

#define MAX_ARGLEN 1024

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
      input_error
    } status;

  char type;
  char i_buf[MAX_ARGLEN];
  char *i_pt;
  int i_ac;
  fts_atom_t i_av[MARG];
  fts_atom_t *i_ap;
} cp_in;


/* The message parser */

static void
init_parser(void)
{
  cp_in.status = waiting_type;
}

static long
dtol(char *s)
{
  long n = 0;

  for (; *s; s++)
    if (('0' <= *s) && (*s <= '9'))
      n = n * 10 + (*s - '0');

  return n;
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

#define LCHECK() \
if (cp_in.i_pt >= cp_in.i_buf + MAX_ARGLEN) \
fprintf(stderr,"overflow %d %d\n", cp_in.i_pt,  cp_in.i_buf + MAX_ARGLEN)

void
fts_client_parse_char(char c)
{
  int eom = 0;

  switch (cp_in.status)
    {
    case waiting_type:
      {
	if ((c >= '0') && (c <= '9'))
	  {
	    /* Skip the client id (old ftsd compatibility *only*) */

	    cp_in.status = waiting_type;
	  }
	else if ((c == ' ') || (c == '\t'))
	  cp_in.status = waiting_type;
	else
	  {
	    cp_in.type = c;
	    cp_in.status = waiting_value;
	    cp_in.i_ac   = 0;
	    cp_in.i_ap   = cp_in.i_av;
	  }
      }
      break;

    case waiting_value:
      {
	if ((c == ' ') || (c == '\t'))
	  cp_in.status = waiting_value;
	else if (c == LONG_POS_CODE)
	  {
	    cp_in.status = in_long;
	    cp_in.i_pt = cp_in.i_buf;
	  }
	else if (c == LONG_NEG_CODE)
	  {
	    cp_in.status = in_neg_long;
	    cp_in.i_pt = cp_in.i_buf;
	  }
	else if (c == FLOAT_CODE)
	  {
	    cp_in.status = in_float;
	    cp_in.i_pt = cp_in.i_buf;
	  }
	else if (c == OBJECT_CODE)
	  {
	    cp_in.status = in_object;
	    cp_in.i_pt = cp_in.i_buf;
	  }
	else if (c == STRING_START_CODE)
	  {
	    cp_in.status = in_string;
	    cp_in.i_pt = cp_in.i_buf;
	  }
	else if (c == EOM_CODE)
	  eom = 1;
	else
	  cp_in.status = input_error;

	break;
      }

    case in_long:
      {
	int value_found = 0;
	
	if ((c == ' ') || (c == '\t'))
	  {
	    value_found = 1;
	    cp_in.status = waiting_value;
	  }
	else if (c == LONG_POS_CODE)
	  {
	    value_found = 1;
	    cp_in.status = in_long;
	  }
	else if (c == LONG_NEG_CODE)
	  {
	    value_found = 1;
	    cp_in.status = in_neg_long;
	  }
	else if (c == FLOAT_CODE)
	  {
	    value_found = 1;
	    cp_in.status = in_float;
	  }
	else if (c == OBJECT_CODE)
	  {
	    value_found = 1;
	    cp_in.status = in_object;
	  }
	else if (c == STRING_START_CODE)
	  {
	    value_found = 1;
	    cp_in.status = in_string;
	  }
	else if (c == EOM_CODE)
	  {
	    value_found = 1;
	    eom = 1;
	  }
	else
	  {

	    LCHECK();
	    if (cp_in.i_pt == cp_in.i_buf + MAX_ARGLEN)
	      cp_in.status = input_error;
	    else
	      *((cp_in.i_pt)++) = c;
	  }

	if (value_found)
	  {
	    *(cp_in.i_pt) = '\0';

	    fts_set_long(cp_in.i_ap, dtol(cp_in.i_buf));

	    (cp_in.i_ac)++;
	    (cp_in.i_ap)++;

	    if (cp_in.i_ac >= MARG) 
	      cp_in.status = input_error;

	    cp_in.i_pt = cp_in.i_buf;
	  }
      }
      break;

    case in_neg_long:
      {
	int value_found = 0;
	
	if ((c == ' ') || (c == '\t'))
	  {
	    value_found = 1;
	    cp_in.status = waiting_value;
	  }
	else if (c == LONG_POS_CODE)
	  {
	    value_found = 1;
	    cp_in.status = in_long;
	  }
	else if (c == LONG_NEG_CODE)
	  {
	    value_found = 1;
	    cp_in.status = in_neg_long;
	  }
	else if (c == FLOAT_CODE)
	  {
	    value_found = 1;
	    cp_in.status = in_float;
	  }
	else if (c == OBJECT_CODE)
	  {
	    value_found = 1;
	    cp_in.status = in_object;
	  }
	else if (c == STRING_START_CODE)
	  {
	    value_found = 1;
	    cp_in.status = in_string;
	  }
	else if (c == EOM_CODE)
	  {
	    value_found = 1;
	    eom = 1;
	  }
	else
	  {
	    LCHECK();
	    if (cp_in.i_pt == cp_in.i_buf + MAX_ARGLEN)
	      cp_in.status = input_error;
	    else
	      *((cp_in.i_pt)++) = c;
	  }

	if (value_found)
	  {
	    *(cp_in.i_pt) = '\0';

	    fts_set_long(cp_in.i_ap, (-1) * dtol(cp_in.i_buf));

	    (cp_in.i_ac)++;
	    (cp_in.i_ap)++;

	    if (cp_in.i_ac >= MARG) 
	      cp_in.status = input_error;

	    cp_in.i_pt = cp_in.i_buf;
	  }
      }
      break;

    case in_float:
      {
	int value_found = 0;

	if ((c == ' ') || (c == '\t'))
	  {
	    value_found = 1;
	    cp_in.status = waiting_value;
	  }
	else if (c == LONG_POS_CODE)
	  {
	    value_found = 1;
	    cp_in.status = in_long;
	  }
	else if (c == LONG_NEG_CODE)
	  {
	    value_found = 1;
	    cp_in.status = in_neg_long;
	  }
	else if (c == FLOAT_CODE)
	  {
	    value_found = 1;
	    cp_in.status = in_float;
	  }
	else if (c == OBJECT_CODE)
	  {
	    value_found = 1;
	    cp_in.status = in_object;
	  }
	else if (c == STRING_START_CODE)
	  {
	    value_found = 1;
	    cp_in.status = in_string;
	  }
	else if (c == EOM_CODE)
	  {
	    value_found = 1;
	    eom = 1;
	  }
	else
	  {
	    LCHECK();
	    if (cp_in.i_pt == cp_in.i_buf + MAX_ARGLEN)
	      cp_in.status = input_error;
	    else
	      *((cp_in.i_pt)++) = c;
	  }

	if (value_found)
	  {
	    float f;
	    *(cp_in.i_pt) = '\0';
	    sscanf(cp_in.i_buf, "%f", &f);

	    fts_set_float(cp_in.i_ap, f);

	    (cp_in.i_ac)++;
	    (cp_in.i_ap)++;
	    if (cp_in.i_ac >= MARG) 
	      cp_in.status = input_error;

	    cp_in.i_pt = cp_in.i_buf;
	  }
      }

      break;

    case in_object:
      {
	int value_found = 0;
	
	if ((c == ' ') || (c == '\t'))
	  {
	    value_found = 1;
	    cp_in.status = waiting_value;
	  }
	else if (c == LONG_POS_CODE)
	  {
	    value_found = 1;
	    cp_in.status = in_long;
	  }
	else if (c == LONG_NEG_CODE)
	  {
	    value_found = 1;
	    cp_in.status = in_neg_long;
	  }
	else if (c == FLOAT_CODE)
	  {
	    value_found = 1;
	    cp_in.status = in_float;
	  }
	else if (c == OBJECT_CODE)
	  {
	    value_found = 1;
	    cp_in.status = in_object;
	  }
	else if (c == STRING_START_CODE)
	  {
	    value_found = 1;
	    cp_in.status = in_string;
	  }
	else if (c == EOM_CODE)
	  {
	    value_found = 1;
	    eom = 1;
	  }
	else
	  {

	    LCHECK();
	    if (cp_in.i_pt == cp_in.i_buf + MAX_ARGLEN)
	      cp_in.status = input_error;
	    else
	      *((cp_in.i_pt)++) = c;
	  }

	if (value_found)
	  {
	    int id;
	    extern fts_object_t *fts_object_table_get(int id); /* @@@ waiting for type merge */


	    *(cp_in.i_pt) = '\0';

	    id = dtol(cp_in.i_buf);
	    fts_set_object(cp_in.i_ap, fts_object_table_get(id));

	    (cp_in.i_ac)++;
	    (cp_in.i_ap)++;

	    if (cp_in.i_ac >= MARG) 
	      cp_in.status = input_error;

	    cp_in.i_pt = cp_in.i_buf;
	  }
      }
      break;

    case in_string:
      if (c == STRING_END_CODE)
	{
	  *(cp_in.i_pt) = '\0';
	  fts_set_symbol(cp_in.i_ap, fts_new_symbol_copy(cp_in.i_buf));

	  (cp_in.i_ac)++;
	  (cp_in.i_ap)++;
	  if (cp_in.i_ac >= MARG) 
	    cp_in.status = input_error;

	  cp_in.status = waiting_value;
	}
      else if (c == STRING_QUOTE_CODE)
	{
	  cp_in.status = in_string_quoted;
	}
      else
	{
	  LCHECK();
	  if (cp_in.i_pt == cp_in.i_buf + MAX_ARGLEN)
	    cp_in.status = input_error;
	  else
	    *((cp_in.i_pt)++) = c;
	}
      break;

    case in_string_quoted:
      *((cp_in.i_pt)++) = c;
      cp_in.status = in_string;
      break;

    case input_error:
      if (c == EOM_CODE)
	{
	  cp_in.status = waiting_type;
	}
      break;
    }

  if (eom)
    {
      fts_client_mess_dispatch(cp_in.type, cp_in.i_ac, cp_in.i_av);
      cp_in.status = waiting_type;
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

