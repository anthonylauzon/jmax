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
 * a mapper into the time value of the ScrEvents
 */
public class ChannelMapper extends Mapper {
  
  /**
   * set the given channel in the given event
   */
  public void set(ScrEvent e, int value) 
  {
    e.setChannel(value);
  }
  
  /**
   * get the channel of the given event
   */
  public int get(ScrEvent e) 
  {
    return e.getChannel();
  }

  /**
   * access the static instance
   */
  static public Mapper getMapper()
  {
    if (itsChannelMapper == null)
      itsChannelMapper = new ChannelMapper();
    
    return itsChannelMapper;
  }

  //--- Fields

  static ChannelMapper itsChannelMapper;
}

