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

package ircam.jmax.editors.sequence.renderers;

import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.editors.sequence.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.JMaxApplication;

import java.beans.*;
import java.awt.*;
import java.awt.image.ImageObserver;
import java.io.*;
import java.net.*;
import java.util.*;

import javax.swing.*;

/**
* The background layer of a score. It builds the background Image for the 
 * piano roll representation */
public class PartitionBackground implements Layer, ImageObserver{
  
  /** Constructor */
  public PartitionBackground ( SequenceGraphicContext theGc)
{
    super();
    
    gc = theGc;
		
    gc.getTrack().getPropertySupport().addPropertyChangeListener(new PropertyChangeListener() {
			public void propertyChange(PropertyChangeEvent e)
		{		
				if(e.getPropertyName().equals("rangeMode") || e.getPropertyName().equals("repaint"))
				{
					toRepaintBack = true;
					gc.getGraphicDestination().repaint();
				}
				else
					if(e.getPropertyName().equals("locked"))
					{
						locked = ((Boolean)e.getNewValue()).booleanValue();
						toRepaintBack = true;
						gc.getGraphicDestination().repaint();
					}
		}
		});
		gc.getAdapter().getGeometry().getPropertySupport().addPropertyChangeListener(new PropertyChangeListener() {
			public void propertyChange(PropertyChangeEvent e)
				{		
				String name = e.getPropertyName();
				if( name.equals("gridMode"))
				{
					toRepaintBack = true;
					gc.getGraphicDestination().repaint();
				}
				}
		});				
}

public static int getMaxPitchInStaff(int max)
{
	if(max <= 9) return 9;
	else if(max <= 18) return 18;
	else if( max <= 33) return 33;
	else if( max <= 42) return 42;
	else if( max <= 57) return 57;
	else if( max <= 62) return 62;
	else if( max <= 78) return 78;
	else if( max <= 86) return 86;
	else if( max <= 102) return 102;
	else if( max <= 110) return 110;
	else return 127;
}

public static int getMinPitchInStaff(int min)
{
	if(min >= 111) return 111;
	else if(min >= 103) return 103;
	else if( min >= 87) return 87;
	else if( min >= 79) return 79;
	else if( min >= 63) return 63;
	else if( min >= 58) return 58;
	else if( min >= 43) return 43;
	else if( min >= 34) return 34;
	else if( min >= 19) return 19;
	else if( min >= 10) return 10;
	else return 0;
}

private boolean staffIsDrawable(int numGroup, int maxPitch, int minPitch)
{		
	if(((PartitionAdapter)gc.getAdapter()).getRangeMode() == MidiTrackEditor.WHOLE_RANGE)
		return true;
	else
		  switch(numGroup)
			{
				case 1:
					return (minPitch <= 9);
				case 2:
					return (minPitch <= 18 && maxPitch >= 10);
				case 3:
					return (minPitch <= 33 && maxPitch >= 19);
				case 4:
					return (minPitch <= 42 && maxPitch >= 34);
				case 5:
					return (minPitch <= 57 && maxPitch >= 43);
				case 6:
					return (minPitch <= 62 && maxPitch >= 58);
				case 7:
					return (minPitch <= 78 && maxPitch >= 63);
				case 8:
					return (minPitch <= 86 && maxPitch >= 79);
				case 9:
					return (minPitch <= 102 && maxPitch >= 87);
				case 10:
					return (minPitch <= 110 && maxPitch >= 103);
				case 11: 
					return (maxPitch >= 111);
				default: 
					return true;
			}
}

/* NOTE: draw always black staff after gray staff */
private void drawBlackStaff(Graphics g, int startLine, int key)
{
	int transp = ((PartitionAdapter)gc.getAdapter()).getVerticalTransp();
	Dimension d = gc.getGraphicDestination().getSize();
	int positionY = SC_BOTTOM;
	int keyPosition;
	int start = SC_BOTTOM-startLine*STEP-transp;
	
	g.setColor(Color.black);
	for (int i = 0; i < 5; i++)                       /*draw horiz lines*/                              
	{
		positionY = start-i*STEP;
		g.drawLine(KEYX, positionY, d.width, positionY);
	}
	if(key == VIOLIN_KEY)                             /*draw key*/
	{
		g.setFont( SequenceFonts.getFont(52));
		g.drawString( SequenceFonts.violinKey, KEYX+5, positionY+34);
	}
	else
	{
		g.setFont( SequenceFonts.getFont(36));
		g.drawString( SequenceFonts.bassKey, KEYX+6, positionY+26); 
	}
	g.drawLine(KEYX, start, KEYX, start-4*STEP);     /*draw vertical lines*/
	g.setColor(Color.gray);
	g.drawLine(KEYEND, start, KEYEND, start-4*STEP);
}

private void drawGrayStaff(Graphics g, int startLine)
{
	int transp = ((PartitionAdapter)gc.getAdapter()).getVerticalTransp();
	Dimension d = gc.getGraphicDestination().getSize();
	int positionY = SC_BOTTOM;
	int start = SC_BOTTOM-startLine*STEP-transp;
	
	g.setColor(horizontalGridLinesColor);		
	for (int j = 0; j < 4; j++)
	{
		positionY = start-(j-1)*STEP;
		g.drawLine(KEYX, positionY, d.width, positionY);
	}
	g.setColor(Color.black);                           /*draw vertical lines*/
	g.drawLine(KEYX, start+STEP, KEYX, start-2*STEP);
	g.setColor(Color.gray);
	g.drawLine(KEYEND, start+STEP, KEYEND, start-2*STEP);
}

/** builds an horizontal grid in the given graphic port
* using the destination size*/
private void drawHorizontalGrid(Graphics g)
{
	PartitionAdapter pa = (PartitionAdapter)(gc.getAdapter());
	int maxPitch = pa.getMaxPitch();
	int minPitch = pa.getMinPitch();
	int transp = pa.getVerticalTransp();
	
	Dimension d = gc.getGraphicDestination().getSize();
	((Graphics2D)g).setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);	
	
