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
import ircam.jmax.editors.sequence.menus.*;
import ircam.jmax.toolkit.*;
import java.awt.*;
import java.awt.event.*;
import java.beans.*;
import java.util.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;

/**
 * A Monodimensional view for a generic Sequence. 
 * This kind of editor use a MonoDimensionalAdapter
 * to map the y values. The value returned is always half of the panel,
 * and settings of y are simply ignored. */
public class MessageTrackEditor extends MonoTrackEditor
{
  public MessageTrackEditor(Geometry g, Track track)
  {
    super(g, track);
    
    super.setAdapter(new MessageAdapter(geometry, gc, MONODIMENSIONAL_TRACK_OFFSET));
    
    setLayout(null);
    
    area = new SequenceTextArea(TextRenderer.getRenderer(), gc);
    area.setMinimumSize(new Dimension(MessageEventRenderer.MINIMUM_WIDTH, MessageValue.DEFAULT_HEIGHT));
    area.addSequenceTextAreaListener(new SequenceTextAreaListener(){
	public void widthChanged(int width)
	{
	  int lenght = gc.getAdapter().getInvWidth(width+4+MessageEventRenderer.BUTTON_WIDTH);		
	  //currentEvt.setProperty("duration", new Integer(lenght));
	  currentEvt.setProperty("duration", new Double(lenght));
	  
	  int time = (int)currentEvt.getTime()+lenght;
	  gc.getScrollManager().scrollIfNeeded(time+((PartitionAdapter)gc.getAdapter()).getInvWidth(20));
	  
	  int evtx = gc.getAdapter().getX(currentEvt)+MessageEventRenderer.BUTTON_WIDTH+2+1;
	  area.setLocation(evtx, area.getLocation().y);
	}
	public void heightChanged(int height)
	{
	  currentEvt.setProperty("height", new Integer(height));
	  int evty = gc.getAdapter().getY(currentEvt);
	  if(evty+height>getSize().height)
	    {
	      int y = gc.getAdapter().getInvY(getSize().height-height);
	      currentEvt.setProperty("integer", new Integer(y));
	      area.setLocation(area.getLocation().x , getSize().height-height+1);
	    }
	}
	public void endEdit()
	{
	  setMessage();
	  repaint();
	}
      });
    add(area);
    validate();
    
    g.addZoomListener( new ZoomListener() {
	public void zoomChanged(float zoom, float oldZoom)
	{
	  updateEventsLength();
	}
      });
  }
    
  public JPopupMenu getMenu()
  {
    MessageTrackPopupMenu.getInstance().update(this);
    return MessageTrackPopupMenu.getInstance();
  }
  
  void doEdit(Event evt, int x, int y)
  {
    isEditing = true;
    currentEvt = evt;
    PartitionAdapter a = ((PartitionAdapter)gc.getAdapter());
    
    String text = a.getLabel(evt);
    
    int evtx = a.getX(evt)+MessageEventRenderer.BUTTON_WIDTH+2+1;
    int evty = a.getY(evt)+1;
    
    int evtLenght, evtHeight;
    
    if(!text.equals(""))
      {
	evtLenght = TextRenderer.getRenderer().getTextWidth(text, gc);
	evtHeight = TextRenderer.getRenderer().getTextHeight(text, gc);
      }	
    else
      {
	evtLenght = MessageEventRenderer.DEFAULT_TEXT_WIDTH-5;
	evtHeight = MessageValue.DEFAULT_HEIGHT;
      }
    
    area.doEdit(text, evtx, evty, evtLenght, evtHeight);

    setCaretPosition(text, x-evtx, y-evty);
  }
  
  void setCaretPosition(String text, int x, int y)
  {
    int pos = area.viewToModel(new Point(x, y));
    
    if ((pos >= 0) && (pos <= text.length()))
      area.setCaretPosition(pos);
    else
      area.setCaretPosition(text.length());
  }

