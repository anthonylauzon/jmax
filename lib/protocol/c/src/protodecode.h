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

#ifndef _PROTODECODE_H
#define _PROTODECODE_H

#define STRING_MAX 256

enum decode_state { 
  STATE_IN_TYPE,
  STATE_IN_INT_0,
  STATE_IN_INT_1,
  STATE_IN_INT_2,
  STATE_IN_INT_3,
  STATE_IN_STRING
};

typedef struct _protodecode_t {
  enum decode_state state;
  int int_value;
  int string_len;
  char string_value[STRING_MAX];
} protodecode_t;

typedef enum {
  INT_TOKEN,
  STRING_TOKEN,
  EOM_TOKEN,
  RUNNING
} protodecode_status_t;

/*
  Description:
    The protodecode structure is used to decode a message.
  Example:
    The following code can be used to decode a protocol message:

void f()
{
  char buff[128];
  protodecode_t pr;

  receive_a_message( buff);
  protodecode_init( &pr);

  for ( i = 0; i < n; i++)
    {
      enum protodecode_status t;

      t = protodecode_run( &pr, buff[i]);

      if ( t == INT_TOKEN)
	printf( "got int %d\n", protodecode_get_int( &pr));
      else if ( t == STRING_TOKEN)
	printf( "got string %d\n", protodecode_get_string( &pr));
      else if ( t == EOM_TOKEN)
	break;
    }
}
*/


/*
   Function: protodecode_init
   Description:
     Initializes a protodecode structure.
   Arguments:
     pr: a pointer to a protodecode structure
   Returns:
*/
extern void protodecode_init( protodecode_t *pr);


/*
   Function: protodecode_run
   Description:
     Decodes the next token
   Arguments:
     pr: a pointer to a protodecode structure
     b: the current byte in received message
   Returns:
     INT_TOKEN if decoded token is an int. It can then be retrieved
       by protodecode_get_int
     STRING_TOKEN if decoded token is a string. It can then be retrieved
       by protodecode_get_string
     EOM_TOKEN if decoded token is end of message.
*/
extern protodecode_status_t protodecode_run( protodecode_t *pr, unsigned char b);

/*
   Function: protodecode_get_int
   Description:
     Returns the current token, assumed it is an int
   Arguments:
     pr: a pointer to a protodecode structure
   Returns:
     the token value
*/
extern int protodecode_get_int( protodecode_t *pr);

/*
   Function: protodecode_get_string
   Description:
     Returns the current token, assumed it is a string
   Arguments:
     pr: a pointer to a protodecode structure
   Returns:
     the token value
*/
extern char *protodecode_get_string( protodecode_t *pr);

#endif
