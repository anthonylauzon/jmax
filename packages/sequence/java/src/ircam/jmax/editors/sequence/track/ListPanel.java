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
import ircam.jmax.utils.*;
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
    public ListPanel(Track track, PopupProvider provider, ListContainer container, SequenceGraphicContext gc)
    {
      super(provider);

      setOpaque(true);

      setLayout(null);

      this.track = track;
      this.container = container;
      data = track.getTrackDataModel();	
      this.gc = gc;

      fm = getFontMetrics(listFont);

      data.addListener(this);

      //to make current this selection maybe move on the mouseright-showpopup  action
      if(!((Boolean)track.getProperty("active")).booleanValue())
	  track.setProperty("active", Boolean.TRUE);

      SequenceSelection.getCurrent().addListSelectionListener(this);

      setSize((data.getNumProperty()+1)*xstep +25, data.length()*20+20);

      addMouseListener(this);
      addKeyListener(this);

      area = new SequenceTextArea(txtRenderer, gc);
      area.setMinimumSize(new Dimension(xstep-5, ystep-2));
      area.setBorder(BorderFactory.createEtchedBorder());
      area.setForeground(Color.black);
      area.setContainer(this);
      area.addSequenceTextAreaListener(new SequenceTextAreaListener(){
	  public void widthChanged(int width)
	  {
	      repaint();
	  }
	  public void heightChanged(int height)
	  {
	      repaint();
	  }
	  public void endEdit()
	  {
	      //setEventValue(); 
	  }
	  
      });
      add(area);
      validate();
    }

    /**
     * TrackDataListener interface
     */    
    public void objectChanged(Object spec, String propName, Object propValue) 
    {
	select((TrackEvent)spec, data.indexOf((TrackEvent)spec), currentParamInEvent);
	repaint();
    }
    
    public void objectAdded(Object spec, int index) 
    {
	setSize(getSize().width, getSize().height+20);
	setPreferredSize(new Dimension(getSize().width, getPreferredSize().height+20));
    }

    public void objectsAdded(int maxTime){}
    
    public void objectDeleted(Object whichObject, int index) 
    {
	setSize(getSize().width, getSize().height-20);
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
	return new Dimension((data.getNumProperty()+1)*xstep+25, data.length()*20+20);
    }

    public Dimension getMinimumSize()
    {
	return new Dimension((data.getNumProperty()+1)*xstep+25, 250);
    }

    public void paintComponent(Graphics g) 
    {
	TrackEvent evt;
	String text;
	Dimension d = getSize();
	Rectangle r = g.getClipBounds();

	g.setColor(Color.white);
	g.fillRect(0, 0, r.width, r.height);

	drawSelection(g, r);

	g.setColor(Color.black);

	int first = getEventIndex(r.y);
	int last = getEventIndex(r.y+r.height+3*ystep);

	int y = ystep*(first+1);	    

	g.setFont(listFont);

	for(Enumeration e = data.getEvents(first, last); e.hasMoreElements();)
	    {
		evt = (TrackEvent) e.nextElement();
	  
		g.drawString(""+numberFormat.format(evt.getTime()), 5, y-6);
		
		//for on the property names
		int i = 1;
		for(Enumeration e1 = track.getTrackDataModel().getPropertyNames(); e1.hasMoreElements();)
		    {
			text = getPropertyAsString(evt.getProperty((String)e1.nextElement()));
			int width = SwingUtilities.computeStringWidth(fm, text);
			if(width > xstep) 
			    text = getSubstringOfWidth(text, xstep-10);

			g.drawString(text, i*xstep, y-6);
			i++;
		    }
		y = y+ystep;
	    }	
	  
	g.setColor(Color.lightGray);

	for(int i = first+1; i <= last;i++)
	  g.drawLine(5, (ystep)*i, 5+d.width, (ystep)*i);
    }
   
    String getSubstringOfWidth(String text, int maxWidth)
    {
	int length = text.length();
	int width = SwingUtilities.computeStringWidth(fm, text);

	if (width < maxWidth)
	    return text;

	String string = text;
	String string2 = "..";

	while(( maxWidth <= width) && (text.length() > 0) )
	    {
		string = string.substring( 0, string.length()-1);
		width = SwingUtilities.computeStringWidth(fm, string);
	    }
	
	if ( ( width + SwingUtilities.computeStringWidth(fm, "..") >= maxWidth)
	     && ( string.length() > 0) )
	    string = string.substring( 0, string.length() - 1);

	string = string + string2;
	return string;
    }

    String getPropertyAsString(Object prop)
    {
	if(prop instanceof Integer)
	    return ""+((Integer)prop).intValue();
	else
	    if(prop instanceof Float)
		return ""+((Float)prop).floatValue();
	    else if(prop instanceof Boolean)
		return ""+((Boolean)prop).booleanValue();
	    else if(prop instanceof Double)
		return ""+((Double)prop).doubleValue();
	    else if(prop instanceof String)
		return (String)prop;
	    else return "";
    }

    void drawSelection(Graphics g, Rectangle r)
    {
	int index;
	TrackEvent evt;
	Dimension d = getSize();
	g.setColor(SequencePanel.violetColor.brighter());
	for(Enumeration e = SequenceSelection.getCurrent().getSelected(); e.hasMoreElements();)
	    {
		evt = (TrackEvent) e.nextElement();
		index = data.indexOf(evt);
		g.fillRect(0, (index)*ystep+1, d.width, ystep-1);
	    }
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

    int firstIndexVisible()
    {
	return getEventIndex(container.getVisibleRect().y);
    }
    int lastIndexVisible()
    {
	Rectangle r = container.getVisibleRect();
	return getEventIndex(r.y+r.height+3*ystep);
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
		evt = data.getEventAt(index);
		if(SequenceSelection.getCurrent().isInSelection(evt))
		{
		    if(!e.isShiftDown())
			{
			    if(isEditing)
				setEventValue();
			    select(evt, index, getEventParamIndex(x));
			    doEdit(index, currentParamInEvent, currentEvent);			    
			}
		    else
			{
			    if(isEditing)
				setEventValue();
			    SequenceSelection.getCurrent().deSelect(evt);
			}
		}
		else
		    {
			if(isEditing)
			    setEventValue();
			if(!e.isShiftDown())
			    SequenceSelection.getCurrent().deselectAll();
			SequenceSelection.getCurrent().select(data.getEventAt(index));
		    }
	    }
	requestFocus();
    }
    public void mouseReleased(MouseEvent e){}
    public void mouseEntered(MouseEvent e){}
    public void mouseExited(MouseEvent e){}
    
    protected void processMouseEvent(MouseEvent e)
    {
	if (e.isPopupTrigger()) 
	    {
		popupx = e.getX();
		popupy = e.getY();
	    }
	super.processMouseEvent(e);
    }

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
	scrollRectToVisible(selRect);
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
	TrackEvent evt = null;
	int index, size;
	size = SequenceSelection.getCurrent().size();
	
	if(size==1)
	    {
		evt = (TrackEvent)SequenceSelection.getCurrent().getSelected().nextElement();
		index = data.indexOf(evt);
		
		if(index!=currentIndex)
		    select(evt, index, currentParamInEvent);
	    }
	else
	    {
		evt = SequenceSelection.getCurrent().getLastSelectedEvent();
		//requestFocus();
		if(evt != null)
		    {
			//requestFocus();
			index = data.indexOf(evt);
			select(evt, index, currentParamInEvent);
		    }
		else
		    deselect(null, -1);
	    }	
    }

    void doEdit(int index, int param, TrackEvent evt)
    {
	String text;

	isEditing = true;
	
	if(param == 0)
	    text = ""+evt.getTime();
	else
	    text = getPropertyAsString(evt.getProperty(getPropNameByIndex(param-1)));

	int width  = txtRenderer.getTextWidth(text, gc)+5;
	if(width < xstep-5) width = xstep-5;
	
	int height = txtRenderer.getTextHeight(text, gc)+4;
	if(height < ystep-4) height = ystep-4;

	area.doEdit(text, xstep*param+1, index*ystep+2-1, width, height);
    }

    void endEdit()
    {
	area.setText("");
	area.setVisible(false);
	requestFocus();
	isEditing = false;
    }

    String getPropNameByIndex(int index)
    {
	String name;
	int i = 0;
	for(Enumeration e = data.getPropertyNames(); e.hasMoreElements();)
	    {
		name = (String)e.nextElement();
		if(i==index) 
		    return name;
		else
		    i++;
	    }
	return "";
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
			Object objVal = getEditValueObject();
			if(objVal!=null)
			    currentEvent.setProperty(getPropNameByIndex(currentParamInEvent-1), objVal);
		    }
	    }
	endEdit();
    }

    Object getEditValueObject()
    {
	int intValue;
	float floatValue;
	double doubleValue;
	switch(currentEvent.getValue().getPropertyType(currentParamInEvent-1))
	    {
	    case EventValue.INTEGER_TYPE:
		try { 
		    intValue = Integer.valueOf(area.getText()).intValue(); // parse int
		} catch (NumberFormatException exc) {
		    System.err.println("Error:  invalid number format!");
		    endEdit();
		    repaint();
		    return null;
		}
	 	if(intValue<0) intValue = 0;
		 return new Integer(intValue);
	
	    case EventValue.FLOAT_TYPE:
		try { 
		    floatValue = Float.valueOf(area.getText()).floatValue(); // parse int
		} catch (NumberFormatException exc) {
		    System.err.println("Error:  invalid number format!");
		    endEdit();
		    repaint();
		    return null;
		}
		if(floatValue<0) floatValue = (float)0.0;
		return new Float(floatValue);
	    
	    case EventValue.STRING_TYPE:
		return area.getText(); 
	    case EventValue.DOUBLE_TYPE:
		try { 
		    doubleValue = Double.valueOf(area.getText()).doubleValue(); // parse int
		} catch (NumberFormatException exc) {
		    System.err.println("Error:  invalid number format!");
		    endEdit();
		    repaint();
		    return null;
		}
		if(doubleValue<0) doubleValue = 0.0;
		return new Double(doubleValue);

	    case EventValue.BOOLEAN_TYPE:
		return Boolean.valueOf(area.getText());
	    default:
		return null;
	    }
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
			if(!e.isShiftDown())
			    SequenceSelection.getCurrent().deselectAll();
			SequenceSelection.getCurrent().select(data.getEventAt(index-1));
		    }
	    }
	    else
		if(e.getKeyCode()==KeyEvent.VK_DOWN)
		    {
			if(index<data.length()-1)
			    {
				if(!e.isShiftDown())
				    SequenceSelection.getCurrent().deselectAll();
				SequenceSelection.getCurrent().select(data.getEventAt(index+1));
			    }
		    }
	requestFocus();
    }
    public void keyReleased(KeyEvent e){}
    public void keyTyped(KeyEvent e){}

    public void removeSelection()
    {
	MaxVector v = new MaxVector();
	// copy the selected elements in another MaxVector (cannot remove
	// elements of a Vector inside a loop based on an enumeration of this vector, it simply does'nt work...)
	for (Enumeration e = SequenceSelection.getCurrent().getSelected(); e.hasMoreElements();)
	    v.addElement(e.nextElement());
	    
	// remove them
	for (int i = 0; i< v.size(); i++)
	    data.removeEvent((TrackEvent)(v.elementAt(i)));
	v = null;
    }

    public void addAfter()
    {
	int index = getEventIndex(popupy);
	if(index<0) return;

	TrackEvent popupEvent = data.getEventAt(index);  
	if(popupEvent!=null)
	    ((FtsTrackObject)data).requestEventCreation((float)popupEvent.getTime(), 
				      popupEvent.getValue().getValueInfo().getName(), 
				      popupEvent.getValue().getPropertyCount(), 
				      popupEvent.getValue().getPropertyValues());
    }

    final public static int xstep = 120;
    final public static int ystep = 20;
    TrackDataModel data;
    Track track;
    SequenceGraphicContext gc;
    ListContainer container;
    TrackEvent currentEvent = null;
    int currentIndex = -1;
    Vector selectedIndexVector = new Vector();
    public static final Font listFont = new Font("Dialog", Font.PLAIN, 12);
    FontMetrics fm; 

    int currentParamInEvent = 0;

    Rectangle selRect = new Rectangle();
    Vector listeners = new Vector();

    TextRenderer txtRenderer = new TextRenderer(listFont, Color.white, Color.black);

    //JTextArea area;
    SequenceTextArea area;
    boolean isEditing = false;

    private int popupx, popupy;

    static public NumberFormat numberFormat;
    static 
    {
	numberFormat = NumberFormat.getInstance();
	numberFormat.setMaximumFractionDigits(4);
	numberFormat.setGroupingUsed(false);
    }
}











