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

/**
 * The interface for the classes that can own a selection (ex,
 * editor's window */
public interface SelectionOwner {
  // implementation notes: This class is used in the Explode package and
  // is currently not used by the toolkit's selection support 
  // (SelectionHandler, SelectionListener, AbstractSelection classes). 
  // The toolkit does not support selection ownership at all (but it should!)
  /**
   * The selection owned by this object has been disactivated */
  public abstract void selectionDisactivated();
  /**
   * The selection owned by this object has been activated */
  public abstract void selectionActivated();
}
