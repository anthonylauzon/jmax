//
//  jMax
//  
//  Copyright (C) 1999 by IRCAM
//  All rights reserved.
//  
//  This program may be used and distributed under the terms of the 
//  accompanying LICENSE.
// 
//  This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
//  for DISCLAIMER OF WARRANTY.
//  


package ircam.jmax.utils;

/**
 * The interface of the objects that can be connected to an oscilloscope.
 * The getValue method will be called each time the oscilloscope needs a 
 * new value to display (the rate depends from the freqence of the oscilloscope)
 * 
 */
public interface OscillSource {
  abstract public int getValue();
}

