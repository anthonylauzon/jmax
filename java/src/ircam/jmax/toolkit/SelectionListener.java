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
package ircam.jmax.toolkit;

/** Interface of the objects that wants to listen to a selection's content.
 * This interface should probably be enriched (see the swing List Listeners)  
 */ 

public interface SelectionListener {

  /** a single objects have been selected.
   */
  public abstract void objectSelected();

  /** a group of object have been selected. 
   */
  public abstract void groupSelected();

  /** a single object have been deselected.
   */
  public abstract void objectDeselected();

  /** a group of objects have been deselected.
   */
  public abstract void groupDeselected();
}

