package ircam.jmax.mda;

/** This interface  is the abstraction for any
 *  program, bean, class, that want to edit a data instance part of of MaxDocument.
 * It do not correspond in general to a "Window"; this interface just
 * abstract over those things related to the mda architecture.
 *
 * Note that no events are defined in this interface; there are "Listener"
 * interface defined for this.
 */

public interface MaxDataEditor
{
  /**
   * This method is called when an edit is asked
   * for the data the editor is already editing; the semantic
   * is editor dependent.
   */

  abstract public void reEdit();

  /**
   * This method return the data instance the editor is editing
   */

  abstract public MaxData getData();
       
  /** This method ask to the stop editing the data.
   *  This probabily means that the data item or the editor is about to
   * be disposed (destroyed).
   *
   */

  abstract public void quitEdit();

  /** Tell the editor to syncronize, i.e. to store in the
   * data all the information possibly cached in the editor
   * and still not passed to the data instance; this usually
   * happen before saving an instance.
   */

  abstract public void syncData();

  /*
   * Tell the editor to show a specif piece of data, here
   * represented by a Java Object.
   */

  abstract public void showObject(Object object);
}





