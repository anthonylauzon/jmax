//
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.protocol;

public class ProtocolDecoder {

  public static final int INT_TOKEN = 42;
  public static final int STRING_TOKEN = 43;
  public static final int EOM_TOKEN = 44;
  public static final int RUNNING = 45;

  private static final int STATE_IN_TYPE = 1;
  private static final int STATE_IN_INT_0 = 2;
  private static final int STATE_IN_INT_1 = 3;
  private static final int STATE_IN_INT_2 = 4;
  private static final int STATE_IN_INT_3 = 5;
  private static final int STATE_IN_STRING = 6;
  
  private int state = STATE_IN_TYPE;

  private int int_value;
  private StringBuffer string_value;

  /**
   * Decodes the next token
   * @param b the current byte in received message
   * @return INT_TOKEN if decoded token is an int. It can then be retrieved by getInt
   * @return STRING_TOKEN if decoded token is a string. It can then be retrieved by getString
   * @return EOM_TOKEN if decoded token is end of message.
   */
  public int run( byte b)
  {
    return EOM_TOKEN;
  }

  /**
   * Returns the current token, assumed it is an int
   * @return   the token value
   */
  public final int getInt()
  {
    return 0;
  }

  /**
   * Returns the current token, assumed it is a string
   * @return   the token value
   */
  public final String getString()
  {
    return "none";
  }
}
