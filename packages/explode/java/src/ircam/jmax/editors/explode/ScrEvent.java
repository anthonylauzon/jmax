
package ircam.jmax.editors.explode;

/**
 * The class representing a generic event in the score
 */
public class ScrEvent {
  int itsTime;
  int itsPitch;
  int itsVelocity;
  int itsDuration;
  int itsChannel;
  int itsSomethingElse;
  static int DEFAULT_VALUE = -1;


  public ScrEvent() {
    itsTime = DEFAULT_VALUE;
    itsPitch = DEFAULT_VALUE;
    itsVelocity = DEFAULT_VALUE;
    itsDuration = DEFAULT_VALUE;
    itsChannel = DEFAULT_VALUE;
    itsSomethingElse = DEFAULT_VALUE;
  }

  /**
   * constructor with all the parameters
   */
  public ScrEvent(int theTime, int thePitch, int theVelocity, int theDuration, int theSomething) {
    itsTime = theTime;
    itsPitch = thePitch;
    itsVelocity = theVelocity;
    itsDuration = theDuration;
    itsSomethingElse = theSomething;
    
  }

  /**
   * the starting time of this event
   */
  public int getTime() {
    return itsTime;
  }

  /**
   * the pitch of this event
   */
  public int getPitch() {
    return itsPitch;
  }

  /**
   * the MIDI velocity associated to this event
   */
  public int getVelocity() {
    return itsVelocity;
  }

  /**
   * the MIDI channel associated to this event
   */
  public int getChannel() {
    return itsChannel;
  }

  /**
   * the duration of this event
   */
  public int getDuration() {
    return itsDuration;
  }

  /* the corresponding set functions.. */
  public void setTime(int time) {
    itsTime = time;
  }

  public void setPitch(int pitch) {
    itsPitch = pitch;
  }

  public void setDuration(int duration) {
    itsDuration = duration;
  }

  public void setVelocity(int theVelocity) {
    itsVelocity = theVelocity;
  }

  public void setChannel(int theChannel) {
    itsChannel = theChannel;
  }

}



