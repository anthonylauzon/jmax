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
import ircam.jmax.JMaxApplication;

import java.awt.*;
import java.awt.image.ImageObserver;
import java.io.File;
import java.beans.*;

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
		if (e.getPropertyName().equals("maximumPitch") || e.getPropertyName().equals("minimumPitch") ||
		    e.getPropertyName().equals("trackName"))
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

      if(!locked)
	  g.setColor(Color.white);
      else
	  g.setColor(ScoreBackground.OUT_RANGE_COLOR);
      g.fillRect(0, 0, d.width, d.height);

      g.setColor(OUT_RANGE_COLOR);
      g.fillRect(0 , 0 -delta, d.width, maxPitch);
      g.fillRect(0, minPitch+2 -delta, d.width, d.height-minPitch-2+delta);

      int positionY;
      g.setFont(gridSubdivisionFont);
      // the minor subdivision
    
      g.setColor(horizontalGridLinesColor);
      for (int i = 0; i < 381; i+=9)
	  {
	      positionY = SC_BOTTOM-i;
	      g.drawLine(KEYEND+1, positionY -delta, d.width, positionY -delta);
	  }

      // the major subdivision lines and numbers
      g.setColor(Color.black);
      g.drawLine(KEYEND+1, SC_BOTTOM+1 -delta, d.width,SC_BOTTOM+1-delta);
      for (int j = 36; j < 381; j+=36)
	  {
	      positionY = SC_BOTTOM-j;
	      g.drawLine(KEYEND+1, positionY -delta, d.width, positionY-delta);
	  }
      // the last (127) line
      g.drawLine(KEYEND+1, SC_TOP-3 -delta, d.width, SC_TOP-3 -delta);//?????
    
      g.setColor(Color.gray);
      for (int j = 0; j < 381; j+=36)
	  {
	      positionY = SC_BOTTOM-j;
	      g.drawString(""+j/3, 10 , positionY+3-delta);
	  }
      // the last (127) number
      g.drawString(""+127, 10, SC_TOP+3-delta);
      
      // the track name
      g.setFont(backFont);
      g.drawString(gc.getTrack().getName(), /*10*/2, 10);

      // the piano keys...
      for (int i = 0; i <= 127; i++)
	  {
	      positionY = SC_BOTTOM-(i*3)-2;
	      if (isAlteration(i)) 
		  {
		      g.setColor(Color.darkGray);
		      g.fillRect(KEYX, positionY-delta, SHORTKEYWIDTH, KEYHEIGHT);
		  }
	      else 
		  {
		      g.setColor(Color.white);
		      g.fillRect(KEYX, positionY -delta, KEYWIDTH, KEYHEIGHT);
		  }
	  }
      g.setColor(Color.black);
      //lines at top and bottom of the keybord 
      g.drawLine(KEYX, SC_TOP-3-delta, KEYEND,SC_TOP-3-delta);
      g.drawLine(KEYX, SC_BOTTOM+1-delta, KEYEND,SC_BOTTOM+1-delta);
      // the vertical line at the end of keyboard
      g.drawLine(KEYEND, SC_TOP-3-delta, KEYEND, SC_BOTTOM+1-delta);
      g.drawLine(KEYX-1, SC_TOP-3-delta, KEYX-1, SC_BOTTOM+1-delta); 
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
    
    drawVerticalGrid(g);
  }

      
    private void drawVerticalGrid(Graphics g)
    {
	AmbitusValue value = new AmbitusValue();
	UtilTrackEvent tempEvent = new UtilTrackEvent(value);
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

    public static final Color horizontalGridLinesColor = new Color(187, 187, 187); 
    //  public static final Font gridSubdivisionFont = new Font("Helvetica", Font.PLAIN, 10);
    public static final Font gridSubdivisionFont = new Font("Serif", Font.PLAIN, 10);
    public static final int KEYX = 31;
    public static final int KEYWIDTH = 24;
    public static final int SHORTKEYWIDTH = 16;
    public static final int KEYHEIGHT = 3;
    public static final int KEYEND = KEYX + KEYWIDTH;

    public static final int SC_BOTTOM = 409;
    public static final int SC_TOP = 28;

    public static final Color OUT_RANGE_COLOR = new Color(230, 230, 230);
    public static Font backFont = new Font("monospaced", Font.PLAIN, 10);
}



