package ircam.jmax.fts;

/**
 * A class coding the FTS client/server protocol. <br>
 * All the important values and range of values are defined
 * here by means of constants or predicates. <p>
 *
 * @see FtsPort
 */

class FtsClientProtocol
{
  // Protocol type coding Special chars

  static final int pos_int_type_code = 'i';
  static final int neg_int_type_code = 'n';
  static final int float_type_code   = 'z';
  static final int object_type_code   = 'o';
  static final int string_start_code = '\"';
  static final int string_end_code   = '\"';
  static final int string_quote_code   = '\\';
  static final int end_of_message_code = 0x0a;
  
  // predicated coding protocol character sets

  static boolean tokenStartingChar(int c)
  {
    return (
	    (c == pos_int_type_code) ||
	    (c == neg_int_type_code) ||
	    (c == float_type_code)   ||
	    (c == object_type_code)   ||
	    (c == string_start_code) ||
	    (c == end_of_message_code) ||
	    isBlank(c)
	     );
  }

  static boolean isBlank(int c)
  {
    return ((c == ' ') || (c == '\t'));
  }

  static boolean isHexDigit(int c)
  {
    return (
	    ('0' == c) ||
	    ('1' == c) ||
	    ('2' == c) ||
	    ('3' == c) ||
	    ('4' == c) ||
	    ('5' == c) ||
	    ('6' == c) ||
	    ('7' == c) ||
	    ('8' == c) ||
	    ('9' == c) ||
	    ('a' == c) ||
	    ('b' == c) ||
	    ('c' == c) ||
	    ('d' == c) ||
	    ('e' == c) ||
	    ('f' == c));
  }

  static boolean isEom(int c)
  {
    return c == end_of_message_code;
  }

  // Protocol defined commands code

  static final int ucs_cmd = 'u';
  static final int sync_cmd = '>';
  static final int sync_done_cmd = '<';
  static final int post_cmd = 'h';

  static final int fts_open_patcher_cmd = 'O';
  static final int fts_close_patcher_cmd = 'C';
  static final int fts_patcher_loaded_cmd = 'L';

  static final int fts_new_object_cmd = 'n';
  static final int fts_redefine_object_cmd = 'N';
  static final int fts_replace_object_cmd = 'R';
  static final int fts_free_object_cmd = 'f';
  static final int fts_connect_objects_cmd = 'c';
  static final int fts_disconnect_objects_cmd = 'd';

  static final int fts_message_cmd = 'm';
  static final int fts_named_message_cmd = 'z';

  static final int fts_put_property_cmd = 'p';
  static final int fts_get_property_cmd = 'g';

  static final int fts_property_value_cmd = 'P';
  static final int fts_server_message_cmd = 'M';
}







