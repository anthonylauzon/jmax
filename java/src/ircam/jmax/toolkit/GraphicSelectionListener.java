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

/**
 * Interface of the objects that wants to listen to a geometric
 * selection in a plane
 */ 
public interface GraphicSelectionListener {
  
  /** a rectangular selection have been choosen */
  public abstract void selectionChoosen(int x, int y, int w, int h);

  /** a point have been choosen */
  public abstract void selectionPointChoosen(int x, int y, int modifiers);
  /** a point have been choosen */
  public abstract void selectionPointDoubleClicked(int x, int y, int modifiers);
}


