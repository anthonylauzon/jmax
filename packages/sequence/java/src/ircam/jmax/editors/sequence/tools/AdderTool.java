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

package ircam.jmax.editors.sequence.tools;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.track.*;
import java.awt.event.*;
import java.awt.*;
import javax.swing.*;
import javax.swing.undo.*;
import java.util.*;

/**
 * The tool used to add an event in a track.
 * It uses just one user-interaction module:
 * a mouse tracker to choose the position.
 * The type of object to add is given by the track's type.
 */ 
public class AdderTool extends Tool implements PositionListener {

  /**
   * Constructor with an Icon.
   */
  public AdderTool(ImageIcon theImageIcon) 
  {
      super("adder", theImageIcon);
      
      itsMidiMouseTracker = new MidiMouseTracker(this);
  }


    /**
     * the default interaction module for this tool
     */
    public InteractionModule getDefaultIM() 
    {
	return itsMidiMouseTracker;
    }

    /**
     * called when this tool is "unmounted"
     */
    public void deactivate() 
    {
    }


    int remember_x; 
    int remember_y;
    /**
     *PositionListener interface
     */
    public void positionChoosen(int x, int y, int modifiers) 
    {
	SequenceGraphicContext egc = (SequenceGraphicContext) gc;
	
	egc.getTrack().setProperty("active", Boolean.TRUE);
	
	if (egc.getTrack().getTrackDataModel().getNumTypes()>1)
	  {
	      // In case of multitrack, the operation is postponed:
	      // a popup is shown to allow the user to choose the type of event to add.
	      popupChoose(x, y, egc.getTrack());
	      return;
	  }
	else
	    {
		ValueInfo info = (ValueInfo) egc.getTrack().getTrackDataModel().getTypes().nextElement();
		addEvent(x, y, (EventValue) info.newInstance());
	    }
    }
    
    
    static FtsAtom[] sendArgs = new FtsAtom[128];
    static
    {
	for(int i=0; i<128; i++)
	    sendArgs[i]= new FtsAtom();
    }

    void addEvent(int x, int y, EventValue value)
    {
	UtilTrackEvent aEvent = new UtilTrackEvent(value); // create a new event with the given FtsRemoteData type
	SequenceGraphicContext egc = (SequenceGraphicContext) gc;
	
	egc.getAdapter().setX(aEvent, x);
	egc.getAdapter().setY(aEvent, y);

	// starts an undoable transition
	/*((UndoableData) egc.getDataModel()).beginUpdate();
	
	  egc.getDataModel().addEvent(aEvent);
	
	  // ends the undoable transition
	  ((UndoableData) egc.getDataModel()).endUpdate();*/

	requestEventCreation(egc.getTrack().getName(),
			     (float)aEvent.getTime(), 
			     value.getValueInfo().getName(), 
			     value.getPropertyCount(), 
			     value.getPropertyValues());
    }

    private void requestEventCreation(String trackName, float time, String type, int nArgs, Object args[])
    {
      SequenceGraphicContext egc = (SequenceGraphicContext) gc;
      
      //////////////////////////////////////
      /*sendArgs[0].setObject(egc.getFtsSequenceObject()); 
	sendArgs[1].setString(trackName); 
	sendArgs[2].setFloat(time); 
	sendArgs[3].setString(type);
      
	for(int i=0; i<nArgs; i++)
	sendArgs[4+i].setValue(args[i]);
      
	MaxApplication.getFts().makeFtsObjectAsync("seqevent", 4+nArgs, sendArgs);*/

      sendArgs[0].setString(trackName); 
      sendArgs[1].setFloat(time); 
      sendArgs[2].setString(type);
      
      for(int i=0; i<nArgs; i++)
	  sendArgs[3+i].setValue(args[i]);
	
      egc.getFtsSequenceObject().sendMessage(FtsObject.systemInlet, "event_new", 3+nArgs, sendArgs);
    }

    void popupChoose(int x, int y, Track track)
    {

	SequenceGraphicContext egc = (SequenceGraphicContext) gc;
	JPopupMenu popup = new JPopupMenu();

	for (Enumeration e = track.getTrackDataModel().getTypes(); e.hasMoreElements();)
	  {
	      ValueInfo info = (ValueInfo) e.nextElement();
	      PrivateAction a = new PrivateAction(info, x, y);
	  
	      popup.add(a);
	  }

	popup.show(gc.getGraphicDestination(), x-5, y-5);
    }


    class PrivateAction extends AbstractAction {
	public PrivateAction(ValueInfo info, int x, int y)
	{
	    super( info.getName(), info.getIcon());

	    this.info = info;
	    this.x = x;
	    this.y = y;
	}
    
	public void actionPerformed(ActionEvent e)
	{
	    addEvent(x, y, (EventValue) info.newInstance());
	}

	int x;
	int y;
	ValueInfo info;

    }

  //-------------- Fields

  MidiMouseTracker itsMidiMouseTracker;
}



