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
			createDisplayer();
		
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

public void createDisplayer()
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
}

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

void setDisplayer(Displayer d)
{
	/*oldDisplayer = gc.getDisplayer();
	gc.setDisplayer(d);*/
	oldDisplayer = d;
}

public void resetDisplayer()
{
	Displayer disp = gc.getDisplayer();
	gc.setDisplayer(oldDisplayer);
	oldDisplayer = disp;
}

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
public final static int TEMPO_HEIGHT = 30/*15*/; 
JLabel displayLabel;
Displayer oldDisplayer;
}    




