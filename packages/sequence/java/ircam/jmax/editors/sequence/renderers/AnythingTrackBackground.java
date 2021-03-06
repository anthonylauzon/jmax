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

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.JMaxApplication;

import java.awt.*;
import java.awt.image.ImageObserver;
import java.io.File;
import java.util.*;

import javax.swing.*;
import java.beans.*;

import java.text.NumberFormat;
/**
* The background layer of a monodimensionalTrackeditor. It builds the background Image */
public class AnythingTrackBackground implements Layer, ImageObserver{
  
  /** Constructor */
  public AnythingTrackBackground( SequenceGraphicContext theGc)
{
    super();
    gc = theGc;
    
    gc.getTrack().getPropertySupport().addPropertyChangeListener(new PropertyChangeListener() {
      public void propertyChange(PropertyChangeEvent e)
    {		
        if (e.getPropertyName().equals("trackName"))
		    {
          toRepaintBack = true;
          gc.getGraphicDestination().repaint();
		    }
        else if(e.getPropertyName().equals("repaint"))
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
    fm = gc.getGraphicDestination().getFontMetrics(ToggleBar.toggleBarFont);
}

static int DEFAULT_IMAGE_WIDTH = 1500;

/**
* Layer interface. Draw the background */
public void render( Graphics g, int order)
{
  Dimension d = gc.getGraphicDestination().getSize();
  int image_width = (d.width > DEFAULT_IMAGE_WIDTH) ? d.width : DEFAULT_IMAGE_WIDTH;
  
  if (itsImage == null) 
  {
    itsImage = gc.getGraphicDestination().createImage(image_width, d.height);
    drawHorizontalLine(itsImage.getGraphics(), image_width, d.height);
  }
  else if (itsImage.getHeight(gc.getGraphicDestination()) < d.height || itsImage.getWidth(gc.getGraphicDestination()) < d.width || toRepaintBack == true)
  {
    itsImage.flush();
    itsImage = null;
    System.gc();
    RepaintManager rp = RepaintManager.currentManager((JComponent)gc.getGraphicDestination());
    
    itsImage = gc.getGraphicDestination().createImage(image_width, d.height);
    drawHorizontalLine(itsImage.getGraphics(), image_width, d.height);
    rp.markCompletelyDirty((JComponent)gc.getGraphicDestination());
    toRepaintBack = false;
  } 
  
  g.drawImage(itsImage, 0, 0, gc.getGraphicDestination());
  
  drawVerticalGrid(g, image_width, d.height);
}

private void drawHorizontalLine(Graphics g, int w, int h)
{
  if(!locked)
    g.setColor(Color.white);
  else
    g.setColor(ScoreBackground.OUT_RANGE_COLOR);
  
  g.fillRect(0, 0, w, h);
  
  Image image = gc.getTrack().getTrackDataModel().getType().getIcon().getImage();    
  g.drawImage(image , 2, h-15, this);
  
  //vertical line at time 0
  g.setColor(Color.black);
  g.drawLine(PartitionBackground.KEYEND, 0, PartitionBackground.KEYEND, h);
}

private void drawVerticalGrid(Graphics g, int w, int h)
{
  UtilTrackEvent tempEvent = new UtilTrackEvent(new AmbitusValue(), gc.getDataModel());
  int windowTime = (int)(gc.getAdapter().getInvX(w) - gc.getAdapter().getInvX(KEYEND)) - 1 ;
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
    
    g.drawLine(xPosition, 0, xPosition, h);
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
}




