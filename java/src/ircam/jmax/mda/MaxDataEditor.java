package ircam.jmax.mda;

/** This interface  is the abstraction for any
 *  program, bean, class, that want to edit an instance of MaxData.
 * It do not correspond in general to a "Window"; this interface just
 * abstract over those things related to the mda architecture.
 *
 * Note that no events are defined in this interface; there are "Listener"
 * interface defined for this.
 */

interface MaxDataEditor
{
  /** This method set the data the editor should edit;
   *  the editor can throw an exception if already set, or can implement
   *  a change of the content.
   */

  abstract public void editData(MaxData data) throws MaxDataException;

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

  /** Tell the editor the data has changed; it pass a sigle Java
   * Object that may code what is changed and where; if the argument
   * is null, means usually that all the data is changed
   */

  abstract public void dataChanged(Object reason);
}
