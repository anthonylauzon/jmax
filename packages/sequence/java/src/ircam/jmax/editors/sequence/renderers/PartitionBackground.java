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

package ircam.jmax.editors.sequence.renderers;

import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.editors.sequence.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.MaxApplication;

import java.beans.*;
import java.awt.*;
import java.awt.image.ImageObserver;
import java.io.*;
import java.net.*;

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
		if (e.getPropertyName().equals("maximumPitch") || e.getPropertyName().equals("minimumPitch"))
		    {
			toRepaintBack = true;
			gc.getGraphicDestination().repaint();
		    }
	    }
    });
  }
  /** builds an horizontal grid in the given graphic port
   * using the destination size*/
  private void drawHorizontalGrid(Graphics g)
  {
    PartitionAdapter pa = (PartitionAdapter)(gc.getAdapter());
    int maxPitch = pa.getY(pa.getMaxPitch());
    int minPitch = pa.getY(pa.getMinPitch());
    int delta = pa.getVerticalTransp();

    Dimension d = gc.getGraphicDestination().getSize();

    g.setColor(Color.white);
    g.fillRect(0, 0, d.width, d.height);

    g.setColor(ScoreBackground.OUT_RANGE_COLOR);
    g.fillRect(0, 0 -delta, d.width, maxPitch);
    g.fillRect(0, minPitch -delta, d.width, d.height-minPitch+delta);
    
    // the track name
    g.setColor(Color.gray);
    g.setFont(ToggleBar.toggleBarFont);
    g.drawString(gc.getTrack().getName(), 10, 10);

    int positionY = SC_BOTTOM;
    g.setFont(gridSubdivisionFont);

    // the minor subdivision
    for(int k=0;k<2;k++)
      {
	g.setColor(Color.black);
	for (int i = 0; i < 5; i++)
	  {
	    positionY = SC_BOTTOM-(i+k*7)*step -delta;
	    g.drawLine(KEYX, positionY, d.width, positionY);
	  }
	g.drawImage(SequenceImages.getImage("faClef"), KEYX+4, positionY-1, this);

	g.setColor(horizontalGridLinesColor);
     
	for (int j = 5; j < 7; j++)
	  {
	    positionY = SC_BOTTOM-(j+k*7)*step -delta;
	    g.drawLine(KEYX, positionY, d.width, positionY);
	  }
      }
    g.setColor(Color.black);
    g.drawString("15", KEYEND-15, SC_BOTTOM-3 -delta);
    g.drawString("8", KEYEND-14, SC_BOTTOM-step*7-3 -delta);    

    for (int i = 14; i < 19; i++)
      {
	positionY = SC_BOTTOM-(i*step) -delta;
	g.drawLine(KEYX, positionY, d.width, positionY);
      }
	
    g.drawImage(SequenceImages.getImage("faClef"), KEYX+4, positionY-1, this);

    g.setColor(horizontalGridLinesColor);
    positionY = SC_BOTTOM-(19*step) -delta;
    g.drawLine(KEYX, positionY, d.width, positionY);
    
    for(int k=0;k<2;k++)
      {
	g.setColor(Color.black);
	for (int i = 0; i < 5; i++)
	  {
	    positionY = SC_BOTTOM-(i+k*7+20)*step -delta;
	    g.drawLine(KEYX, positionY, d.width, positionY);
	  }
	
	g.drawImage(SequenceImages.getImage("violinClef"), KEYX+4, positionY-7, this);

	g.setColor(horizontalGridLinesColor);
     
	for (int j = 5; j < 7; j++)
	  {
	    positionY = SC_BOTTOM-(j+k*7+20)*step -delta;
	    g.drawLine(KEYX, positionY, d.width, positionY);
	  }
      }

    g.setColor(Color.black);
    for (int i = 0; i < 5; i++)
      {
	positionY = SC_BOTTOM-(i+34)*step -delta;
	g.drawLine(KEYX, positionY, d.width, positionY);
      }

    g.drawImage(SequenceImages.getImage("violinClef"), KEYX+4, SC_TOP-7 -delta, this);

    g.drawString("15", KEYEND-9, SC_TOP+2 -delta);
    g.drawString("8", KEYEND-6, SC_TOP+step*7+2 -delta);    

    // the vertical line at the end of keyboard
    g.drawLine(KEYX, SC_TOP-delta, KEYX, SC_BOTTOM-delta);
    g.setColor(Color.gray);
    g.drawLine(KEYEND, 0, KEYEND, d.height);
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
    
    drawVerticalGrid(g);
  }

      
    private void drawVerticalGrid(Graphics g)
    {
	UtilTrackEvent tempEvent = new UtilTrackEvent(new AmbitusValue());
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
  public static final Color horizontalGridLinesColor = new Color(220, 220, 220);   
  public static final Font gridSubdivisionFont = new Font("Serif", Font.PLAIN, 10);
  public static final int KEYX = 27;
  public static final int KEYWIDTH = 28;
  public static final int KEYHEIGHT = 3;
  public static final int KEYEND = KEYX + KEYWIDTH;

  public static final int step = 8;

  public static final int SC_BOTTOM = 334;
  public static final int SC_TOP = 30;
  //75 is the number of notes without alteration  
  //public static final float note_step = (float)((SC_BOTTOM-SC_TOP)/(float)75.0);    
}





