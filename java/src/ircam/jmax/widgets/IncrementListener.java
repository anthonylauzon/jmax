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
package ircam.jmax.widgets;

/**
 * The interface associated to the IncrementController objects */
public interface IncrementListener {
  /** called when the user pressed the increment button */
  public abstract void increment();
  /** called when the user pressed the decrement button */
  public abstract void decrement();

}
