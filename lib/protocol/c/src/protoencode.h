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

#ifndef _PROTOENCODE_H
#define _PROTOENCODE_H

/* 
   Description:
     a hidden structure to encode a set of values in a buffer
*/
typedef struct _protoencode_t protoencode_t;

/*
   Function: protoencode_init
   Description:
     Initializes a protoencode structure.
   Arguments:
     pr: a pointer to a protoencode structure
     size: the size of the message buffer.
   Returns:
*/
extern void protoencode_init( protoencode_t *pr, int buffer_size);

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
   Function: protoencode_put_string
   Description:
     Append a string value to the current message
   Arguments:
     pr: a pointer to a protoencode structure
   Returns:
*/
extern void protoencode_put_string( protoencode_t *pr, const char *s);


#endif
