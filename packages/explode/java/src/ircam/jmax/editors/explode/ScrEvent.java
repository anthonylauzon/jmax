
package ircam.jmax.editors.explode;

import ircam.jmax.toolkit.*;

/**
 * The class representing a generic event in the score
 */
public class ScrEvent extends AbstractUndoableObject {


  /**
   * constructor for bean inspector (temporary)
   */
  public ScrEvent()
  {
    itsTime = DEFAULT_TIME;
    itsPitch = DEFAULT_PITCH;
    itsVelocity = DEFAULT_VELOCITY;
    itsDuration = DEFAULT_DURATION;
    itsChannel = DEFAULT_CHANNEL;
  }
 
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

  public final ExplodeDataModel getDataModel()
  {
    return itsExplodeDataModel;
  }  

  /* the corresponding set functions.. */

  public final void setTime(int time) 
  {

    if (itsExplodeDataModel != null)
      {
	if (itsExplodeDataModel.isInGroup())
	  itsExplodeDataModel.postEdit(new UndoableEventTransformation(this));
      }

    itsTime = time;
    
    if (itsExplodeDataModel != null)
      {
	itsExplodeDataModel.moveEvent(this);
      }
  }


  public final void setPitch(int pitch) 
  {

    if (itsExplodeDataModel != null)
      {
	if (itsExplodeDataModel.isInGroup())
	  itsExplodeDataModel.postEdit(new UndoableEventTransformation(this));
      }

    itsPitch = pitch;

    if (itsExplodeDataModel != null)
      {
	itsExplodeDataModel.changeEvent(this);	
      }
  }

  public final void setDuration(int duration) 
  {

    if (itsExplodeDataModel != null)
      {
	if (itsExplodeDataModel.isInGroup())
	itsExplodeDataModel.postEdit(new UndoableEventTransformation(this));
      }

    itsDuration = duration;

    if (itsExplodeDataModel != null)
      {
	itsExplodeDataModel.changeEvent(this);
      }
  }
  
  public final void setVelocity(int theVelocity) 
  {

    if (itsExplodeDataModel != null)
      {
	if (itsExplodeDataModel.isInGroup())
	  itsExplodeDataModel.postEdit(new UndoableEventTransformation(this));
      }

    itsVelocity = theVelocity;

    if (itsExplodeDataModel != null)
      {
	itsExplodeDataModel.changeEvent(this);
      }
  }

  public final void setChannel(int theChannel) 
  {

    if (itsExplodeDataModel != null)
      {
	if (itsExplodeDataModel.isInGroup())
	  itsExplodeDataModel.postEdit(new UndoableEventTransformation(this));
      }

    itsChannel = theChannel;

    if (itsExplodeDataModel != null)
      {
	itsExplodeDataModel.changeEvent(this);
      }

  }

  /** Undoable data interface */
  public void beginUpdate()
  {
    itsExplodeDataModel.beginUpdate();
  }

  public void endUpdate()
  {
    itsExplodeDataModel.endUpdate();
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




