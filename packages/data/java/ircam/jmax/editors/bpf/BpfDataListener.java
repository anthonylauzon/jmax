//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.editors.bpf;

/**
 * The interface of the objects that want to be called back
 * when the sequence data changes. 
 */
public interface BpfDataListener {
  // Implementation notes: this interface should be enriched:
  // Furthermore, the "low level" objectMoved call is used
  // to make life easier for entities that depends on indexes
  // (Ex: SequenceSelection)
  /**
   * callbacks
   */
  abstract public void pointsDeleted(int index, int size);
  abstract public void pointAdded(int index);
  abstract public void pointChanged(int oldIndex, int newIndex, float newTime, float newValue);
  abstract public void pointsChanged();
  abstract public void cleared();
  abstract public void nameChanged( String name);
}

