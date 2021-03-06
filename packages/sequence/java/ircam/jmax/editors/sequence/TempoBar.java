//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.editors.sequence;

import ircam.jmax.toolkit.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.renderers.*;
import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.editors.sequence.menus.*;

import ircam.fts.client.*;

import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import java.beans.*;

public class TempoBar extends PopupToolbarPanel implements TrackDataListener, TrackStateListener, TrackListener, ListSelectionListener, MouseListener
{
  public TempoBar( Geometry geom, FtsGraphicObject obj, SequenceEditor ed)
	{
    super();
		
		setFont(SequencePanel.rulerFont);
    fm = getFontMetrics( SequencePanel.rulerFont);
    this.geometry = geom;
		this.ftsObj = obj;
		this.container = ed;
		this.isInSequence = (ftsObj instanceof FtsSequenceObject);
		pa = new PartitionAdapter(geometry, null);
    
		setBackground(Color.white);
    setLayout( new BoxLayout( this, BoxLayout.Y_AXIS));
    
		if( !isInSequence)
			((FtsTrackObject)ftsObj).addTrackStateListener(this);		
		else
			((FtsSequenceObject)ftsObj).addTrackListener(this);
		
    geometry.addTranspositionListener( new TranspositionListener() {
			public void transpositionChanged(int newValue)
		  {
				repaint();
			}
		});		
		geometry.getPropertySupport().addPropertyChangeListener(new PropertyChangeListener() {
			public void propertyChange(PropertyChangeEvent e)
		{	  
				if( e.getPropertyName().equals("gridMode"))
					repaint();
		}
		});
		
		addMouseListener(this);		
		
    setSize(SequenceWindow.DEFAULT_WIDTH, TempoBar.TEMPO_HEIGHT);
    setPreferredSize(TempoBar.tempoDimension);
    setMinimumSize(TempoBar.tempoDimension);
    
    validate();
	}

public FtsTrackObject getMarkers()
{
	return markersTrack;
} 

public JPopupMenu getMenu()
{
	if(popup == null)
		createPopup();
		
  if(popup != null)
    popup.update();
	return popup;
}

public Enumeration getPropertyNamesToDraw()
{
  Vector temp = new Vector();
  for(int i=0 ; i < propertyNames.length; i++)
		if(propertyToDraw[i])
      temp.addElement(propertyNames[i]);
  return temp.elements();
}

boolean propertyToDraw(String propName, boolean toDraw)
{
  boolean changeSize = false;
	
	for(int i=0 ; i < propertyNames.length; i++)
		if(propertyNames[i].equals(propName))
		{
			if(propertyToDraw[i] != toDraw)
			{				
				propertyToDraw[i] = toDraw;
				changeSize = true;
			}
			break;
		}
      
  if(changeSize)
  {
    Dimension d = getSize();    
    int delta;
    if(toDraw) 
    {
      numPropToDraw++;
      delta = DELTA_H;
    }
    else 	
    {
      numPropToDraw--;			
      delta = -DELTA_H;
    }
    d.height += delta;		
    setSize(d.width, d.height);
    setPreferredSize(d);
    setMinimumSize(d);
  }  
  return changeSize;
}

public void setPropertyToDraw(String propName, boolean toDraw)
{
	boolean changeSize = propertyToDraw(propName, toDraw);
	if(changeSize)
	{
		validate();		
    if(container.getEditorContainer().getFrame().isVisible())
      container.getEditorContainer().getFrame().pack();

    if( !isInSequence && (((FtsTrackObject)ftsObj).editorObject != null))
		  ((FtsTrackObject)ftsObj).editorObject.setPropertiesToDraw( getPropertyNamesToDraw());
	}
	repaint();
}

boolean propInited = false;
void initPropertiesToDraw()
{  
  if( !propInited)
  {   
    String pname;
    int i = 0;
    int count = markersTrack.getPropertyCount()-1;
    propertyNames = new String[ count];
    propertyToDraw = new boolean[ count];
    for(Enumeration e = markersTrack.getPropertyNames(); e.hasMoreElements();)
    {
      pname = (String)e.nextElement();	
      if(!pname.equals("type"))
      {
        propertyNames[i] = pname;
        propertyToDraw[i] = (propertyNames[i].equals("tempo") || propertyNames[i].equals("meter"));
        i++;
      }
    }
    numPropToDraw = 2;
    propInited = true;
  }
}

void deInitPropertiesToDraw()
{  
  int height = tempoDimension.height - numPropToDraw*DELTA_H;  
  for(int i = 0; i < propertyToDraw.length; i++)
    propertyToDraw[i] = false;
  numPropToDraw = 0;
  
  setSize(tempoDimension.width, height);
  Dimension d = new Dimension(tempoDimension.width, height);
  setPreferredSize(d);
  setMinimumSize(d);
}

void createPopup()
{
  if(propertyNames.length > 0)
    popup = new TempoBarPopupMenu(this);
}

public void paintComponent(Graphics g)
{
	Rectangle clip = g.getClipBounds();
	g.setColor( Color.white);
	g.fillRect( clip.x, clip.y, clip.width, clip.height);
	
	paintLegend(g, clip);
	paintMeasures(g, clip);
}

public void paintLegend(Graphics g, Rectangle clip)
{
	if(markersTrack!=null)
	{
		int h = DELTA_H-1;
		g.setColor( Color.lightGray);
		g.setFont( Displayer.displayFont);
		for(int i = 0; i < propertyNames.length; i++)
			if(propertyToDraw[i])
			{
        /*if(propertyNames[i].equals("cue"))
        {
        g.setColor( cueBorderColor);
        g.drawString( propertyNames[i], clip.x + 3, clip.y+h);
        g.setColor( Color.lightGray);
				}
        else*/
        g.drawString( propertyNames[i], clip.x + 3, clip.y+h);
        
        h+=DELTA_H;
			}
	}
}
public void paintMeasures(Graphics g, Rectangle clip)
{
	if( markersTrack!= null)
	{
		TrackEvent evt;		
		int x;
		String type;
    Color color;
		Dimension d = getSize();		
		g.setFont( Displayer.displayFont);
		
		for (Enumeration e = markersTrack.intersectionSearch( pa.getInvX(ScoreBackground.KEYEND), 
																													pa.getInvX(d.width-ScoreBackground.KEYEND)); e.hasMoreElements();) 
		{
			evt = (TrackEvent) e.nextElement();
			type = (String)(evt.getProperty("type"));
			x = pa.getX(evt)+getXIndentation();
			boolean selected = markersSelection.isInSelection(evt);
			
      if(selected) g.setColor(selLineColor);
      else g.setColor(lineColor);
      g.drawLine( x, 0, x, d.height);
      
      if(type.equals("tempo"))
			{
				if( selected)
					color = selTempoColor;
				else
					color = tempoColor;	
			}
			else
			{
				if( selected)
					color = Color.red;
        else
					color = Color.darkGray;	
			}
      //g.setColor( color);
			int h = DELTA_H-1;
			Object prop;
			String str;
			int strw;
			g.setFont( Displayer.displayFont);
			for(int i = 0; i < propertyNames.length; i++)
				if(propertyToDraw[i])
				{
					prop = evt.getProperty(propertyNames[i]);
					if(prop!=null)
					{
						if(prop instanceof Double)
							str = ""+((Double)prop).intValue();
						else
							str = prop.toString();
						strw = fm.stringWidth(str);
            drawProperty(g, propertyNames[i], str, selected, color, x - strw/2 + 1, h, strw, DELTA_H-3);
          }
					h+=DELTA_H;
				}
		}
	}
}

void drawProperty(Graphics g, String propName, String propVal, boolean selected, Color oldColor, int x, int y, int strw, int strh)
{
  if(propName.equals("bar #"))
  {
    if(selected)
      g.setColor(selBorderColor);
    else
      g.setColor(borderColor);
    g.drawRect(x-1, y-strh-1, strw+1, strh+2);
    g.setColor(oldColor);
    g.drawString( propVal, x, y);
  }
  else
    if(propName.equals("label"))
    {
      if(selected)
        g.setColor(selLabelColor);
      else
        g.setColor(tempoColor);
      g.fillRect(x-1, y-strh-1, strw+1, strh+2);
      
      if(selected)
        g.setColor( selBorderColor);
      else
        g.setColor(Color.gray);
      
      g.drawRect(x-1, y-strh-1, strw+1, strh+2);
      
      g.drawString( propVal, x, y);
      g.setColor(oldColor);
    }
  else 
    if(propName.equals("cue"))
    {
      if(selected)
        g.setColor( selLabelColor);
      else
        g.setColor(cueColor);
      g.fillRect(x-1, y-strh-1, strw+1, strh+2);
      
      if(selected)
        g.setColor(selBorderColor);
      else
        g.setColor(cueBorderColor);
      
      g.drawRect(x-1, y-strh-1, strw+1, strh+2);
      
      g.drawString( propVal, x, y);
      g.setColor(oldColor);
    }  
  else
  {
    g.setColor(oldColor);
    g.drawString( propVal, x, y);
  }
 
}

int getXIndentation()
{
	if( isInSequence)
		return 2+TrackContainer.BUTTON_WIDTH;
	else
		return 2;
}

public TrackEvent firstMarkerContaining(int x, int y)
{
  TrackEvent mark;
  TrackEvent currMark = null;
  int mark_x;
  Dimension d = getSize();	
  
  for (Enumeration e = markersTrack.intersectionSearch( pa.getInvX(ScoreBackground.KEYEND), 
                                                        pa.getInvX(d.width-ScoreBackground.KEYEND)); e.hasMoreElements();) 
  {
    mark = (TrackEvent) e.nextElement();
    mark_x = pa.getX(mark);
    
    if( x <= mark_x + 3 && x >= mark_x - 3)
      currMark = mark;
  }
  return currMark;
}

public void processKeyEvent(KeyEvent e)
{
  if(e.getID()==KeyEvent.KEY_PRESSED)
  {
    switch( e.getKeyCode())
    {
      case KeyEvent.VK_LEFT:       
        if(markersSelection != null && markersSelection.size() > 0)
          markersSelection.selectPreviousByType();
        break;
      case KeyEvent.VK_RIGHT:
        if(markersSelection != null && markersSelection.size() > 0)
          markersSelection.selectNextByType();
        break;
      default:
        break;
    }  
  }
	super.processKeyEvent(e);
	requestFocus();
}

TrackEvent lastBar = null;
double lastBarTime = -1.0;
/* if is a bar set if is last bar in score as a property "last_bar" */
void setLastBar(TrackEvent evt)
{
	double time = evt.getTime();
	if(time >= lastBarTime)
	{
		if(lastBar != null) 
			lastBar.getValue().setProperty("last_bar", Boolean.FALSE);
		
		lastBar = evt;
		lastBarTime = time;
		lastBar.getValue().setProperty("last_bar", Boolean.TRUE);
	}	
}

public void notifySelectionToListeners(SequenceSelection s, String message)
{
  if( !isInSequence && (((FtsTrackObject)ftsObj).editorObject != null))
  {
    if(s.size() > 0)
    {
      TrackEvent evt;
      listener_args.clear();
      listener_args.addSymbol(FtsSymbol.get("selection"));
      listener_args.addSymbol(FtsSymbol.get("start"));
     ((FtsTrackObject)ftsObj).editorObject.requestListenersNotify(listener_args);
        
      for(Enumeration e = s.getSelected(); e.hasMoreElements(); )
      {
        evt = (TrackEvent)e.nextElement();
          
        listener_args.clear();
        listener_args.addSymbol(FtsSymbol.get("selection"));
        listener_args.addSymbol(FtsSymbol.get(message));
        listener_args.addDouble(evt.getTime());
        listener_args.addObject(evt);
        ((FtsTrackObject)ftsObj).editorObject.requestListenersNotify(listener_args);      
      }
        
      listener_args.clear();
      listener_args.addSymbol(FtsSymbol.get("selection"));
      listener_args.addSymbol(FtsSymbol.get("end"));    
      ((FtsTrackObject)ftsObj).editorObject.requestListenersNotify(listener_args);  
    }
  }
}
//=================== MouseListener interface ===========================
public void mouseClicked(MouseEvent e){}
public void mousePressed(MouseEvent e)
{  
  int x = e.getX();
  int y = e.getY();
  int modifiers = e.getModifiers();
  
  if((modifiers & Toolkit.getDefaultToolkit().getMenuShortcutKeyMask()) != 0) // add markers
  {
    if(markersTrack != null)
    {
      if ((modifiers & InputEvent.SHIFT_MASK) == 0) //without shift
        markersSelection.deselectAll();
      
      markersTrack.requestInsertMarker( pa.getInvX(x));
    }
    else
      if(!isInSequence) 
        ((FtsTrackObject)ftsObj).requestInsertMarker( pa.getInvX(x));
  }
  else
  {
    if(markersTrack != null)
    {
      TrackEvent currMark = firstMarkerContaining(x, y);
      
      if(currMark!=null)
      { //click on marker				
        if ( !markersSelection.isInSelection( currMark)) 
        {
          if ((modifiers & InputEvent.SHIFT_MASK) == 0) //without shift
            markersSelection.deselectAll();
          
          markersSelection.select( currMark);
        }
        
        if( !isInSequence && (((FtsTrackObject)ftsObj).editorObject != null))
        {
          listener_args.clear();
          listener_args.addSymbol(FtsSymbol.get("click"));
          listener_args.addSymbol(FtsSymbol.get("marker"));
          listener_args.addDouble(pa.getInvX(x));
          listener_args.addObject(currMark);
          ((FtsTrackObject)ftsObj).editorObject.requestListenersNotify(listener_args);
          
          notifySelectionToListeners( markersSelection, "marker");
        }
      }
      else	
      {//click on empty
        if ((modifiers & InputEvent.SHIFT_MASK) == 0)
        {
          if( markersSelection != null)
            markersSelection.deselectAll();
        }
        if(!isInSequence)
          ((FtsTrackObject)ftsObj).requestNotifyGuiListeners( pa.getInvX(x), null);
        
        if( !isInSequence && (((FtsTrackObject)ftsObj).editorObject != null))
        {
          listener_args.clear();
          listener_args.addSymbol(FtsSymbol.get("click"));
          listener_args.addSymbol(FtsSymbol.get("background"));
          listener_args.addDouble(pa.getInvX(x));
          listener_args.addDouble(pa.getInvY(y));
          ((FtsTrackObject)ftsObj).editorObject.requestListenersNotify(listener_args);
          
          listener_args.clear();
          listener_args.addSymbol(FtsSymbol.get("selection"));
          listener_args.addSymbol(FtsSymbol.get("empty"));
          ((FtsTrackObject)ftsObj).editorObject.requestListenersNotify(listener_args);
        }
      }
    }
  }
}

public void mouseReleased(MouseEvent e){}
public void mouseEntered(MouseEvent e)
{
  requestFocus();
}
public void mouseExited(MouseEvent e){}
//=================== TrackDataListener interface ========================

public void objectChanged(Object spec, int index, String propName, Object propValue)
{
	if(propName != null && propName.equals("type"))
		if((propValue.toString()).equals("bar"))
			setLastBar((TrackEvent)spec);

	repaint();
}
public void lastObjectMoved(Object whichObject, int oldIndex, int newIndex, boolean fromClient)
{
	String type = (String)(((TrackEvent)whichObject).getProperty("type"));  
  if( type.equals("bar"))
		setLastBar((TrackEvent)whichObject);
	repaint();
}
public void objectMoved(Object whichObject, int oldIndex, int newIndex, boolean fromClient)
{
	if(whichObject == lastBar)
		setLastBar((TrackEvent)whichObject);
	repaint();
}

public void objectAdded(Object whichObject, int index)
{
  String type = (String)(((TrackEvent)whichObject).getProperty("type"));  
  if( type.equals("bar"))
		setLastBar((TrackEvent)whichObject);
	
  repaint();
}
public void objectsAdded(int maxTime){repaint();}
public void objectDeleted(Object whichObject, int oldIndex)
{
	if(whichObject == lastBar)
	{
		lastBarTime = -1.0;
		lastBar = null;

		int i = oldIndex-1;
		String lastType = "";
		TrackEvent last = markersTrack.getEventAt(i);
		if(last != null)
			lastType = (String)last.getProperty("type");
		while(last == null || !lastType.equals("bar"))
		{
			i--;
			last = markersTrack.getEventAt(i);
			if(last != null)
				lastType = (String)last.getProperty("type");
			else
				lastType = "";
		}
		if(last != null)
			setLastBar(last);
	}
	repaint();
}
public void trackCleared()
{
	lastBar = null;
	lastBarTime = -1.0;
	repaint();
}
public void startTrackUpload( TrackDataModel track, int size){}
public void endTrackUpload( TrackDataModel track){ initPropertiesToDraw();}
public void startPaste(){}
public void endPaste(){}
public void startUndoRedo(){}
public void endUndoRedo(){}
//==================== ListSelectionListener interface =====================

public void valueChanged(ListSelectionEvent e){repaint();}
//==================== TrackStateListener interface ========================
public void lock(boolean lock){}
public void active(boolean active){}
boolean stateRestored = false;
public void restoreEditorState(FtsTrackEditorObject editorState)
{
  if(!stateRestored)
  {    
    Enumeration props = editorState.getPropertiesToDraw();
    if(props == null) return;
    else
    {
      String propName;
      deInitPropertiesToDraw();
      
      for(Enumeration e = props; e.hasMoreElements();)
      {
        propName = (String)e.nextElement();
        propertyToDraw(propName, true);
      }
      validate();
      if(container.getEditorContainer().getFrame().isVisible())
        container.getEditorContainer().getFrame().pack();
      repaint();
    }    
    stateRestored = true;
  }
};
public void hasMarkers(FtsTrackObject markers, SequenceSelection markersSelection)
{
	this.markersSelection = markersSelection;
	markersSelection.addListSelectionListener(this);
	markersTrack = markers;
	markersTrack.addListener(this);	
}
public void updateMarkers(FtsTrackObject marks, SequenceSelection markSel)
{
  if(markersSelection != null)
    markersSelection.removeListSelectionListener(this);
	if(markersTrack != null)
    markersTrack.removeListener(this);
	if(marks != null)
		hasMarkers( marks, markSel);
	else
	{
		markersSelection = null;
		markersTrack = null;
	}	
	repaint();
	popup = null;
}
public void ftsNameChanged(String name){}
//===================== TrackListener interface =============================
public void trackAdded(Track track, boolean isUploading)
{
	track.getTrackDataModel().addTrackStateListener(this);	
};   
public void tracksAdded(int maxTime){};   
public void trackRemoved(Track track){};   
public void trackChanged(Track track){};   
public void trackMoved(Track track, int oldPosition, int newPosition){};   
public void sequenceStartUpload(){};
public void sequenceEndUpload(){}; 
public void sequenceClear(){}; 

//--- Ruler fields
FontMetrics fm;
PartitionAdapter utilityPartitionAdapter;
Geometry geometry;
FtsGraphicObject ftsObj;
PartitionAdapter pa;
SequenceEditor container;
public final static int TEMPO_HEIGHT = 25; 
public final static int DELTA_H = 11; 
public static Dimension tempoDimension = new Dimension(SequenceWindow.DEFAULT_WIDTH, TEMPO_HEIGHT);
public boolean isInSequence;
FtsTrackObject markersTrack = null;
SequenceSelection markersSelection = null;
String[] propertyNames = new String[0];
public boolean[] propertyToDraw = new boolean[0];
int numPropToDraw;

TempoBarPopupMenu popup = null;
Color tempoColor = new Color(165, 165, 165, 100);
Color selTempoColor = new Color(255, 0, 0, 100);
Color highTempoColor = new Color(0, 255, 0, 100);
Color selBorderColor = new Color(255, 0, 0, 100);
Color borderColor = new Color(165, 165, 165, 150);
Color cueBorderColor = new Color(0, 0, 255, 150);
Color cueColor = new Color(00, 0, 255, 50);

Color selLabelColor = new Color(255, 0, 0, 70);

Color selLineColor = new Color(255, 0, 0, 20);
Color lineColor = new Color(165, 165, 165, 25);

protected transient static FtsArgs listener_args = new FtsArgs();
}    




