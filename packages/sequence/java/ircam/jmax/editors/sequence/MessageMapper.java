//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.editors.sequence;

import ircam.jmax.editors.sequence.track.*;

/**
 * A simple Mapper that accesses the Integer parameter
 */ 
public class MessageMapper extends StringMapper {
  
  /**
   * set the Integer of this event
   */
  public void set(Event e, String mess) 
  {
    e.setProperty("message", mess);
  }

  /**
   * get the Integer of this event
   */
  public String get(Event e) 
  {
    return (String)e.getProperty("message");
  }

  public String getName()
  {
    return "message";
  }

  /**
   * access the static instance
   */
  public static StringMapper getMapper() 
  {
    if (itsMessageMapper == null)
       itsMessageMapper = new MessageMapper();
    return itsMessageMapper;
  }

  //--- Fields

  static MessageMapper itsMessageMapper;
}





