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

  static final int int_type_code      = 'i';
  static final int float_type_code    = 'z';
  static final int object_type_code   = 'o';
  static final int connection_type_code   = 'x';
  static final int data_type_code       = 0x05;
  static final int string_start_code = 0x01;
  static final int string_end_code   = 0x02;
  static final int end_of_message_code = 0x03;
  
  // predicated coding protocol character sets

  static boolean tokenStartingChar(int c)
  {
    return (
	    (c == int_type_code) ||
	    (c == float_type_code)   ||
	    (c == object_type_code)   ||
	    //	    (c == connection_type_code) || // (fd) was missing... Is it intentionnal ?
	    (c == data_type_code)   ||
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

  static final int remote_call_code = '!';

  static final int fts_save_patcher_bmax_cmd = 'S';
  static final int fts_save_patcher_tpat_cmd = 'T';

  static final int fts_load_patcher_bmax_cmd = 'U';
  static final int fts_load_patcher_tpat_cmd = 'V';
  static final int fts_load_patcher_dpat_cmd = 'Z';

  static final int fts_declare_abstraction_cmd      = 'E';
  static final int fts_declare_abstraction_path_cmd = 'F';

  static final int fts_download_patcher_cmd = 'D';
  static final int fts_download_object_cmd = 'G';
  static final int fts_download_connection_cmd = 'H';

  static final int fts_open_patcher_cmd = 'O';
  static final int fts_close_patcher_cmd = 'C';
  static final int fts_patcher_loaded_cmd = 'L';




  static final int fts_new_object_cmd = 'n';
  static final int fts_new_abstraction_cmd = 'A';
  static final int fts_redefine_patcher_cmd = 'Q';
  static final int fts_reposition_inlet_cmd = 'X';
  static final int fts_reposition_outlet_cmd = 'Y';

  static final int fts_free_object_cmd = 'f';
  static final int fts_connect_objects_cmd = 'c';
  static final int fts_disconnect_objects_cmd = 'd';

  static final int fts_message_cmd = 'm';
  static final int fts_named_message_cmd = 'q';

  static final int fts_put_property_cmd = 'p';
  static final int fts_get_property_cmd = 'g';

  static final int fts_property_value_cmd = 'P';
  static final int fts_server_message_cmd = 'M';

  static final int fts_update_group_start_cmd = '{';
  static final int fts_update_group_end_cmd = '}';

  static final int fts_shutdown_cmd = 's';

  static public String getCommandName(int command)
  {
    switch (command)
      {
      case 'u':
	return "ucs";
      case '>':
	return "sync";
      case '<':
	return "sync_done";
      case 'h':
	return "post";
      case 'S':
	return "fts_save_patcher_bmax";
      case 'T':
	return "fts_save_patcher_tpat";
      case 'U':
	return "fts_load_patcher_bmax";
      case 'V':
	return "fts_load_patcher_tpat";
      case 'Z':
	return "fts_load_patcher_dpat";
      case 'E':
	return "fts_declare_abstraction";
      case 'F':
	return "fts_declare_abstraction_path";
      case 'D':
	return "fts_download_patcher";
      case 'G':
	return "fts_download_object";
      case 'H':
	return "fts_download_connection";
      case 'O':
	return "fts_open_patcher";
      case 'C':
	return "fts_close_patcher";
      case 'L':
	return "fts_patcher_loaded";
      case 'n':
	return "fts_new_object";
      case 'A':
 	return "fts_new_abstraction";
      case 'Q':
	return "fts_redefine_patcher";
      case 'X':
	return "fts_reposition_inlet";
      case 'Y':
	return "fts_reposition_outlet";
      case 'f':
	return "fts_free_object";
      case 'c':
	return "fts_connect_objects";
      case 'd':
	return "fts_disconnect_objects";
      case 'm':
	return "fts_message";
      case 'q':
	return "fts_named_message";
      case 'p':
	return "fts_put_property";
      case 'g':
	return "fts_get_property";
      case 'P':
	return "fts_property_value";
      case 'M':
	return "fts_server_message";
      case '{':
	return "fts_update_group_start";
      case '}':
	return "fts_update_group_end";
      case '!':
	return "fts_remote_call";
      default:
	return "(unknown command)";
      }
  }
}







    
