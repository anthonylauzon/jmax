/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */

#include "protodefs.h"
#include "protodecode.h"


void protodecode_init( protodecode_t *pr)
{
  pr->state = STATE_IN_TYPE;
}

protodecode_status_t protodecode_run( protodecode_t *pr, unsigned char b)
{
  switch( pr->state) {
  case STATE_IN_TYPE:
    if ( b == INT_CODE)
      {
	pr->state = STATE_IN_INT_0;
	pr->int_value = 0;
	return RUNNING;
      }
    else if ( b == STRING_START_CODE)
      {
	pr->state = STATE_IN_STRING;
	pr->string_len = 0;;
	return RUNNING;
      }
    else if ( b == EOM_CODE)
      return EOM_TOKEN;
    break;

  case STATE_IN_INT_0:
    pr->state = STATE_IN_INT_1;
    pr->int_value = b;
    return RUNNING;

  case STATE_IN_INT_1:
    pr->state = STATE_IN_INT_2;
    pr->int_value = (pr->int_value << 8) | b;
    return RUNNING;

  case STATE_IN_INT_2:
    pr->state = STATE_IN_INT_3;
    pr->int_value = (pr->int_value << 8) | b;
    return RUNNING;

  case STATE_IN_INT_3:
    pr->state = STATE_IN_TYPE;
    pr->int_value = (pr->int_value << 8) | b;
    return INT_TOKEN;

  case STATE_IN_STRING:
    if ( b != STRING_END_CODE)
      {
	/* TODO: should realloc() on buffer overflow */
	if ( pr->string_len < STRING_MAX)
	  {
	    pr->string_value[ pr->string_len] = (char)b;
	    pr->string_len++;
	  }
	
	return RUNNING;
      }
    else 
      {
	pr->string_value[ pr->string_len ] = '\0';
	pr->state = STATE_IN_TYPE;

	return STRING_TOKEN;
      }
  }

  return RUNNING;
}

int protodecode_get_int( protodecode_t *pr)
{
  return pr->int_value;
}

char *protodecode_get_string( protodecode_t *pr)
{
  return pr->string_value;
}

