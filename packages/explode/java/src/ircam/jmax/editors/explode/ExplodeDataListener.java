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
 * The interface of the objects that want to be called back
 * when the explode data changes. 
 */
public interface ExplodeDataListener {
  // Implementation notes: this interface should be enriched:
  // Furthermore, the "low level" objectMoved call is used
  // to make life easier for entities that depends on indexes
  // (Ex: ExplodeSelection)
  /**
   * callbacks
   */

  abstract public void objectDeleted(Object whichObject, int oldIndex);
  abstract public void objectAdded(Object whichObject, int index);
  abstract public void objectChanged(Object whichObject);
  abstract public void objectMoved(Object whichObject, int oldIndex, int newIndex);
}

