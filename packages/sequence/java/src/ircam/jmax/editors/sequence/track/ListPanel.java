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
import java.beans.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;

import java.text.NumberFormat;

/**
 * A view for a Midi sequence (sequence composed of events whose Value field is
 * a Midi value) */
public class ListPanel extends PopupToolbarPanel implements TrackDataListener, MouseListener, ListSelectionListener, KeyListener
{
    public ListPanel(Track track, PopupProvider provider)
    {
      super(provider);

      setOpaque(true);

      setLayout(null);

      this.track = track;
      data = track.getTrackDataModel();	

      data.addListener(this);

      //to make current this selection maybe move on the mouseright-showpopup  action
      if(!((Boolean)track.getProperty("active")).booleanValue())
	  track.setProperty("active", Boolean.TRUE);

      SequenceSelection.getCurrent().addListSelectionListener(this);
      setSize(300, data.length()*20+20);
      addMouseListener(this);
      addKeyListener(this);

      //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      area = new JTextArea();
      area.setCursor( Cursor.getPredefinedCursor( Cursor.TEXT_CURSOR));
      area.setEditable(true);
      area.setLineWrap(true);
      area.setWrapStyleWord(true);
      area.setBackground(Color.white);
      area.addKeyListener(new KeyListener(){
	  public void keyPressed(KeyEvent e){
	      if(e.getKeyCode()==KeyEvent.VK_ENTER)
		     setEventValue();
	  }
	  public void keyReleased(KeyEvent e){}
	  public void keyTyped(KeyEvent e){}
      });
      add(area);
      validate();
    }

    /**
     * TrackDataListener interface
     */
    
    public void objectChanged(Object spec) 
    {
	select((TrackEvent)spec, data.indexOf((TrackEvent)spec), currentParamInEvent);
	repaint();
    }
    
    public void objectAdded(Object spec, int index) 
    {
	setSize(300, getSize().height+20);
	select((TrackEvent)spec, index, 0);
	repaint();
    }
    
    public void objectDeleted(Object whichObject, int index) 
    {
	setSize(300, getSize().height-20);
	deselect((TrackEvent)whichObject, index);
	repaint();
    }
    
    public void objectMoved(Object whichObject, int oldIndex, int newIndex) 
    {
	select((TrackEvent)whichObject, newIndex, 0);
	repaint();
    }
    
    public Dimension getPreferredSize()
    {
	return new Dimension(300, data.length()*20+20);
    }

    public Dimension getMinimumSize()
    {
	return new Dimension(300, 250);
    }

    public void paintComponent(Graphics g) 
    {
	TrackEvent evt;
	Rectangle r = g.getClipBounds();
	g.setColor(Color.white);
	g.fillRect(0, 0, r.width, r.height);

	if(currentIndex!=-1)
	{
	  g.setColor(SequencePanel.violetColor.brighter());
	  g.fillRect(0, (currentIndex)*ystep+1, r.width, ystep-1);
	}

	g.setColor(Color.black);
	int y = ystep;	    
	for(Enumeration e = data.getEvents(); e.hasMoreElements();)
	{
	    evt = (TrackEvent) e.nextElement();

	    g.drawString(""+numberFormat.format(evt.getTime()), 5, y-2);
	    g.drawString(""+((Integer)evt.getProperty("pitch")).intValue(), 5+xstep, y-2);
	    g.drawString(""+((Integer)evt.getProperty("duration")).intValue(), 5+2*xstep, y-2);

	    y = y+ystep;
	}

	g.setColor(Color.lightGray);
	for(int i=1; i<data.length();i++)
	    g.drawLine(5, (ystep)*i, 5+r.width, (ystep)*i);
    }
   
    int getEventIndex(int y)
    {
	int index = y/ystep;
	if(index >= data.length()) index = data.length()-1;
	return index;
    }
    //dopo deve controllare sul numero di parametri mostrati (per ora 3)
    int getEventParamIndex(int x)
    {
	int index = x/xstep;
	if(index > 2) index = 2;
	return index;
    }
    public void mouseClicked(MouseEvent e){}
    public void mousePressed(MouseEvent e)
    {
	int index;
	TrackEvent evt;
	
	if(!((Boolean)track.getProperty("active")).booleanValue())
	    track.setProperty("active", Boolean.TRUE);

	int x = e.getX();
	int y = e.getY();
	if(data.length()!=0)
	    {
		index = getEventIndex(y);
		if(index==currentIndex)//already selected
		    {
			currentParamInEvent = getEventParamIndex(x);		
			doEdit(index, currentParamInEvent, currentEvent);
		    }
		else
		    {
			SequenceSelection.getCurrent().deselectAll();
			SequenceSelection.getCurrent().select(data.getEventAt(index));
		    }
	    }
    }
    public void mouseReleased(MouseEvent e){}
    public void mouseEntered(MouseEvent e){}
    public void mouseExited(MouseEvent e){}
    

    public void addListListener(TrackListListener listener)
    {
      listeners.addElement(listener);
    }

    void notifySelection(int index, int paramIndex, TrackEvent evt)
    {
	for(Enumeration e = listeners.elements(); e.hasMoreElements();)
	    ((TrackListListener) e.nextElement()).eventSelected(index, paramIndex, evt);
    }

    void notifyDeselection(int index, TrackEvent evt)
    {
	for(Enumeration e = listeners.elements(); e.hasMoreElements();)
	    ((TrackListListener) e.nextElement()).eventDeselected(index, evt);
    }    

