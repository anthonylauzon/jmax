//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.editors.explode;

/**
 * a mapper into the time value of the ScrEvents
 */
public class TimeMapper extends Mapper {
  
  /**
   * set the given starting time in the given event
   */
  public void set(ScrEvent e, int value) 
  {
    e.move(value);
  }
  
  /**
   * get the starting time from the given event
   */
  public int get(ScrEvent e) 
  {
    return e.getTime();
  }

  static public Mapper getMapper()
  {
    return itsTimeMapper;
  }

  //--- Fields

  static TimeMapper itsTimeMapper = new TimeMapper();
}
