//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.editors.sequence.track;

/**
 * The interface of the objects that want to be called back
 * when the Track data changes. 
 */
public interface TrackDataListener {
  // Implementation notes: this interface should be enriched:
  // Furthermore, the "low level" objectMoved call is used
  // to make life easier for entities that depends on indexes
  // (Ex: SequenceSelection)
  /**
   * callbacks
   */

  abstract public void objectDeleted(Object whichObject, int oldIndex);
  abstract public void objectAdded(Object whichObject, int index);
  abstract public void objectsAdded(int maxTime);
  abstract public void objectChanged(Object whichObject, String propertyName, Object propertyValue);
  abstract public void objectMoved(Object whichObject, int oldIndex, int newIndex);
  abstract public void lastObjectMoved(Object whichObject, int oldIndex, int newIndex);
  abstract public void trackNameChanged(String oldName, String newName);
  abstract public void trackCleared();
}