    void notifyShow(int index, TrackEvent evt, Rectangle r)
    {
	for(Enumeration e = listeners.elements(); e.hasMoreElements();)
	    ((TrackListListener) e.nextElement()).showEvent(index, evt, r);
    }

    public TrackEvent getCurrentEvent()
    {
	return currentEvent;
    }
    public int getCurrentParamIndex()
    {
	return currentParamInEvent;
    }
    public void select(TrackEvent evt, int index, int paramIndex)
    {
	currentParamInEvent = paramIndex;
	currentIndex = index;
	currentEvent = evt;
	notifySelection(currentIndex, currentParamInEvent, currentEvent);

	selRect.setBounds(currentParamInEvent*xstep, currentIndex*ystep, xstep, ystep);
	notifyShow(currentIndex, currentEvent, selRect);
    }

    public void deselect(TrackEvent evt, int index)
    {
	currentIndex = -1;
	currentEvent = null;
	notifyDeselection(index, evt);
	endEdit();
    }

    //listSelectionListener interface
    public void valueChanged(ListSelectionEvent e)
    {
	TrackEvent evt;
	int index, size;
	size = SequenceSelection.getCurrent().size();

	if(size==1)
	{
	  evt = (TrackEvent)SequenceSelection.getCurrent().getSelected().nextElement();
	  index = data.indexOf(evt);
	  if(index!=currentIndex)
	    select(evt, index, currentParamInEvent);
	}
	else deselect(currentEvent, currentIndex);
    }

    void doEdit(int index, int param, TrackEvent evt)
    {
	area.setBounds(xstep*param+1, index*ystep+2, xstep, ystep-4);
	area.requestFocus();
	switch(param)
	    {
	    case 0:
		area.setText(""+evt.getTime());
		break;
	    case 1:
		area.setText(""+((Integer)evt.getProperty("pitch")).intValue());
		break;
	    case 2:
		area.setText(""+((Integer)evt.getProperty("duration")).intValue());
	    }
	area.setVisible(true);	
    }

    void endEdit()
    {
	area.setText("");
	area.setVisible(false);
	requestFocus();
    }

    void setEventValue()
    {	
	int value = 0;
	double doubleValue = 0;
	if(currentEvent!=null)
	    {
		if(currentParamInEvent==0)
		    {
			try { 
			   doubleValue = Double.valueOf(area.getText()).doubleValue(); // parse double
			} catch (NumberFormatException exc) {
			    System.err.println("Error:  invalid number format!");
			    endEdit();
			    repaint();
			    return;
			}
			if(doubleValue<0) doubleValue = 0;
			currentEvent.move(doubleValue);
		    }
		else
		    {
			try { 
			    value = Integer.valueOf(area.getText()).intValue(); // parse int
			} catch (NumberFormatException exc) {
			    System.err.println("Error:  invalid number format!");
			    endEdit();
			    repaint();
			    return;
			}
			if(value<0) value=0;
			switch(currentParamInEvent)
			    {
			    case 0:
			
				//currentEvent.move(value);
				break;
			    case 1:
				if(value>127) value=127;
				currentEvent.setProperty("pitch", new Integer(value));
				break;
			    case 2:
				currentEvent.setProperty("duration", new Integer(value));
				break;
			    default:
				break;
			    }
		    }
	    }
	endEdit();
    }

    public JTextArea getTextArea()
    {
	return area;
    }

    //keylistener interface
    public void keyPressed(KeyEvent e){
	
	int index=currentIndex;
	
	if((e.getKeyCode()==KeyEvent.VK_BACK_SPACE)||(e.getKeyCode()==KeyEvent.VK_CANCEL))
	    removeSelection();
	else
	    if(e.getKeyCode()==KeyEvent.VK_UP)
	    {
		if(index>0)
		    {
			SequenceSelection.getCurrent().deselectAll();
			SequenceSelection.getCurrent().select(data.getEventAt(index-1));
		    }
	    }
	    else
		if(e.getKeyCode()==KeyEvent.VK_DOWN)
		    {
			if(index<data.length()-1)
			    {
				SequenceSelection.getCurrent().deselectAll();
				SequenceSelection.getCurrent().select(data.getEventAt(index+1));
			    }
		    }
    }
    public void keyReleased(KeyEvent e){}
    public void keyTyped(KeyEvent e){}

    public void removeSelection()
    {
	if(currentEvent!=null)
	    data.removeEvent(currentEvent);
    }

    public void addAfterSelection()
    {
	if(currentEvent!=null)
	    {
		
		TrackEvent aEvent = new TrackEvent((EventValue) currentEvent.getValue().getValueInfo().newInstance());						
		aEvent.setTime(currentEvent.getTime());
		aEvent.setProperty("duration", currentEvent.getProperty("duration"));
		aEvent.setProperty("pitch", currentEvent.getProperty("pitch"));
		data.addEvent(aEvent);
	    }
    }


    final public static int xstep = 100;
    final public static int ystep = 20;
    TrackDataModel data;
    Track track;
    TrackEvent currentEvent = null;
    int currentIndex = -1;
    int currentParamInEvent = 0;

    Rectangle selRect = new Rectangle();
    Vector listeners = new Vector();

    JTextArea area;

    static public NumberFormat numberFormat;
    static 
    {
	numberFormat = NumberFormat.getInstance();
	numberFormat.setMaximumFractionDigits(4);
	numberFormat.setGroupingUsed(false);
    }
}











