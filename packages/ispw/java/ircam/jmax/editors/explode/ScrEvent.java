//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.editors.explode;

import ircam.jmax.toolkit.*;

/**
 * The class representing a generic event in the score
 */
public class ScrEvent extends AbstractUndoableObject implements /* TwoOrderObject,*/ Cloneable{


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

//    // TwoOrderObject interface. It makes a ScrEvent suitable
//    // for the use in a TDTree (future implementation?) 
//    public boolean firstLessOrEqual(TwoOrderObject obj)
//    {
//      return getFirst()<=obj.getFirst();
//    }

//    public boolean secondLessOrEqual(TwoOrderObject obj)
//    {
//      return getSecond()<=obj.getSecond();
//    }
 
//    public int getFirst()
//    {
//      return getTime();
//    }
  
//    public int getSecond()
//    {
//      return getTime()+getDuration();
//    }
  

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
    itsTime = time;
  }

  /**
   * this is the method that must be called by the editors to
   * change the initial time of an event. It takes care of
   * keeping the data base consistency */
  public void move(int time)
  {
    if (time < 0) time = 0;
    if (itsExplodeDataModel != null)
      {
	if (((UndoableData) itsExplodeDataModel).isInGroup())
	  ((UndoableData) itsExplodeDataModel).postEdit(new UndoableMove(this));
	
	itsExplodeDataModel.moveEvent(this, time);
      }
    else setTime(time);
  }

  public final void setPitch(int pitch) 
  {

    if (pitch < 0) pitch = 0;
    if (itsExplodeDataModel != null)
      {
	if (((UndoableData) itsExplodeDataModel).isInGroup())
	  ((UndoableData) itsExplodeDataModel).postEdit(new UndoableEventTransformation(this));
      }

    itsPitch = pitch;

    if (itsExplodeDataModel != null)
      {
	itsExplodeDataModel.changeEvent(this);	
      }
  }

  public final void setDuration(int duration) 
  {

    if (duration < 0) duration = 0;
    if (itsExplodeDataModel != null)
      {
	if (((UndoableData) itsExplodeDataModel).isInGroup())
	((UndoableData) itsExplodeDataModel).postEdit(new UndoableEventTransformation(this));
      }

    itsDuration = duration;

    if (itsExplodeDataModel != null)
      {
	itsExplodeDataModel.changeEvent(this);
      }
  }
  
  public final void setVelocity(int velocity) 
  {

    if (velocity <0) velocity = 0;
    if (itsExplodeDataModel != null)
      {
	if (((UndoableData) itsExplodeDataModel).isInGroup())
	  ((UndoableData) itsExplodeDataModel).postEdit(new UndoableEventTransformation(this));
      }

    itsVelocity = velocity;

    if (itsExplodeDataModel != null)
      {
	itsExplodeDataModel.changeEvent(this);
      }
  }

  public final void setChannel(int channel) 
  {

    if (channel < 0) channel = 0;
    if (channel > 15) channel = 15;
    if (itsExplodeDataModel != null)
      {
	if (((UndoableData) itsExplodeDataModel).isInGroup())
	  ((UndoableData) itsExplodeDataModel).postEdit(new UndoableEventTransformation(this));
      }

    itsChannel = channel;

    if (itsExplodeDataModel != null)
      {
	itsExplodeDataModel.changeEvent(this);
      }

  }


  public void setDataModel(ExplodeDataModel theDataModel)
  {
    itsExplodeDataModel = theDataModel;
  }


  /** Undoable data interface */
  public void beginUpdate()
  {
    ((UndoableData) itsExplodeDataModel).beginUpdate();
  }

  public void endUpdate()
  {
    ((UndoableData) itsExplodeDataModel).endUpdate();
  }


  ScrEvent duplicate () throws CloneNotSupportedException
  {
    return (ScrEvent) clone();
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




