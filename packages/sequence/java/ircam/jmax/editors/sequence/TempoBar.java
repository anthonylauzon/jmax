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
		
	popup.update();
	return popup;
}

public void setPropertyToDraw(String propName, boolean toDraw)
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
		validate();
		container.getEditorContainer().getFrame().pack();
	}
		
	repaint();
}

void initPropertiesToDraw()
{
	int i = 0;
	int count = markersTrack.getPropertyCount();
	propertyNames = new String[ count];
	propertyToDraw = new boolean[ count];
	for(Enumeration e = markersTrack.getPropertyNames(); e.hasMoreElements();)
	{
		propertyNames[i] = (String)e.nextElement();	
		propertyToDraw[i] = (propertyNames[i].equals("tempo") || propertyNames[i].equals("meter"));
		i++;
	}
	numPropToDraw = 2;
}

void createPopup()
{
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
		Dimension d = getSize();		
		g.setFont( Displayer.displayFont);
		
		for (Enumeration e = markersTrack.intersectionSearch( pa.getInvX(ScoreBackground.KEYEND), 
																													pa.getInvX(d.width-ScoreBackground.KEYEND)); e.hasMoreElements();) 
		{
			evt = (TrackEvent) e.nextElement();
			type = (String)(evt.getProperty("type"));
			x = pa.getX(evt)+getXIndentation();
			
			if(type.equals("tempo"))
			{
				if( markersSelection.isInSelection(evt))
					g.setColor( selTempoColor);
				else
					g.setColor( tempoColor);	
			}
			else
			{
				if( markersSelection.isInSelection(evt))
					g.setColor( Color.red);
				else
					g.setColor( Color.darkGray);	
			}
			
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
						g.drawString( str, x - strw/2 + 1, h);
					}
					h+=DELTA_H;
				}
		}
	}
}

int getXIndentation()
{
	if( isInSequence)
		return 2+TrackContainer.BUTTON_WIDTH;
	else
		return 0;
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

//=================== MouseListener interface ===========================
public void mouseClicked(MouseEvent e){}
public void mousePressed(MouseEvent e)
{
  if(markersTrack != null)
  {
    int x = e.getX();
    int y = e.getY();
    int modifiers = e.getModifiers();    
		TrackEvent currMark = firstMarkerContaining(x, y);

    if(currMark!=null)
    { //click on marker				
      if ( !markersSelection.isInSelection( currMark)) 
      {
        if ((modifiers & InputEvent.SHIFT_MASK) == 0) //without shift
         markersSelection.deselectAll();
        
        markersSelection.select( currMark);
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

public void objectChanged(Object spec, String propName, Object propValue){repaint();}
public void lastObjectMoved(Object whichObject, int oldIndex, int newIndex){repaint();}
public void objectMoved(Object whichObject, int oldIndex, int newIndex){repaint();}
public void objectAdded(Object whichObject, int index){repaint();}
public void objectsAdded(int maxTime){repaint();}
public void objectDeleted(Object whichObject, int oldIndex){repaint();}
public void trackCleared(){repaint();}
public void startTrackUpload( TrackDataModel track, int size){}
public void endTrackUpload( TrackDataModel track){ initPropertiesToDraw();}
public void startPaste(){}
public void endPaste(){}
public void trackNameChanged(String oldName, String newName) {}
//==================== ListSelectionListener interface =====================

public void valueChanged(ListSelectionEvent e){repaint();}
//==================== TrackStateListener interface ========================
public void lock(boolean lock){}
public void active(boolean active){}
public void restoreEditorState(FtsTrackEditorObject editorState){};
public void hasMarkers(FtsTrackObject markers, SequenceSelection markersSelection)
{
	this.markersSelection = markersSelection;
	markersSelection.addListSelectionListener(this);
	markersTrack = markers;
	markersTrack.addListener(this);
	
	initPropertiesToDraw();
}
public void updateMarkers(FtsTrackObject marks, SequenceSelection markSel)
{
	markersSelection.removeListSelectionListener(this);
	markersTrack.removeListener(this);
	if(marks != null)
		hasMarkers( marks, markSel);
	else
	{
		markersSelection = null;
		markersTrack = null;
	}
	initPropertiesToDraw();
	
	repaint();
	popup = null;
}
//===================== TrackListener interface =============================
public void trackAdded(Track track)
{
	track.getTrackDataModel().addTrackStateListener(this);	
};   
public void tracksAdded(int maxTime){};   
public void trackRemoved(Track track){};   
public void trackChanged(Track track){};   
public void trackMoved(Track track, int oldPosition, int newPosition){};   

//--- Ruler fields
FontMetrics fm;
PartitionAdapter utilityPartitionAdapter;
Geometry geometry;
FtsGraphicObject ftsObj;
PartitionAdapter pa;
SequenceEditor container;
public final static int TEMPO_HEIGHT = 23; 
public final static int DELTA_H = 10; 
public static Dimension tempoDimension = new Dimension(SequenceWindow.DEFAULT_WIDTH, TEMPO_HEIGHT);
public boolean isInSequence;
FtsTrackObject markersTrack = null;
SequenceSelection markersSelection = null;
String[] propertyNames;
public boolean[] propertyToDraw;
int numPropToDraw;

TempoBarPopupMenu popup = null;
Color tempoColor = new Color(165, 165, 165, 100);
Color selTempoColor = new Color(255, 0, 0, 100);
Color highTempoColor = new Color(0, 255, 0, 100);
}    




