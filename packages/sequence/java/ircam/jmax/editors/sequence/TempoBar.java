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

public class TempoBar extends JPanel implements TrackDataListener, TrackStateListener, TrackListener, ListSelectionListener
{
  public TempoBar( Geometry geom, FtsGraphicObject obj)
  {
    super();
	
		setFont(SequencePanel.rulerFont);
    fm = getFontMetrics( SequencePanel.rulerFont);
    this.geometry = geom;
		this.ftsObj = obj;
		this.isInSequence = (ftsObj instanceof FtsSequenceObject);
		pa = new PartitionAdapter(geometry, null);
    
		setBackground(Color.white);
    setLayout( new BoxLayout( this, BoxLayout.Y_AXIS));
    
		if( !isInSequence)
		{
			//createDisplayer();
			((FtsTrackObject)ftsObj).addTrackStateListener(this);		
		}
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
	}

/*public void createDisplayer()
{
	JPanel labelPanel = new JPanel();
	labelPanel.setLayout( new BoxLayout( labelPanel, BoxLayout.X_AXIS));
	labelPanel.setOpaque(false);
	
	displayLabel = new JLabel();
	displayLabel.setFont( Displayer.displayFont);
	displayLabel.setForeground( Color.gray);
	displayLabel.setBackground( Color.red);
	displayLabel.setPreferredSize( new Dimension(102, 15));
	displayLabel.setMaximumSize( new Dimension(102, 15));
	displayLabel.setMinimumSize( new Dimension(102, 15));
	
	labelPanel.add( Box.createRigidArea( new Dimension(5, 0)) );
	labelPanel.add( displayLabel);
	labelPanel.add( Box.createHorizontalGlue());
	
	add(labelPanel);
	add(displayLabel);
	
	add(Box.createVerticalGlue());
	 
	setDisplayer( new Displayer(){
		public void display(String text)
	  {
			displayLabel.setText( text);
		}
	});
	validate();
}*/

public void paintComponent(Graphics g)
{
	paintMeasures(g);
}

public void paintMeasures(Graphics g)
{
	Rectangle clip = g.getClipBounds();
	g.setColor( Color.white);
	g.fillRect( clip.x, clip.y, clip.width, clip.height);
	
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
					
				Double tempo = (Double)evt.getProperty("tempo");
				if(tempo!=null)
				{
					String str = ""+tempo.intValue();
					int strw = fm.stringWidth(str);
					g.drawString( str, x - strw/2 + 1, 12);
				}
			}
			else if(type.equals("bar"))
			{					
				if( markersSelection.isInSelection(evt))
					g.setColor( Color.red);
				else
					g.setColor( Color.darkGray);			
					
				String meterUp = ((MarkerValue)evt.getValue()).getMeterValue();
				String meterDown = ((MarkerValue)evt.getValue()).getMeterType();
				
				if(meterUp != null && meterDown != null)
				{
					g.drawString(""+meterUp, x - 3, 8);
					g.drawString(""+meterDown, x - 3, 16);
				}
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

/*void setDisplayer(Displayer d)
{
	oldDisplayer = d;
}

public void resetDisplayer()
{
	Displayer disp = gc.getDisplayer();
	gc.setDisplayer(oldDisplayer);
	oldDisplayer = disp;
}*/

public Dimension getPreferredSize()
{ return tempoDimension; }

public Dimension getMinimumSize()
{ return tempoDimension; }

//=================== TrackDataListener interface ========================

public void objectChanged(Object spec, String propName, Object propValue){repaint();}
public void lastObjectMoved(Object whichObject, int oldIndex, int newIndex){repaint();}
public void objectMoved(Object whichObject, int oldIndex, int newIndex){repaint();}
public void objectAdded(Object whichObject, int index){repaint();}
public void objectsAdded(int maxTime){repaint();}
public void objectDeleted(Object whichObject, int oldIndex){repaint();}
public void trackCleared(){repaint();}
public void startTrackUpload( TrackDataModel track, int size){}
public void endTrackUpload( TrackDataModel track){}
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
	repaint();
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
Dimension tempoDimension = new Dimension(SequenceWindow.DEFAULT_WIDTH, TEMPO_HEIGHT);
FontMetrics fm;
PartitionAdapter utilityPartitionAdapter;
Geometry geometry;
FtsGraphicObject ftsObj;
PartitionAdapter pa;
public final static int TEMPO_HEIGHT = 20; 
public boolean isInSequence;
FtsTrackObject markersTrack = null;
SequenceSelection markersSelection = null;
/*JLabel displayLabel;
Displayer oldDisplayer;*/

Color tempoColor = new Color(165, 165, 165, 100);
Color selTempoColor = new Color(255, 0, 0, 100);
Color highTempoColor = new Color(0, 255, 0, 100);
}    




