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

#ifndef _PROTCODEC_H
#define _PROTCODEC_H

/* ********************************************************************** */
/*                                                                        */
/* Protocol encoding                                                      */
/*                                                                        */
/* ********************************************************************** */

/* 
   Description:
     a hidden structure to encode a set of values in a buffer
*/
typedef struct _protoencode_t {
  unsigned char *buffer;
  int buffer_size;
  int current;
} protoencode_t;

/*
   Function: protoencode_init
   Description:
     Initializes a protoencode structure and allocates the internal
     buffer.
   Arguments:
     pr: a pointer to a protoencode structure
   Returns:
*/
extern void protoencode_init( protoencode_t *pr);

/*
   Function: protoencode_destroy
   Description:
     Deinitializes a protoencode structure and frees the internal buffer.
   Arguments:
     pr: a pointer to a protoencode structure
   Returns:
*/
extern void protoencode_destroy( protoencode_t *pr);

/*
   Function: protoencode_get_mess
   Description:
     Returns a pointer to the current message, in order to send it on
     a UDP port (for example).
   Arguments:
     pr: a pointer to a protoencode structure
   Returns:
     A pointer to the current message
*/
extern unsigned char *protoencode_get_mess( protoencode_t *pr);

/*
   Function: protoencode_get_size
   Description:
     Returns the size of the current encoded message, in order to send it 
     on a UDP port (for example).
   Arguments:
     pr: a pointer to a protoencode structure
   Returns:
     The size of the current message
*/
extern int protoencode_get_size( protoencode_t *pr);

/*
   Function: protoencode_start
   Description:
     Starts encoding a new set of values
   Arguments:
     pr: a pointer to a protoencode structure
   Returns:
*/
extern void protoencode_start( protoencode_t *pr);

/*
   Function: protoencode_end
   Description:
     Finish encoding a new set of values.
     It adds a 'end of message' token.
   Arguments:
     pr: a pointer to a protoencode structure
   Returns:
*/
extern void protoencode_end( protoencode_t *pr);

/*
   Function: protoencode_put_int
   Description:
     Append an int value to the current message
   Arguments:
     pr: a pointer to a protoencode structure
   Returns:
*/
extern void protoencode_put_int( protoencode_t *pr, int value);

/*
   Function: protoencode_put_float
   Description:
     Append a float value to the current message
   Arguments:
     pr: a pointer to a protoencode structure
   Returns:
*/
extern void protoencode_put_float( protoencode_t *pr, float value);

/*
   Function: protoencode_put_string
   Description:
     Append a string value to the current message
   Arguments:
     pr: a pointer to a protoencode structure
   Returns:
*/
extern void protoencode_put_string( protoencode_t *pr, const char *s);



/* ********************************************************************** */
/*                                                                        */
/* Protocol decoding                                                      */
/*                                                                        */
/* ********************************************************************** */

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
