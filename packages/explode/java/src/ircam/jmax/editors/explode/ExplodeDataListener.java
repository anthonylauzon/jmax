package ircam.jmax.editors.explode;

/**
 * The interface of the objects that want to be called back
 * when the explode data changes
 */
public interface ExplodeDataListener {

  /**
   * callbacks
   */
  abstract public void dataChanged(Object spec);

  abstract public void objectDeleted(Object whichObject);
}

