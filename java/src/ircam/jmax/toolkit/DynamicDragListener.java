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

import java.awt.event.*;

/**
 * The interface of the objects that wants to 
 * dynamically follow drag operations. Unlike the DragListener interface,
 * this IM tracks the beginning and every movement, not just the end.
 * @see DragListener 
 * @see InteractionModule*/
public interface DynamicDragListener extends DragListener {
  
  /**
   * Begin the drag */
  public abstract void dragStart(int x, int y, MouseEvent e);

  /**
   * Continue the drag...*/
  public abstract void dynamicDrag(int x, int y, MouseEvent e);

}
