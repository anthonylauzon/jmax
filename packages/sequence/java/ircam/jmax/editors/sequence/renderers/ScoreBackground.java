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

import java.awt.*;
import java.awt.image.ImageObserver;
import java.io.File;
import java.beans.*;
import java.util.*;

import javax.swing.*;

/**
* The background layer of a score. It builds the background Image for the		
 * piano roll representation */
public class ScoreBackground implements Layer{
	
	/** Constructor */
	public ScoreBackground ( SequenceGraphicContext theGc)
	{   
		super();
		
		gc = theGc; 
		
		gc.getTrack().getPropertySupport().addPropertyChangeListener(new PropertyChangeListener() {
			public void propertyChange(PropertyChangeEvent e)
		{		
				String name = e.getPropertyName();
				if (name.equals("rangeMode") || name.equals("trackName"))
				{
					toRepaintBack = true;
					gc.getGraphicDestination().repaint();
				}
				else
					if(name.equals("locked"))
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
	if(max <= 11) return 11;
	else if(max <= 23) return 23;
	else if( max <= 35) return 35;
	else if( max <= 47) return 47;
	else if( max <= 59) return 59;
	else if( max <= 71) return 71;
	else if( max <= 83) return 83;
	else if( max <= 95) return 95;
	else if( max <= 107) return 107;
	else if( max <= 119) return 119;
	else return 127;
}

public static int getMinPitchInStaff(int min)
{
	if(min >= 120) return 120;
	else if(min >= 108) return 108;
	else if( min >= 96) return 96;
	else if( min >= 84) return 84;
	else if( min >= 72) return 72;
	else if( min >= 60) return 60;
	else if( min >= 48) return 48;
	else if( min >= 36) return 36;
	else if( min >= 24) return 24;
	else if( min >= 12) return 12;
	else return 0;
}	

/** builds an horizontal grid in the given graphic port
* using the destination size*/
private void drawHorizontalGrid(Graphics g)
{
	PartitionAdapter pa = (PartitionAdapter)(gc.getAdapter());
	int maxPitch = pa.getMaxPitch();
	int minPitch = pa.getMinPitch();
	int maxPitchY = pa.getY(maxPitch);
	int minPitchY = pa.getY(minPitch);
	int transp = pa.getVerticalTransp();
	
	Dimension d = gc.getGraphicDestination().getSize();
	/********** Background ******************************************************/
	if(!locked)
		g.setColor(Color.white);
	else
		g.setColor(ScoreBackground.OUT_RANGE_COLOR);
	g.fillRect(0, 0, d.width, d.height);
	
	/********** Gray Lines ******************************************************/
	int positionY;
	int maxGray = (SC_BOTTOM - maxPitchY)/GRAY_STEP;
	int minGray = (SC_BOTTOM - minPitchY)/GRAY_STEP;
	g.setColor(ScoreRenderer.horizontalGridLinesColor);
	for (int i = minGray+1; i <= maxGray; i++)
	{
		positionY = SC_BOTTOM-i*GRAY_STEP;
		g.drawLine(KEYEND+1, positionY -transp, d.width, positionY -transp);
	}
	/********** Black Lines *****************************************************/
	g.setColor(Color.black);
	int maxBlack = (SC_BOTTOM - maxPitchY)/BLACK_STEP;
	int minBlack = (SC_BOTTOM - minPitchY)/BLACK_STEP;
	
	g.drawLine(KEYEND+1, minPitchY+2-transp, d.width, minPitchY+2-transp);
	for(int j = minBlack+1; j <= maxBlack; j++)
	{
		positionY = SC_BOTTOM-j*BLACK_STEP;
		g.drawLine(KEYEND+1, positionY-transp, d.width, positionY-transp);
	}	
	g.drawLine(KEYEND+1, maxPitchY-2-transp, d.width, maxPitchY-2-transp);
	/********** Numbers **********************************************************/
	g.setColor(Color.gray);
	g.setFont(ScoreRenderer.gridSubdivisionFont);
	
	g.drawString(""+minPitch, 10, minPitchY+4-transp);
	for(int j = minBlack+1; j <= maxBlack; j++)
	{
		positionY = SC_BOTTOM-j*BLACK_STEP;
		g.drawString(""+(j*BLACK_STEP)/3, 10 , positionY+3-transp);
	}
	g.drawString(""+maxPitch, 10, maxPitchY+2-transp);
	/********** Track Name ********************************************************/
	if( gc.isInSequence())
	{
		g.setFont(backFont);
		g.drawString(gc.getTrack().getName(), 2, d.height - 2);
	}
	/********** Piano Keys *********************************************************/
	for (int i = minPitch; i <= maxPitch; i++)
	{
		positionY = SC_BOTTOM-(i*3)-2;
		if (isAlteration(i)) 
		{
			g.setColor(Color.darkGray);
			g.fillRect(KEYX, positionY-transp, SHORTKEYWIDTH, KEYHEIGHT);
		}
		else 
		{
			g.setColor(Color.white);
			g.fillRect(KEYX, positionY -transp, KEYWIDTH, KEYHEIGHT);
		}
	}
	/********** lines at top and bottom of the keybord  ****************************/
	g.setColor(Color.black);
	g.drawLine(KEYX, maxPitchY-2-transp, KEYEND, maxPitchY-2-transp);
	g.drawLine(KEYX, minPitchY+2-transp, KEYEND, minPitchY+2-transp);
	
	/********** the vertical line at the end of keyboard ****************************/
	g.drawLine(KEYEND, maxPitchY-2-transp, KEYEND, minPitchY+2-transp);
	g.drawLine(KEYX-1, maxPitchY-2-transp, KEYX-1, minPitchY+2-transp);					 
}

public static boolean isAlteration(int note)
{
	int local = note % 12;
	return (local == 1 || local == 3 || local == 6 || local == 8 || local == 10);
}

static int currentPressedKey = -1;
static public void pressKey(int key, SequenceGraphicContext sgc)
{
	if (key < 0 || key > 127) return;
	if (currentPressedKey != -1 && currentPressedKey != key) releaseKey(sgc);
	PartitionAdapter pa = (PartitionAdapter)(sgc.getAdapter());
	int positionY = SC_BOTTOM-(key*KEYHEIGHT)-2-pa.getVerticalTransp();
	
	Graphics g = sgc.getGraphicDestination().getGraphics();
	g.setColor(OUT_RANGE_COLOR);
	if (isAlteration(key))
		g.fillRect(KEYX, positionY, SHORTKEYWIDTH, KEYHEIGHT);
	else
		g.fillRect(KEYX, positionY, KEYWIDTH, KEYHEIGHT);
	
	g.dispose();
	currentPressedKey = key;
}

static public void releaseKey(SequenceGraphicContext sgc)
{
	if (currentPressedKey == -1) return;
	PartitionAdapter pa = (PartitionAdapter)(sgc.getAdapter());
	int positionY = SC_BOTTOM-(currentPressedKey*KEYHEIGHT)/*-1*/-2-pa.getVerticalTransp();
	
	Graphics g = sgc.getGraphicDestination().getGraphics();
	if (isAlteration(currentPressedKey))
	{
		g.setColor(Color.darkGray);
		g.fillRect(KEYX, positionY, SHORTKEYWIDTH, KEYHEIGHT);
	}
	else
	{ 
		g.setColor(Color.white);
		g.fillRect(KEYX, positionY, KEYWIDTH, KEYHEIGHT);
	}
	g.dispose();
	currentPressedKey = -1;
}

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
	
	if( gc.getGridMode() == TrackEditor.TIME_GRID)
		drawVerticalGrid(g);
}

private void drawVerticalGrid(Graphics g)
{
	AmbitusValue value = new AmbitusValue();
	UtilTrackEvent tempEvent = new UtilTrackEvent(value, gc.getDataModel());
	Dimension d = gc.getGraphicDestination().getSize();
	int windowTime = (int) (gc.getAdapter().getInvX(d.width) - gc.getAdapter().getInvX(KEYEND))-1 ;
	int timeStep, xPosition, snappedTime;
	int maxY = ((PartitionAdapter)gc.getAdapter()).getMaxScoreY();
	
	timeStep = findBestTimeStep(windowTime);
	
	g.setColor(ScoreRenderer.verticalLinesColor);

	for (int i=gc.getLogicalTime()+timeStep; i<gc.getLogicalTime()+windowTime; i+=timeStep) 
	{
		snappedTime = (i/timeStep)*timeStep;
		tempEvent.setTime(snappedTime);
		xPosition = gc.getAdapter().getX(tempEvent);
		
		g.drawLine(xPosition, 0, xPosition, maxY);
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
	
	if(pow != 0)
		pow = pow/10;
	
	if (windowTime/pow < 5) pow = pow/5;
	if (pow == 0) return 1;
	return pow;
}


//--- Fields
SequenceGraphicContext gc;
private Image itsImage;
boolean toRepaintBack = false;
boolean locked = false;
boolean imageReady = true;

public static final int KEYX = 31;
public static final int KEYWIDTH = 24;
public static final int SHORTKEYWIDTH = 16;
public static final int KEYHEIGHT = 3;
public static final int KEYEND = KEYX + KEYWIDTH;

public static final int SC_BOTTOM = 409;
public static final int SC_TOP = 28;

public static final int GRAY_STEP = 9;
public static final int BLACK_STEP = 36;

public static final Color OUT_RANGE_COLOR = new Color(230, 230, 230);
public static Font backFont = new Font("monospaced", Font.PLAIN, 10);
}



