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
    
    drawRectangle(g, d.width, d.height);

    //draw zero line if != from minimum
    if(gc.getFtsObject().getMinimumValue() != 0.0)
	{
	    int y0 = (gc.getAdapter()).getY((float)0.0); 
	    int x0 = (gc.getAdapter()).getX((float)0.0); 

	    g.setColor(Color.gray);
	    g.drawLine(x0, y0, d.width, y0);
	}
  }

  private void drawHorizontalLine(Graphics g, int w, int h)
  {
      g.setColor(BPF_COLOR);      
      g.fillRect(0, 0, w, h);      
  }

    private void drawRectangle(Graphics g, int w, int h)
    {	   
	int x0 = gc.getAdapter().getX(0);

	float maxTime = gc.getMaximumTime() - gc.getAdapter().getInvWidth(BpfAdapter.DX);
	int xMax = gc.getAdapter().getX(maxTime);

	int yMin = gc.getAdapter().getY(gc.getFtsObject().getMinimumValue());
	int yMax = gc.getAdapter().getY(gc.getFtsObject().getMaximumValue());

	g.setColor(Color.white);	
	g.drawRect(x0, yMax, xMax-x0, yMin - yMax);
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
  
  public static final Color BPF_COLOR = new Color(230, 230, 230);
}




