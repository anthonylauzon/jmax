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

package ircam.jmax.editors.bpf.renderers;

import ircam.jmax.editors.bpf.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.MaxApplication;

import java.awt.*;
import java.awt.image.ImageObserver;
import java.io.File;
import java.util.*;

import javax.swing.*;
import java.beans.*;

import java.text.NumberFormat;
/**
 * The background layer of a monodimensionalTrackeditor. It builds the background Image */
public class BpfBackground implements Layer, ImageObserver{
  
  /** Constructor */
  public  BpfBackground( BpfGraphicContext theGc)
  {
      super();
      gc = theGc;

      fm = gc.getGraphicDestination().getFontMetrics(BpfPanel.rulerFont);
  }

  /**
   * Layer interface. Draw the background */
  public void render( Graphics g, int order)
  {
    Dimension d = gc.getGraphicDestination().getSize();
    
    if (itsImage == null) 
      {
	itsImage = gc.getGraphicDestination().createImage(d.width, d.height);
	drawHorizontalLine(itsImage.getGraphics(), d.width, d.height);
      }
    else if (itsImage.getHeight(gc.getGraphicDestination()) != d.height || itsImage.getWidth(gc.getGraphicDestination()) != d.width || toRepaintBack == true)
      {
	itsImage.flush();
	itsImage = null;
	System.gc();
	RepaintManager rp = RepaintManager.currentManager((JComponent)gc.getGraphicDestination());
	
	itsImage = gc.getGraphicDestination().createImage(d.width, d.height);
	drawHorizontalLine(itsImage.getGraphics(), d.width, d.height);
	rp.markCompletelyDirty((JComponent)gc.getGraphicDestination());
	toRepaintBack = false;
      } 
    
    if (!g.drawImage(itsImage, 0, 0, gc.getGraphicDestination()))
      System.err.println("something wrong: incomplete Image  ");
    
    drawVerticalGrid(g, d.width, d.height);
  }

  private void drawHorizontalLine(Graphics g, int w, int h)
  {
      g.setColor(Color.white);
      
      g.fillRect(0, 0, w, h);
      
      /*g.setColor(Color.lightGray);

	int y0 = ((BpfAdapter)gc.getAdapter()).getY(0); 
	g.drawLine(0, y0, w, y0);*/
  }

  private void drawVerticalGrid(Graphics g, int w, int h)
  {
      int windowTime = (int)(gc.getAdapter().getInvX(w)) - 1 ;    
      int logicalTime = -gc.getAdapter().getGeometry().getXTransposition();      
      int timeStep = findBestTimeStep(windowTime-logicalTime);
    
      g.setColor(Color.lightGray);
	
      int xPosition;
      int snappedTime;
    
      for (int i=gc.getLogicalTime()+timeStep; i<gc.getLogicalTime()+windowTime; i+=timeStep) 
	  {
	      snappedTime = (i/timeStep)*timeStep;
	      xPosition = gc.getAdapter().getX(snappedTime);
	      g.drawLine(xPosition, 0, xPosition, h);
	  }

      int y0 = (gc.getAdapter()).getY((float)0.0); 
      /*String maxString, minString;
	maxString = ""+numberFormat.format(gc.getAdapter().getInvY(0));
	minString = ""+numberFormat.format(gc.getAdapter().getInvY(gc.getGraphicDestination().getSize().height));  

	g.setColor(Color.gray);
	g.setFont(BpfPanel.rulerFont);
	g.drawString(maxString, PartitionBackground.KEYEND - SwingUtilities.computeStringWidth(fm, maxString)-2, 10);
	g.drawString(minString, PartitionBackground.KEYEND - SwingUtilities.computeStringWidth(fm, minString)-2, h-2);   
      */
      g.setColor(Color.black);
      g.drawLine(0, y0, w, y0);
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
  BpfGraphicContext gc;
  Image itsImage;
  boolean toRepaintBack = false;
  FontMetrics fm;
  
  public static final int KEYX = 31;
  public static final int KEYWIDTH = 24;
  public static final int KEYEND = KEYX + KEYWIDTH;

  static public NumberFormat numberFormat;
  static 
  {
      numberFormat = NumberFormat.getInstance();
      numberFormat.setMaximumFractionDigits(2);
      numberFormat.setGroupingUsed(false);
  }

  public static final Color OUT_RANGE_COLOR = new Color(230, 230, 230);
}




