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
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 


package ircam.jmax.editors.sequence.track;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.renderers.*;

import ircam.fts.client.*;
import ircam.jmax.toolkit.*;

import java.io.*;
import javax.swing.undo.*;

/**
 * The class representing an event in a track. These objects have a time 
 * and a value (a FtsRemoteData).
 */

public class TrackEvent extends FtsObject implements Event, Drawable, UndoableData, Cloneable
{
  public TrackEvent(FtsServer server, FtsObject parent, int objId, String className, FtsAtom args[], int offset, int length)
  {
    super(server, parent, objId);
	
    this.time = (double)args[offset].floatValue;
    
    EventValue evtValue = (EventValue)(ValueInfoTable.getValueInfo(args[offset+1].symbolValue.toString()).newInstance());
    
    for(int i = 0; i< length-2-offset; i++)
      {
	Object obj = args[offset+2+i].getValue();
	
	if(obj instanceof Float) 
	  obj = new Double(((Float)obj).doubleValue());
	else if(obj instanceof FtsSymbol)
	  obj = ((FtsSymbol)obj).toString();

	evtArgs[i] = obj;	  
      }
    
    evtValue.setPropertyValues(length-2-offset, evtArgs);   
    setValue(evtValue);
  }

  /**
   * Sets the data model this event belongs to */
  public void setDataModel(TrackDataModel model)
  {
    itsTrackDataModel = model;
  }

  /**
   * Gets the data model this event belongs to */
  public TrackDataModel getDataModel()
  {
    return itsTrackDataModel;
  }
  
  /**
   * Get the initial time for this event */
  public double getTime()
  {
    return time;
  }
  
  /**
   * Set the initial time for this event. Use move() when the event is into a 
   * model (i.e. always for the editors, the exception are Events created on-the-fly 
   * by specific intereaction modules), in order to keep the DB consistency. */
  public void setTime(double time)   
  {
    this.time = time;
  }

  /**
   * This is the method that must be called by the editors to
   * change the initial time of an event. It takes care of
   * keeping the data base consistency */
  public void move(double time)
  {
    ((FtsTrackObject)itsTrackDataModel).requestEventMove(this, time);
  }

  public void moveTo(double time)
  {
    if (((UndoableData) itsTrackDataModel).isInGroup())
      ((UndoableData) itsTrackDataModel).postEdit(new UndoableMove(this, time));
    
    itsTrackDataModel.moveEvent(this, time);
  }
    

  public boolean isHighlighted()
  {
    return highlighted;
  }
  public void setHighlighted(boolean hh)
  {
    highlighted = hh;
  }
  /**
   * Set the named property */
  public void setProperty(String name, Object theValue)
  {    
    //int intVal;
    double doubleVal;
    
    if (itsTrackDataModel != null)
      {
	if (((UndoableData) itsTrackDataModel).isInGroup()&&(!name.equals("time")))
	  ((UndoableData) itsTrackDataModel).postEdit(new UndoableEventTransf(this, name, theValue));
      }
	
    if (theValue instanceof Double) 
      {
	doubleVal = ((Double)theValue).doubleValue();
	if (name.equals("time"))
	  setTime(doubleVal);
	else  {
	  if (value != null) value.setProperty(name, theValue); //unknown Double property
	}
      }
    else if (value != null)
      value.setProperty(name, theValue); //unknow not-Integer property, delegate it to the value object
    
    if (itsTrackDataModel != null)
      {
	itsTrackDataModel.changeEvent(this, name, theValue);
      }
    
    //send the set message to the fts event object
    sendSetMessage(value.getValueInfo().getName(), value.getPropertyCount(), value.getPropertyValues());
  }

  void sendSetMessage(String type, int nArgs, Object arguments[])
  {
    args.clear();
    for(int i=0; i<nArgs; i++)
      args.add(arguments[i]);

    try{
      send( FtsSymbol.get("set"), args);
    }
    catch(IOException e)
      {
	System.err.println("TrackEvent: I/O Error sending set Message!");
	e.printStackTrace(); 
      }
  }

    /**
     * Get the given property.
     * The property can be either a time or a
     * property handled by this object's value field.
     * Usually, the time property is not get with the getProperty method, 
     * but via the direct methods getTime() */
  public Object getProperty(String name)
  {
    if (name.equals("time"))
      return new Double(time);
    else if (value != null && !value.getProperty(name).equals(EventValue.UNKNOWN_PROPERTY))
      return value.getProperty(name); //this is not a know property, ask to the value object
    else return EventValue.DEFAULT_PROPERTY;
  }

  /**
   * Returns the value of this event */
  public EventValue getValue()
  {
    return value;
  }

  /** Set the Value corresponding to this event */
  public void setValue(EventValue value)
  {
    this.value = value;
  }

  /* --------- Drawable interface ----------*/
  
  public SeqObjectRenderer getRenderer()
  {
    if (value!= null)
      return value.getRenderer();
    else return AmbitusEventRenderer.getRenderer();
    // difficult choice here:
    // the value field is not there yet, and we're asked to provide
    // a renderer for this object of unknown type.
    // AmbitusEventRenderer is choosen here because it is a renderer 
    // that is able at least to correctly show
    // a rectangle with the starting time of an event.
  }
  
  /* --------- Undoable data interface ----------*/
  
    /**
     * Tells the model to start an undo section */
  public  void beginUpdate()  
  {
    //((UndoableData) itsTrackDataModel).beginUpdate();
    inGroup = true;
  }
    
  /**
   * posts an undo edit in the buffers */
  public  void postEdit(UndoableEdit e)
  {
  }
    
    /**
     * Signal that the undoable section ended */
    public void endUpdate() 
    {
      //((UndoableData) itsTrackDataModel).endUpdate();
      inGroup = false;
    }

    /**
     * Return true if an undoable section have been opened, and not closed yet*/
    public boolean isInGroup() 
    {
      return inGroup;
    }

    /**
     * ask to undo the section */
    public  void undo()
    {
    }
    
    /**
     * ask to redo the section */
    public  void redo()
    {
    }
    
    /*--------------  Editable interface --------------------*/
    public ValueEditor getValueEditor()
    {
      if (value != null)
	return value.getValueInfo().newValueEditor();
      else return NullEditor.getInstance();
    }

  public Event duplicate () throws CloneNotSupportedException
  {
    UtilTrackEvent evt = new UtilTrackEvent();
    evt.setTime(getTime());
    EventValue evtValue = (EventValue)(getValue().getValueInfo().newInstance());
    evtValue.setPropertyValues(getValue().getPropertyCount(), getValue().getPropertyValues());
    evtValue.setLocalPropertyValues(getValue().getLocalPropertyCount(), getValue().getLocalPropertyValues());
    evt.setValue(evtValue);
    return evt;
  }

  //--- Fields
  private double time;
  private EventValue value;
  private boolean inGroup = false;
  private boolean highlighted = false;

  public static double DEFAULT_TIME = 0;
  
  private TrackDataModel itsTrackDataModel;
  static Object[] evtArgs = new Object[128];
  
  protected FtsArgs args = new FtsArgs();
}






