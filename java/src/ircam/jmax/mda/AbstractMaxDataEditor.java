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

import ircam.jmax.utils.*;

/** This class is an empty implementation of a MaxDataEditor that
 *  provide empty methods, apart from getData, but handle the editorReadylistener list.
 *  it implement a fireEditorReadyListeners() method.
 */

public abstract class AbstractMaxDataEditor implements  MaxDataEditor
{
  MaxData data;
  private boolean ready = false;

  protected AbstractMaxDataEditor(MaxData data)
  {
    this.data = data;
  }

  /**
   * This method is called when an edit is asked
   * for the data the editor is already editing; the semantic
   * is editor dependent.
   */

  public void reEdit()
  {
  }

  /**
   * This method return the data instance the editor is editing
   */

  public MaxData getData()
  {
    return data;
  }
       
  /** This method ask to the stop editing the data.
   *  This probabily means that the data item or the editor is about to
   * be disposed (destroyed).
   *
   */

  public void quitEdit()
  {
  }

  /*
   * Tell the editor to show a specif piece of data, here
   * represented by a Java Object.
   */

  public void showObject(Object object)
  {
  }

  /**
   * Add an editorReady listener
   */

  private MaxVector listeners;

  public void addEditorReadyListener(MaxDataEditorReadyListener l)
  {
    if (ready)
      {
	// Fire now if ready

	l.editorReady(this);
      }
    else
      {
	if (listeners == null)
	  listeners = new MaxVector();

	listeners.addElement(l);
      }
  }

  public void removeEditorReadyListener(MaxDataEditorReadyListener l)
  {
    if (listeners != null)
      listeners.removeElement(l);
  }

  public void fireEditorReadyListeners()
  {
    if (listeners != null)
      {
	Object[] objects = listeners.getObjectArray();

	for (int i = 0; i < listeners.size(); i++)
	  ((MaxDataEditorReadyListener) objects[i]).editorReady(this);
      }

    ready = true;
  }
}





