
package ircam.jmax.editors.explode;

import ircam.jmax.fts.*;
import java.util.Enumeration;

/**
 * the functionalities of the model for the explode data. 
 * ExplodeRemoteData implements it.
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
   * access the first event whose starting time is 
   * after a given time
   */
  public abstract int indexOfFirstEventStartingAfter(int time);


  /**
   * access the first event whose ENDING time is 
   * after a given time
   */
  public abstract int indexOfFirstEventEndingAfter(int time);


  /**
   * access the last event whose ENDING time is 
   * before a given time
   */
  public abstract int indexOfLastEventEndingBefore(int time);

  /**
   * access the last event whose starting time is 
   * before a given time
   */
  public abstract int indexOfLastEventStartingBefore(int time);


  /**
   * the enumeration of all the events active at a given time value
   */
  public abstract Enumeration eventsLivingAt(int time);


  /**
   * adds an event in the database
   */
  public abstract void addEvent(ScrEvent theEvent);

  /**
   * change an event in the database
   */
  public abstract void changeEvent(ScrEvent event);

  /**
   * move an event in the database
   */
  public abstract void moveEvent(ScrEvent event);

  /**
   * deletes an event from the database
   */
  public abstract void removeEvent(ScrEvent theEvent);


  /*
   */

  /**
   * requires to be notified when the database changes
   */
  public void addListener(ExplodeDataListener theListener);

  /**
   * removes the listener
   */
  public abstract void removeListener(ExplodeDataListener theListener);


}