	/********** Background ****************************************************/
	if(!locked)
		g.setColor(Color.white);
	else
		g.setColor(ScoreBackground.OUT_RANGE_COLOR);
	g.fillRect(0, 0, d.width, d.height);
	
	/********** Track Name ***************************************************/
	if( gc.isInSequence())
	{
		g.setColor(Color.gray);
		g.setFont(ToggleBar.toggleBarFont);	
		g.drawString(gc.getTrack().getName(), 2, d.height - 2);
	}
	/********* First Bass Line ***********************************************/
	if(staffIsDrawable(2, maxPitch, minPitch))
		drawGrayStaff( g, 5);
	if(staffIsDrawable(1, maxPitch, minPitch))
		drawBlackStaff( g, 0, BASS_KEY);
	/********* Second Bass Line **********************************************/
	if(staffIsDrawable(4, maxPitch, minPitch))
		drawGrayStaff( g, 12);
	if(staffIsDrawable(3, maxPitch, minPitch))
		drawBlackStaff( g, 7, BASS_KEY);
	/********* Third Bass Line ***********************************************/	
	if(staffIsDrawable(6, maxPitch, minPitch))
	{
		g.setColor(horizontalGridLinesColor); 
		int positionY = SC_BOTTOM-19*STEP-transp;
		g.drawLine(KEYX, positionY, d.width, positionY);
		g.setColor(Color.black);
		g.drawLine(KEYX, positionY+STEP, KEYX, positionY-STEP);
		g.setColor(Color.gray);
		g.drawLine(KEYEND, positionY+STEP, KEYEND, positionY-STEP);
	}
	if(staffIsDrawable(5, maxPitch, minPitch))
		drawBlackStaff( g, 14, BASS_KEY);
	/********* First Violin Line **********************************************/	
	if(staffIsDrawable(8, maxPitch, minPitch))
		drawGrayStaff( g, 25);	
	if(staffIsDrawable(7, maxPitch, minPitch))
		drawBlackStaff( g, 20, VIOLIN_KEY);
	/********* Second Violin Line *********************************************/	
	if(staffIsDrawable(10, maxPitch, minPitch))
		drawGrayStaff( g, 32);
	if(staffIsDrawable(9, maxPitch, minPitch))
		drawBlackStaff( g, 27, VIOLIN_KEY);
	/********* Third Violin Line **********************************************/	
	if(staffIsDrawable(11, maxPitch, minPitch))
		drawBlackStaff( g, 34, VIOLIN_KEY);
	/********* Vertical Lines at the end of keyboard **************************/
	/*g.setColor(Color.gray);
	g.drawLine(KEYEND, 0, KEYEND, d.height);*/
}

//???????????????????????????????
static int currentPressedKey = -1;
static public void pressKey(int key, GraphicContext sgc){}
static public void releaseKey(GraphicContext sgc){}

