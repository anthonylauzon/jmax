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
package ircam.jmax.editors.table;

/**
 * The interface of the objects that want to be called back
 * when the table data changes
 */
public interface TableDataListener {

  /**
   * The data between the two indexes (included) have changed. */
  abstract public void valueChanged(int index1, int index2);

  /** The data at the given index has changed */
  abstract public void valueChanged(int index);
}

