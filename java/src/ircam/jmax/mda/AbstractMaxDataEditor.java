package ircam.jmax.mda;

import ircam.jmax.utils.*;

/** This class is an empty implementation of a MaxDataEditor that
 *  provide empty methods, apart from getData, but handle the editorReadylistener list.
 *  it implement a fireEditorReadyListeners() method.
 */

public abstract class AbstractMaxDataEditor implements  MaxDataEditor
{
  private boolean ready = false;

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

  abstract public MaxData getData();
       
  /** This method ask to the stop editing the data.
   *  This probabily means that the data item or the editor is about to
   * be disposed (destroyed).
   *
   */

  public void quitEdit()
  {
  }

  /** Tell the editor to syncronize, i.e. to store in the
   * data all the information possibly cached in the editor
   * and still not passed to the data instance; this usually
   * happen before saving an instance.
   */

  public void syncData()
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

  public void addEditorReadyListener(MaxEditorReadyListener l)
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

  public void removeEditorReadyListener(MaxEditorReadyListener l)
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
	  ((MaxEditorReadyListener) objects[i]).editorReady(this);

	ready = true;
      }
  }
}





