#ifndef _PROTOCOLS_H
#define _PROTOCOLS_H

/* A message  is conceptually made of:
 *
 * 1- the command this message represent.
 * 2- an array of fts_values, whose interpretation depend
 *    on the type of message
 *
 *    The size of the array is variable
 *
 * Used Letters are:
 *
 * Forbidden values : 0x00 (easy message as string in C)
 * Low values: 0x01 0x02 0x03
 * Symbols   : - { } > < \
 * Lowercase : a b c d e f g h i l m n o p q s u x y w z  
 * Uppercase : A B C D E F G H M N O P Q R S U X Y Z 
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

#define LOAD_PATCHER_BMAX_CODE       'U'
#define LOAD_PATCHER_DPAT_CODE       'Z'

#define DECLARE_ABSTRACTION_CODE          'E'
#define DECLARE_ABSTRACTION_PATH_CODE     'F'

#define DECLARE_TEMPLATE_CODE          'y'
#define DECLARE_TEMPLATE_PATH_CODE     'w'

#define DOWNLOAD_OBJECT_CODE    'G'
#define DOWNLOAD_CONNECTION_CODE  'H'

#define NEW_OBJECT_CODE         'n'
#define NEW_OBJECT_VAR_CODE     'N'
#define DELETE_OBJECT_CODE      'f'

#define REDEFINE_PATCHER_CODE   'Q'
#define REDEFINE_OBJECT_CODE    'R'
#define REDEFINE_CONNECTION_CODE    'V'
#define REPOSITION_INLET        'X'
#define REPOSITION_OUTLET       'Y'


#define NEW_CONNECTION_CODE     'c'
#define DELETE_CONNECTION_CODE  'a'

#define CONNECTION_RELEASE_CODE 'd'
#define OBJECT_RELEASE_CODE     'l'
#define OBJECT_RELEASE_DATA_CODE 'b'
#define MESSAGE_CODE            'm'

#define PUTPROP_CODE            'p'
#define GETPROP_CODE            'g'
#define GETALLPROP_CODE         'B'

#define CLIENTPROP_CODE         'P'
#define CLIENTMESS_CODE         'M'

#define UPDATE_GROUP_START_CODE  '{'
#define UPDATE_GROUP_END_CODE    '}'

#define RECOMPUTE_ERRORS_CODE    'e'
#define FTS_SHUTDOWN_CODE       's'
/* Sincronization code */

#define SYNC_CODE     '>'
#define SYNC_DONE_CODE     '<'

#define POST_CODE     'h'	/* a post request for the server */


/* DATA RELATIVE CODES */

#define REMOTE_CALL_CODE    '!'

/* Value coding */

/*
   The float code cannot be f, because this bring
   to an ambiguity with hex ints.

   Objects are represented as a positive number corresponding to its ID.
*/
   
#define STRING_START_CODE   0x01
#define STRING_END_CODE     0x02

#define INT_CODE            0x03
#define FLOAT_CODE          0x04
#define SYMBOL_CACHED_CODE  0x05
#define SYMBOL_AND_DEF_CODE 0x06
#define SYMBOL_CODE         0x07
#define OBJECT_CODE         0x08
#define CONNECTION_CODE     0x09
#define DATA_CODE           0x0a
#define EOM_CODE            0x0b


#endif





