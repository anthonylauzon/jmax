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

package ircam.jmax.toolkit;

/**
 * The interface associated to the DirectionChooser interface module.
 * Used by objects that need to be informed on the direction choosed
 * by the user with the mouse.
 */
public interface DirectionListener {

  /** The user dragged the mouse in the resulting direction 
   */
  public abstract void directionChoosen(int theDirection);

  /** the user aborted the process (mouseReleased)
   */
  public abstract void directionAbort();

  //--- Fields
  public static final int HORIZONTAL_MOVEMENT = DirectionChooser.HORIZONTAL_MOVEMENT;
  public static final int VERTICAL_MOVEMENT = DirectionChooser.VERTICAL_MOVEMENT;
}


