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

import java.util.Enumeration;

/**
 * The interface implemented by the handlers of generic selections of objects.
 * It makes no assumption on the kind of objects and on the model of data.
 */
public interface SelectionHandler {

  /** select the given object 
   */
  public abstract void select(Object obj);
  
  /** select the given enumeration of objects.
   * When possible, use this method instead of
   * selecting single objects. 
   */
  public abstract void select(Enumeration e);

  /** remove the given object from the selection
   */
  public abstract void deSelect(Object obj);


  /** remove the given enumeration of objects from the selection
   * When possible, use this method instead of
   * deselecting single objects.    */
  public abstract void deSelect(Enumeration e);


  /** returns true if the object is currently selected
   */
  public abstract boolean isInSelection(Object obj);
  

  /** returns an enumeration of all the selected objects
   */
  public abstract Enumeration getSelected();
  

  /** returns the number of objects in the selection
   */
  public abstract int size();


  /** selects all the objects. This method actually
   * depend on the concrete model used.
   */
  public abstract void selectAll();

  /** deselects all the objects currently selected
   */
  public abstract void deselectAll();


  /** ask to be informed when the selection's content changes
   */
  public abstract void addSelectionListener(SelectionListener theListener);


  /** remove the given listener
   */
  public abstract void removeSelectionListener(SelectionListener theListener);

}









