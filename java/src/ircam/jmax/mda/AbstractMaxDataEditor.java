//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
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





