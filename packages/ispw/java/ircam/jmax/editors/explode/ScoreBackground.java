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

package ircam.jmax.editors.explode;

import ircam.jmax.toolkit.*;
import ircam.jmax.JMaxApplication;

import java.awt.*;
import java.awt.image.ImageObserver;
import java.io.File;

import javax.swing.*;

/**
 * The background layer of a score. It builds the background Image for the 
 * piano roll representation */
public class ScoreBackground implements Layer{
  
  /** Constructor */
  public ScoreBackground ( ExplodeGraphicContext theGc)
  {
    super();
    
    gc = theGc;
    
  }

  /** builds an horizontal grid in the given graphic port
   * using the destination size*/
  private void drawTheGrid(Graphics g)
  {

    g.setFont(gridSubdivisionFont);
    Dimension d = gc.getGraphicDestination().getSize();

    g.setColor(Color.white);
    g.fillRect(0, 0, d.width, d.height);
    int positionY;

    // the minor subdivision
    g.setColor(horizontalGridLinesColor);
    for (int i = 0; i < 381; i+=9)
      {
	positionY = 409-i;
	g.drawLine(KEYX, positionY, d.width, positionY);
      }

    // the major subdivision lines and numbers
    g.setColor(Color.black);
    for (int j = 0; j < 381; j+=36)
      {
	positionY = 409-j;
	g.drawLine(56, positionY, d.width, positionY);
	g.drawString(""+j/3, 10, positionY+3);
      }

    // the last (127) line and number
    g.drawLine(56, 28, d.width, 28);
    g.drawString(""+127, 10, 31);

    // the piano keys...
    for (int i = 0; i < 127; i++)
      {
	positionY = 409-(i*3)-1;
	if (isAlteration(i)) 
	  {
	    g.setColor(Color.darkGray);
	    g.fillRect(KEYX, positionY, KEYWIDTH, 3);
	  }
	else 
	  {
	    g.setColor(Color.lightGray);
	    g.fillRect(KEYX, positionY, KEYWIDTH, 3);
	  }
      }

    // the vertical line at the end of keyboard
    g.drawLine(KEYEND, 28, KEYEND, 409);
  }

  public static boolean isAlteration(int note)
  {
    int local = note % 12;
    return (local == 1 || local == 3 || local == 6 || local == 8 || local == 10);
  }

  static int currentPressedKey = -1;
  static public void pressKey(int key, GraphicContext sgc)
  {
    if (key < 0 || key > 127) return;
    if (currentPressedKey != -1 && currentPressedKey != key) releaseKey(sgc);
    int positionY = 409-(key*3)-1;
    Graphics g = sgc.getGraphicDestination().getGraphics();
    g.setColor(Color.white);
    g.fillRect(KEYX, positionY, 25, 3);
    g.dispose();
    currentPressedKey = key;
  }

  static public void releaseKey(GraphicContext sgc)
  {
    if (currentPressedKey == -1) return;
    int positionY = 409-(currentPressedKey*3)-1;
    Graphics g = sgc.getGraphicDestination().getGraphics();
    if (isAlteration(currentPressedKey))
      g.setColor(Color.darkGray);
    else g.setColor(Color.lightGray);
    g.fillRect(KEYX, positionY, 25, 3);
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
	drawTheGrid(itsImage.getGraphics());
      }
    else if (itsImage.getHeight(gc.getGraphicDestination()) != d.height || itsImage.getWidth(gc.getGraphicDestination()) != d.width)
      {
	itsImage.flush();
	itsImage = null;
	System.gc();
	RepaintManager rp = RepaintManager.currentManager((JComponent)gc.getGraphicDestination());
	
	itsImage = gc.getGraphicDestination().createImage(d.width, d.height);
	drawTheGrid(itsImage.getGraphics());
	rp.markCompletelyDirty((JComponent)gc.getGraphicDestination());
      } 
    
    if (!g.drawImage(itsImage, 0, 0, gc.getGraphicDestination()))
      System.err.println("something wrong: incomplete Image  ");
    
    // paint the vertical grid
    int MIN_GRID = 6;
    int MAX_GRID = 50;
    ScrEvent tempEvent = new ScrEvent(null);
    
    int windowTime = gc.getAdapter().getInvX(gc.getGraphicDestination().getSize().width) - gc.getAdapter().getInvX(KEYEND)-1 ;
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
	
	g.drawLine(xPosition, 24, xPosition, 409);
	g.drawString(""+snappedTime, xPosition-20, 15);
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
  private int findBestTimeStep(int windowTime) 
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


  //--- Fields
  ExplodeGraphicContext gc;
  static Image itsImage;
  static boolean imageReady = true/*false*/;
  public static final Color horizontalGridLinesColor = new Color(187, 187, 187); 
  public static final Font gridSubdivisionFont = new Font("Helvetica", Font.PLAIN, 10);

  public static final int KEYX = 31;
  public static final int KEYWIDTH = 24;
  public static final int KEYEND = KEYX + KEYWIDTH;
}



