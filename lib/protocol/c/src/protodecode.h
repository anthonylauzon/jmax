/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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

enum decode_state { 
  STATE_IN_TYPE,
  STATE_IN_INT,
  STATE_IN_FLOAT,
  STATE_IN_STRING
};

#define STRING_MAX 256

typedef struct _protodecode_t {
  enum decode_state state;
  int counter;
  int int_value;
  float float_value;
  int string_len;
  char string_value[STRING_MAX];
} protodecode_t;

typedef enum {
  INT_TOKEN,
  FLOAT_TOKEN,
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
      else if ( t == FLOATT_TOKEN)
	printf( "got float %f\n", protodecode_get_float( &pr));
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
     FLOAT_TOKEN if decoded token is a float. It can then be retrieved
       by protodecode_get_float
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
   Function: protodecode_get_float
   Description:
     Returns the current token, assumed it is a float
   Arguments:
     pr: a pointer to a protodecode structure
   Returns:
     the token value
*/
extern float protodecode_get_float( protodecode_t *pr);

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
