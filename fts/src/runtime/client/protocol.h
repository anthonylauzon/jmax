#ifndef _PROTOCOLS_H
#define _PROTOCOLS_H

/* A message  is conceptually made of:
 *
 * 1- a type of message (i.e. control, max message, event ...) (an int)
 * 2- the command this message represent.
 * 2- an integer giving the number of arguments
 * 3- an array of fts_values, whose interpretation depend
 *    on the type of message
 *
 *    The size of the array is variable
 *
 * Used Letters are:
 *
 * Forbidden values : 0x00 (easy message as string in C)
 * Low values: 0x01 0x02 0x03
 * Symbols   : - { } > < \
 * Lowercase : c d f g h i m n o p q u z x
 * Uppercase : A C D E F G H L M N O P Q R S T U V X Y Z 
 * 
 */

/* The first character identify
   The meaning of the command is subsystem specific.
   Here are the predefined commands
*/


/* The void command */

#define VOID_COMMAND_CODE    '-'

/* The ucs command  */

#define UCS_CODE      'u'

/* Command defined for the FOS subsystem: actually they should move to tiles/messtiles.h  */

#define SAVE_PATCHER_BMAX_CODE       'S'
#define SAVE_PATCHER_TPAT_CODE       'T'

#define LOAD_PATCHER_BMAX_CODE       'U'
#define LOAD_PATCHER_TPAT_CODE       'V'
#define LOAD_PATCHER_DPAT_CODE       'Z'

#define DECLARE_ABSTRACTION_CODE          'E'
#define DECLARE_ABSTRACTION_PATH_CODE     'F'

#define DOWNLOAD_PATCHER_CODE   'D'
#define DOWNLOAD_OBJECT_CODE    'G'
#define DOWNLOAD_CONNECTION_CODE  'H'

#define OPEN_PATCHER_CODE       'O'
#define CLOSE_PATCHER_CODE      'C'
#define PATCHER_LOADED_CODE     'L'

#define NEW_OBJECT_CODE         'n'
#define NEW_ABSTRACTION_CODE    'A'
#define REDEFINE_PATCHER_CODE   'Q'
#define REPOSITION_INLET        'X'
#define REPOSITION_OUTLET       'Y'
#define FREE_OBJECT_CODE        'f'
#define CONNECT_OBJECTS_CODE    'c'
#define DISCONNECT_OBJECTS_CODE 'd'
#define MESSAGE_CODE            'm'
#define NAMED_MESSAGE_CODE      'q'

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

/*
   The float code cannot be f, because this bring
   to an ambiguity with hex ints.

   Objects are represented as a positive number corresponding to its ID.
*/
   
#define LONG_POS_CODE 'i'
#define FLOAT_CODE 'z'
#define STRING_START_CODE 0x01
#define STRING_END_CODE   0x02
#define STRING_QUOTE_CODE '\\'
#define OBJECT_CODE 'o'
#define CONNECTION_CODE 'x'

/*
   End of message: this character cannot be changed, and cannot
   appear inside the message itself; strings do not support
   control character below 0x08. (temporary situation, waiting for
   the binary protocol).
 */

#define EOM_CODE  0x03


#endif





