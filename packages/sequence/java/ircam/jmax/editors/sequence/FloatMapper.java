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

package ircam.jmax.editors.sequence;

import ircam.jmax.editors.sequence.track.*;

/**
 * A simple Mapper that accesses the Float parameter
 */ 
public class FloatMapper {
  
  /**
   * set the Integer of this event
   */
  public void set(Event e, float value) 
  {
    e.setProperty("value", new Float(value));
  }

  /**
   * get the Integer of this event
   */
  public float get(Event e) 
  {
    return ((Float)e.getProperty("value")).floatValue();
  }

  public String getName()
  {
    return "value";
  }

  /**
   * access the static instance
   */
  public static FloatMapper getMapper() 
  {
    if (itsFloatMapper == null)
       itsFloatMapper = new FloatMapper();
    return itsFloatMapper;
  }

  //--- Fields

  static FloatMapper itsFloatMapper;
}





