//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
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
 * a mapper into the velocity value of the ScrEvents
 */
public class VelocityMapper extends Mapper {
  
  /**
   * set the given velocity in the given event
   */
  public void set(ScrEvent e, int value) 
  {
    e.setVelocity(value);
  }
  
  /**
   * get the velocity of the given event
   */
  public int get(ScrEvent e) 
  {
    return e.getVelocity();
  }

  static public Mapper getMapper()
  {
    return itsVelocityMapper;
  }

  //--- Fields

  static VelocityMapper itsVelocityMapper = new VelocityMapper();
}
