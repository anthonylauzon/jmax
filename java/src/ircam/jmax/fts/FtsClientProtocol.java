//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

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

  static final int string_code          = 0x01;
  static final int string_end_code      = 0x02;
  static final int int_code             = 0x03;
  static final int float_code           = 0x04;
  static final int symbol_cached_code   = 0x05;
  static final int symbol_and_def_code  = 0x06;
  static final int symbol_code          = 0x07;
  static final int object_code          = 0x08;
  static final int connection_code      = 0x09;
  static final int data_code            = 0x0a;
  static final int end_of_message       = 0x0b;
  
  // predicated coding protocol character sets

  static boolean tokenStartingChar(int c)
  {
    return (
	    (c == int_code) ||
	    (c == float_code)   ||
	    (c == symbol_code)   ||
	    (c == symbol_and_def_code)   ||
	    (c == object_code)   ||
	    (c == connection_code) || 
	    (c == data_code)   ||
	    (c == string_code) ||
	    (c == end_of_message)
	     );
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
    return c == end_of_message;
  }

  // Protocol defined commands code

  static final int ucs_cmd = 'u';
  static final int sync_cmd = '>';
  static final int sync_done_cmd = '<';
  static final int post_cmd = 'h';
  static final int post_line_cmd = 'L';

  static final int remote_call = '!';

  static final int fts_save_patcher_bmax_cmd = 'S';

  static final int fts_load_patcher_bmax_cmd = 'U';
  static final int fts_load_patcher_dpat_cmd = 'Z';

  static final int fts_declare_abstraction_cmd      = 'E';
  static final int fts_declare_abstraction_path_cmd = 'F';

  static final int fts_declare_template_cmd      = 'y';
  static final int fts_declare_template_path_cmd = 'w';

  static final int fts_download_object_cmd = 'G';
  static final int fts_download_connection_cmd = 'H';

  static final int fts_new_object_cmd = 'n';
  static final int fts_new_object_var_cmd = 'N';
  static final int fts_delete_object_cmd = 'f';

  static final int fts_redefine_patcher_cmd = 'Q';
  static final int fts_redefine_object_cmd = 'R';
  static final int fts_redefine_connection_cmd = 'V';
  static final int fts_reposition_inlet_cmd = 'X';
  static final int fts_reposition_outlet_cmd = 'Y';

  static final int fts_delete_connection_cmd = 'a';
  static final int fts_new_connection_cmd   = 'c';

  static final int fts_release_connection_cmd = 'd';
  static final int fts_release_object_cmd = 'l';
  static final int fts_release_object_data_cmd = 'b';

  static final int fts_message_cmd = 'm';

  static final int fts_put_property_cmd = 'p';
  static final int fts_get_property_cmd = 'g';
  static final int fts_get_all_property_cmd = 'B';

  static final int fts_property_value_cmd = 'P';
  static final int fts_server_message_cmd = 'M';

  static final int fts_update_group_start_cmd = '{';
  static final int fts_update_group_end_cmd = '}';
  static final int fts_recompute_error_objects_cmd = 'e';
  static final int fts_shutdown_cmd = 's';

  /** Decode a command. For debug purposes */

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
      case 'U':
	return "fts_load_patcher_bmax";
      case 'Z':
	return "fts_load_patcher_dpat";
      case 'E':
	return "fts_declare_abstraction";
      case 'F':
	return "fts_declare_abstraction_path";
      case 'y':
	return "fts_declare_template";
      case 'w':
	return "fts_declare_template_path";
      case 'G':
	return "fts_download_object";
      case 'H':
	return "fts_download_connection";
      case 'n':
	return "fts_new_object";
      case 'N':
	return "fts_new_object_var";
      case 'Q':
	return "fts_redefine_patcher";
      case 'R':
	return "fts_redefine_object";
      case 'V':
	return "fts_redefine_connection";
      case 'X':
	return "fts_reposition_inlet";
      case 'Y':
	return "fts_reposition_outlet";
      case 'f':
	return "fts_delete_object";
      case 'c':
	return "fts_new_connection";
      case 'a':
	return "fts_delete_connection";
      case 'd':
	return "fts_release_connection";
      case 'l':
	return "fts_release_object";
      case 'b':
	return "fts_release_object_data_cmd";
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
	return "(unknown command: " + String.valueOf((char) command) + " )";
      }
  }
}







    