/**
* Layer interface. Draw the background */
public void render( Graphics g, int order)
{
	Dimension d = gc.getGraphicDestination().getSize();
	
	if (itsImage == null) 
	{
		itsImage = gc.getGraphicDestination().createImage(d.width, d.height);
		drawHorizontalGrid(itsImage.getGraphics());
	}
	else if (itsImage.getHeight(gc.getGraphicDestination()) != d.height || itsImage.getWidth(gc.getGraphicDestination()) != d.width || toRepaintBack == true)
	{
		itsImage.flush();
		itsImage = null;
		System.gc();
		RepaintManager rp = RepaintManager.currentManager((JComponent)gc.getGraphicDestination());
		
		itsImage = gc.getGraphicDestination().createImage(d.width, d.height);
		drawHorizontalGrid(itsImage.getGraphics());
		rp.markCompletelyDirty((JComponent)gc.getGraphicDestination());
		toRepaintBack = false;
	} 
	
	if (!g.drawImage(itsImage, 0, 0, gc.getGraphicDestination()))
		System.err.println("something wrong: incomplete Image  ");
	
	if( gc.getGridMode() == MidiTrackEditor.TIME_GRID)
		drawVerticalGrid(g);
	/*else
		drawMeasures(g);*/
}
 
/*private void drawMeasures(Graphics g)
{
	FtsTrackObject markers = gc.getMarkersTrack();
	if( markers!= null)
	{
		TrackEvent evt;
		Dimension d = gc.getGraphicDestination().getSize();
		
    for (Enumeration e = markers.intersectionSearch( gc.getAdapter().getInvX(ScoreBackground.KEYEND), 
																										 gc.getAdapter().getInvX(d.width-ScoreBackground.KEYEND)); e.hasMoreElements();) 
		{
			evt = (TrackEvent) e.nextElement();
			evt.getRenderer().render( evt, g, false, gc);
		}
	}
}*/

private void drawVerticalGrid(Graphics g)
{
	UtilTrackEvent tempEvent = new UtilTrackEvent(new AmbitusValue(), gc.getDataModel());
	Dimension d = gc.getGraphicDestination().getSize();
	int windowTime = (int) (gc.getAdapter().getInvX(d.width) - gc.getAdapter().getInvX(KEYEND))-1 ;
	int timeStep;
	
	timeStep = findBestTimeStep(windowTime);
	
	g.setColor(Color.lightGray);
	
	int xPosition;
	int snappedTime;
	
	for (int i=gc.getLogicalTime()+timeStep; i<gc.getLogicalTime()+windowTime; i+=timeStep) 
	{
		snappedTime = (i/timeStep)*timeStep;
		tempEvent.setTime(snappedTime);
		xPosition = gc.getAdapter().getX(tempEvent);
		
		g.drawLine(xPosition, 0, xPosition, d.height);
	}
}

/**
* Layer interface. */
public void render(Graphics g, Rectangle r, int order)
{
	render(g, order);
}

public ObjectRenderer getObjectRenderer()
{
	return null; // no events in this layer!
}

/**
* utility function: find a "good" time step for vertical subdivisions
 * given a window time size.
 * The politic is to find 
 * 1) at least 5 divisions 
 * 2) if possible, a power of 10  */  
public static int findBestTimeStep(int windowTime) 
{
	
	// find a good time interval between two grid
	
	int pow = 1;
	
	while (windowTime/pow>0) 
	{
		pow *= 10;
	}
	
	pow = pow/10;
	
	if (windowTime/pow < 5) pow = pow/5;
	if (pow == 0) return 1;
	return pow;
}

/* ImageObserver interface*/
public boolean imageUpdate(Image img, int infoflags, int x, int y, int width, int height)
{
	return true;
}

//--- Fields
SequenceGraphicContext gc;
Image itsImage;
boolean toRepaintBack = false;
boolean locked = false;
public static final Color horizontalGridLinesColor = new Color(220, 220, 220);   
public static final Font gridSubdivisionFont = new Font("Serif", Font.PLAIN, 10);
//public static final Font bracketFont = new Font("Palatino", Font.PLAIN, 90);

public static final int KEYX = 27;
public static final int KEYWIDTH = 28;
public static final int KEYHEIGHT = 3;
public static final int KEYEND = KEYX + KEYWIDTH;

public static int VIOLIN_KEY = 0;
public static int BASS_KEY = 1;

public static final int STEP = 8;

public static final int SC_BOTTOM = 334;
public static final int SC_TOP = 30;
}





