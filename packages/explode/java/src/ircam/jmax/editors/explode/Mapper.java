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
 * The base class of mappings between score parameters and graphic
 * parameters. A mapper (ex. TimeMapper) handles one and only one
 * score parameter. Different mappers can be associated with
 * different graphic parameters (example, x to time, y to pitch).
 */
public abstract class Mapper {

  /**
   * set the given value in the given event
   */
  public abstract void set(ScrEvent e, int val);


  /**
   * get the value from the given event
   */
  public abstract int get(ScrEvent e);


}

