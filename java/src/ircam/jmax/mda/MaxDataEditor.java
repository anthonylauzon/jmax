//
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.mda;

/** 
 * A Editor to edit MaxData.
 * This interface  is the abstraction for any
 * program, bean, class, that want to edit a data instance part of of MaxDocument.
 * It do not necessarly correspond in general to a "Window"; this interface just
 * abstract over those things related to the mda architecture; an editor can be 
 * a Batch function, or a background process without user interface.
 *
 * Note that no events are defined in this interface; there are "Listener"
 * interface defined for this.
 */

public interface MaxDataEditor
{
  /**
   * Restart editing.
   * This method is called when an edit is asked
   * for the data the editor is already editing; the semantic
   * is editor dependent.
   */

  abstract public void reEdit();

  /**
   * This method return the data instance the editor is editing.
   */

  abstract public MaxData getData();
       
  /** This method ask to the stop editing the data.
   *  This probabily means that the data item or the editor is about to
   * be disposed (destroyed).
   */

  abstract public void quitEdit();

  /*
   * Tell the editor to show a specif piece or location of data, here
   * represented by a Java Object; the exact semantic of the argument
   * is editor dependent.
   */

  abstract public void showObject(Object object);

  /**
   * Add an editorReady listener.
   * The correct default implementation (for a synchronius editor)
   * is to call l.editorReady(this) immediately.
   * Use AbstractMaxDataEditor to build a new one easily.
   */

  abstract void addEditorReadyListener(MaxDataEditorReadyListener l);
  abstract void removeEditorReadyListener(MaxDataEditorReadyListener l);
}





