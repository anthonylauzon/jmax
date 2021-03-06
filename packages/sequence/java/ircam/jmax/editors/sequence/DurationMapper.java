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
 * A simple Mapper that accesses the duration parameter
 */ 
public class DurationMapper extends DoubleMapper/*Mapper*///*@*//
{ 
  /**
   * set the duration of this event
   */
    /*public void set(Event e, int value) 
      {
      e.setProperty("duration", new Integer(value));
      }*/
  public void set(Event e, double value) 
  {
    e.setProperty("duration", new Double(value));
  }
    
    
  /**
   * get the duration of this event
   */
  /*public int get(Event e) 
    {
    return ((Integer)e.getProperty("duration")).intValue();
    }*/
  public double get(Event e) 
  {
    return ((Double)e.getProperty("duration")).doubleValue();
  }
    

  public String getName()
  {
    return "duration";
  }

    /**
     * access the static instance
     */
  static DoubleMapper/*Mapper*/ getMapper()
  {
    if (itsDurationMapper == null)
      itsDurationMapper = new DurationMapper();
    return itsDurationMapper;
  }
  
  //--- Fields
    
  static DurationMapper itsDurationMapper;
}












