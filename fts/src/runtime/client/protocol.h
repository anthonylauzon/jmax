#ifndef _PROTOCOLS_H
#define _PROTOCOLS_H

/* A message  is conceptually made of:

   1- a type of message (i.e. control, max message, event ...) (an int)
   2- the command this message represent.
   2- an integer giving the number of arguments
   3- an array of fts_values, whose interpretation depend
      on the type of message

The size of the array is variable

From now on, the character code and the value in the message structure
are the same, so the users can add as many message type as they want.
(of course, different from the existings ..).
*/

/* The first character identify
   The meaning of the command is subsystem specific.
   Here are the predefined commands
*/


/* The void command */

#define VOID_COMMAND_CODE    '-'

/* The ucs command  */

#define UCS_CODE      'u'

/* Command defined for the FOS subsystem  */

#define SAVE_PATCHER_CODE       'S'
#define OPEN_PATCHER_CODE       'O'
#define CLOSE_PATCHER_CODE      'C'
#define PATCHER_LOADED_CODE     'L'

#define NEW_OBJECT_CODE         'n'
#define REDEFINE_OBJECT_CODE    'N'
#define REPLACE_OBJECT_CODE     'R'
#define FREE_OBJECT_CODE        'f'
#define CONNECT_OBJECTS_CODE    'c'
#define DISCONNECT_OBJECTS_CODE 'd'
#define MESSAGE_CODE            'm'
#define NAMED_MESSAGE_CODE      'z'

#define PUTPROP_CODE            'p'
#define GETPROP_CODE            'g'

#define CLIENTPROP_CODE         'P'
#define CLIENTMESS_CODE         'M'

#define UPDATE_GROUP_START_CODE  '{'
#define UPDATE_GROUP_END_CODE    '}'

/* Sincronization code */

#define SYNC_CODE     '>'
#define SYNC_DONE_CODE     '<'

#define POST_CODE     'h'	/* a post request for the server */

/* Value coding */

/* Need more documentation ... */

/* Strings are delimited by '"'; quotes inside
   the strings are automatically quoted by a backslash
   This is done to don't have nulls in the message,
   so it is really human readable ascii.

   The float code cannot be f, because this bring
   to an ambiguity with hex ints.

   Ints are represented base hex, with value + sign representation.
   (2's complement is not architecture indipendent); the sign is merged
   with the type id to spare one char for negative ints.

   Objects are represented as a positive number corresponding to its ID.
   
*/
   
#define LONG_POS_CODE 'i'
#define FLOAT_CODE 'z'
#define STRING_START_CODE 0x01
#define STRING_END_CODE   0x02
#define STRING_QUOTE_CODE '\\'
#define OBJECT_CODE 'o'

/*
   End of message: this character cannot be changed, and cannot
   appear inside the message itself; strings do not support
   control character below 0x08. (temporary situation, waiting for
   the binary protocol).
 */

#define EOM_CODE  0x03


#endif



