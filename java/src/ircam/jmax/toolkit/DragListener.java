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

package ircam.jmax.toolkit;

import java.awt.event.MouseEvent;
/**
 * The interface used by objects that listen to dragging operations,
 * such as the moving tools. The information passed back is limited 
 * to the end of the interaction.
 * Use DynamicDragListener for richer set of informations.
 * @see SelectionMover
 * @see DynamicDragListener 
 */
public interface DragListener {
  
  /**
   * informs that the drag ended at the point x,y
   */
  public abstract void dragEnd(int x, int y, MouseEvent e);
  public abstract void updateStartingPoint(int deltaX, int deltaY);
}



