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

public class TempoBar extends JPanel
{
  public TempoBar( Geometry geom, SequenceGraphicContext gc, boolean isInSequence)
  {
    super();
	
		setFont(SequencePanel.rulerFont);
    fm = getFontMetrics( SequencePanel.rulerFont);
    this.geometry = geom;
		this.gc = gc;
    
		setBackground(Color.white);
    setLayout( new BoxLayout( this, BoxLayout.Y_AXIS));
		utilityPartitionAdapter = new PartitionAdapter(geometry, null);
    
		if( !isInSequence)
		{
			//createDisplayer();
			
			gc.getDataModel().addTrackStateListener(new TrackStateListener(){
				public void lock(boolean lock){}
				public void active(boolean active){}
				public void restoreEditorState(FtsTrackEditorObject editorState){};
				public void hasMarkers(FtsTrackObject markers, SequenceSelection markersSelection)
				{
					markersSelection.addListSelectionListener( new ListSelectionListener(){
						public void valueChanged(ListSelectionEvent e)
				    {
							repaint();
						}
					});
					TempoBar.this.gc.getMarkersTrack().addListener( new TrackDataListener() {
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
					});
				}
			});		
		}
							
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
	if( gc.getGridMode() == TrackEditor.TIME_GRID)
		paintTimeGrid(g);
	else
		paintMeasures(g);
}

public void paintMeasures(Graphics g)
{
	Rectangle clip = g.getClipBounds();
	g.setColor( Color.white);
	g.fillRect( clip.x, clip.y, clip.width, clip.height);
	
	FtsTrackObject markers = gc.getMarkersTrack();
	if( markers!= null)
	{
		TrackEvent evt;		
		int x;
		String type;
		Dimension d = getSize();
		PartitionAdapter pa = (PartitionAdapter)gc.getAdapter();
		SequenceSelection sel = gc.getMarkersSelection();
		
		g.setFont( Displayer.displayFont);
		
		for (Enumeration e = markers.intersectionSearch( gc.getAdapter().getInvX(ScoreBackground.KEYEND), 
																										 gc.getAdapter().getInvX(d.width-ScoreBackground.KEYEND)); e.hasMoreElements();) 
		{
			evt = (TrackEvent) e.nextElement();
			type = (String)(evt.getProperty("type"));
			x = pa.getX(evt);
			
			if(type.equals("tempo"))
			{
				if( sel.isInSelection(evt))
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
				if( sel.isInSelection(evt))
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

public void paintTimeGrid(Graphics g)
{
	int xPosition;
	int snappedTime;
	String timeString;
	Dimension d = getSize();
	Rectangle clip = g.getClipBounds();
	
	int logicalTime = -geometry.getXTransposition();
	int windowTime = (int) (gc.getAdapter().getInvX(d.width) - gc.getAdapter().getInvX(ScoreBackground.KEYEND))-1;
	int timeStep = ScoreBackground.findBestTimeStep(windowTime);	
	
	g.setColor( Color.white);
	g.fillRect( clip.x, clip.y, clip.width, clip.height);
	
	g.setColor( Color.lightGray);
	
	for (int i=logicalTime+timeStep; i<logicalTime+windowTime; i+=timeStep) 
	{
		snappedTime = (i/timeStep)*timeStep;
		xPosition = utilityPartitionAdapter.getX(snappedTime)+getXIndentation();
		g.drawLine(xPosition, 0, xPosition, TEMPO_HEIGHT);		  
	}
}

int getXIndentation()
{
	if( gc.isInSequence())
		return 3+TrackContainer.BUTTON_WIDTH;
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

//--- Ruler fields
Dimension tempoDimension = new Dimension(SequenceWindow.DEFAULT_WIDTH, TEMPO_HEIGHT);
FontMetrics fm;
PartitionAdapter utilityPartitionAdapter;
Geometry geometry;
SequenceGraphicContext gc;
public final static int TEMPO_HEIGHT = 20; 
/*JLabel displayLabel;
Displayer oldDisplayer;*/

Color tempoColor = new Color(165, 165, 165, 100);
Color selTempoColor = new Color(255, 0, 0, 100);
Color highTempoColor = new Color(0, 255, 0, 100);
}    




