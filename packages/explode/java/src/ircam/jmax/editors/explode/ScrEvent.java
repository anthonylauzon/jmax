
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


  public ScrEvent() {
    itsTime = 0;
    itsPitch = 64;
    itsVelocity = 64;
    itsDuration = 100;
    itsChannel = 1;
    itsSomethingElse = 0;
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
  public final int getTime() {
    return itsTime;
  }

  /**
   * the pitch of this event
   */
  public final int getPitch() {
    return itsPitch;
  }

  /**
   * the MIDI velocity associated to this event
   */
  public final int getVelocity() {
    return itsVelocity;
  }

  /**
   * the MIDI channel associated to this event
   */
  public final int getChannel() {
    return itsChannel;
  }

  /**
   * the duration of this event
   */
  public final int getDuration() {
    return itsDuration;
  }

  /* the corresponding set functions.. */
  public final void setTime(int time) {
    itsTime = time;
  }

  public final void setPitch(int pitch) {
    itsPitch = pitch;
  }

  public final void setDuration(int duration) {
    itsDuration = duration;
  }

  public final void setVelocity(int theVelocity) {
    itsVelocity = theVelocity;
  }

  public final void setChannel(int theChannel) {
    itsChannel = theChannel;
  }

}