  void setMessage()
  {
    if(currentEvt!=null)
      ((MessageValue)currentEvt.getValue()).setText(area.getText(), (TrackEvent)currentEvt, gc);
    endEdit();
  }
  
  void endEdit()
  {
    area.setText("");
    area.setVisible(false);
    requestFocus();
    currentEvt = null;
    isEditing = false;
  }

  int pressX, pressY;
  protected void processMouseEvent(MouseEvent e)
  {
    int id = e.getID();
    
    if(isEditing)
      if((id==MouseEvent.MOUSE_PRESSED)&&(e.getClickCount() == 1))
	{
	  ((UndoableData)itsTrack.getTrackDataModel()).beginUpdate();
	  setMessage();
	  ((UndoableData)itsTrack.getTrackDataModel()).endUpdate();
	}
      else if(isExitedFromTrack(e))
	{
	  setMessage();
	  repaint();
	}
    super.processMouseEvent(e);
    
    if(id==MouseEvent.MOUSE_PRESSED)
      {
	pressX = e.getX();
	pressY = e.getY();
      }
    else 
      if(id==MouseEvent.MOUSE_RELEASED)
	startEditIfNeeded(e);
  }
  
  boolean isExitedFromTrack(MouseEvent e)
  {
    return ((e.getID()==MouseEvent.MOUSE_EXITED)&&
	    (!area.getBounds().contains(e.getX(), e.getY())));
  }

  TrackEvent editEvt;
  void startEditIfNeeded(MouseEvent e)
  {
    int x = e.getX();
    int y = e.getY();
    if((pressX <= x+2)&&(pressX >= x-2)&&(pressY <= y+2)&&(pressY >= y-2))
      {
	editEvt = (TrackEvent) gc.getRenderManager().
	  firstObjectContaining(pressX, pressY);
	
	if (editEvt != null) 
	  { 
	    if(!((MessageValue)editEvt.getValue()).isOnTheButton(editEvt, pressX, gc))
	      SwingUtilities.invokeLater(new Runnable(){
		  public void run()
		  {
		    doEdit(editEvt, pressX, pressY);
		  }
		});
	  }
      }
  }

  void updateEventProperties(Object whichObject, String propName, Object propValue)
  {
    if(propName.equals("message"))
      {
	String text = (String)propValue;
	TrackEvent evt = (TrackEvent)whichObject;
	int width = TextRenderer.getRenderer().getTextWidth(text, gc);
	int height = TextRenderer.getRenderer().getTextHeight(text, gc);
	  
	if(width < MessageEventRenderer.MINIMUM_WIDTH) 
	  width  = MessageEventRenderer.MINIMUM_WIDTH;

	if(height < MessageValue.DEFAULT_HEIGHT) 
	  height = MessageValue.DEFAULT_HEIGHT;		
	
	int lenght = gc.getAdapter().getInvWidth(width+4+MessageEventRenderer.BUTTON_WIDTH);		
	evt.setProperty("duration", new Double(lenght));
	evt.setProperty("height", new Integer(height));
      }
  }

  public void updateNewObject(Object obj)
  {
    TrackEvent evt = (TrackEvent)obj;
    ((MessageValue)(evt.getValue())).updateLength(evt, gc); 
    ((MessageValue)(evt.getValue())).updateHeight(evt, gc); 
  }

    //update "duration" in order to have the same graphic length with the new zoom value
    //for all events in track  (called at zoom change). Called also at message object instantiation
    //to avoid the problem of the gc==null and so setting the good duration
    void updateEventsLength()
    {
	TrackEvent aTrackEvent;
	for (Enumeration e = gc.getDataModel().getEvents(); e.hasMoreElements();) 
	  {      
	      aTrackEvent = (TrackEvent) e.nextElement();
	      ((MessageValue)aTrackEvent.getValue()).updateLength(aTrackEvent, gc); 
	  }
    }

    SequenceTextArea area;
    boolean isEditing = false;
    Event currentEvt = null;
}














