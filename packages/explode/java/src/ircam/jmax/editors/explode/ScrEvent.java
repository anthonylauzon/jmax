
package ircam.jmax.editors.explode;

/**
 * The class representing a generic event in the score
 */
public class ScrEvent {

  /**
   * default constructor.
   * It provides an event with a default set of parameters
   */
  public ScrEvent(ExplodeDataModel explodeData) 
  {
    itsExplodeDataModel = explodeData;
    itsTime = DEFAULT_TIME;
    itsPitch = DEFAULT_PITCH;
    itsVelocity = DEFAULT_VELOCITY;
    itsDuration = DEFAULT_DURATION;
    itsChannel = DEFAULT_CHANNEL;
  }


  /**
   * constructor with all the parameters
   */
  public ScrEvent(ExplodeDataModel explodeDb, 
		  int theTime, int thePitch, int theVelocity, int theDuration, int theChannel) 
  {
    itsExplodeDataModel = explodeDb;
    itsTime = theTime;
    itsPitch = thePitch;
    itsVelocity = theVelocity;
    itsDuration = theDuration;
    itsChannel = theChannel;
  }

  /**
   * get the starting time of this event
   */
  public final int getTime() 
  {
    return itsTime;
  }

  /**
   * get the pitch of this event
   */
  public final int getPitch() 
  {
    return itsPitch;
  }

  /**
   * the MIDI velocity associated to this event
   */
  public final int getVelocity() 
  {
    return itsVelocity;
  }

  /**
   * get the MIDI channel associated to this event
   */
  public final int getChannel() 
  {
    return itsChannel;
  }

  /**
   * get the duration of this event
   */
  public final int getDuration() 
  {
    return itsDuration;
  }

  
  /* the corresponding set functions.. */

  public final void setTime(int time) 
  {
    itsTime = time;

    if (itsExplodeDataModel != null)
      itsExplodeDataModel.moveEvent(this);
  }

  public final void setPitch(int pitch) 
  {
    itsPitch = pitch;

    if (itsExplodeDataModel != null)
      itsExplodeDataModel.changeEvent(this);
  }

  public final void setDuration(int duration) 
  {
    itsDuration = duration;

    if (itsExplodeDataModel != null)
      itsExplodeDataModel.changeEvent(this);
  }

  public final void setVelocity(int theVelocity) 
  {
    itsVelocity = theVelocity;

    if (itsExplodeDataModel != null)
      itsExplodeDataModel.changeEvent(this);
  }

  public final void setChannel(int theChannel) 
  {
    itsChannel = theChannel;

    if (itsExplodeDataModel != null)
      itsExplodeDataModel.changeEvent(this);
  }

  //------------ Fields

  int itsTime;
  int itsPitch;
  int itsVelocity;
  int itsDuration;
  int itsChannel;

  /** Back pointer to the data base; if not null,
   *  all the change are reported to the data base
   */

  ExplodeDataModel itsExplodeDataModel; 


  public static int DEFAULT_TIME = 0;
  public static int DEFAULT_PITCH = 64;
  public static int DEFAULT_VELOCITY = 64;
  public static int DEFAULT_DURATION = 100;
  public static int DEFAULT_CHANNEL = 0;
}



