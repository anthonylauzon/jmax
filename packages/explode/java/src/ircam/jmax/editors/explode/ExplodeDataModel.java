
package ircam.jmax.editors.explode;

import ircam.jmax.fts.*;
import java.util.Enumeration;

/**
 * the model for the explode data. ExplodeRemoteData implements it.
 */
public interface ExplodeDataModel {

  /**
   * how many events in the database?
   */
  public abstract int length();
  

  /**
   * returns an enumeration of all the events
   */
  public abstract Enumeration getEvents();
  
  /**
   * returns a given event
   */
  public abstract ScrEvent getEventAt(int index);

  /**
   * adds an event in the database
   */
  public abstract void addEvent(ScrEvent theEvent);


  /**
   * deletes an event from the database
   */
  public abstract void removeEvent(ScrEvent theEvent);

  /**
   * requires to be notified when the database changes
   */
  public void addListener(ExplodeDataListener theListener);

  /**
   * removes the listener
   */
  public abstract void removeListener(ExplodeDataListener theListener);


}


