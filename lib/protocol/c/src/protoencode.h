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

#ifndef _PROTOENCODE_H
#define _PROTOENCODE_H

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


#endif
