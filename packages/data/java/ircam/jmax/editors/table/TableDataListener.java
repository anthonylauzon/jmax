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

package ircam.jmax.editors.table;

/**
 * The interface of the objects that want to be called back
 * when the table data changes
 */
public interface TableDataListener {

  /**
   * The data between the two indexes (included) have changed. */
  abstract public void valueChanged(int index1, int index2, boolean fromScroll);
  abstract public void pixelsChanged(int index1, int index2);

  abstract public void tableSetted();
  abstract public void tableCleared();
  abstract public void sizeChanged(int size, int oldSize);
}

