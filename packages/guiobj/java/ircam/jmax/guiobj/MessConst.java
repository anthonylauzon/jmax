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

package ircam.jmax.guiobj;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.interactions.*;

public class MessConst extends Editable implements FtsObjectErrorListener, FtsIntValueListener
{
  private boolean isFlashing = false;
  private int minWidth = ObjectGeometry.INOUTLET_PAD + ObjectGeometry.HIGHLIGHTED_INOUTLET_WIDTH;
  private int cornerSize = 0;
  private int cornerSizeMax = 0;

  public MessConst( FtsGraphicObject theFtsObject) 
  {
    super( theFtsObject);

    cornerSizeMax = minWidth;
  }
    
  public String getArgs()
  {
    return ((FtsMessConstObject)ftsObject).getMessage();
  }
    
  public void errorChanged(boolean value) 
  {
    redraw();
  }
    
  public void valueChanged(int value) 
  {
    if (value != 0)
      isFlashing = true;
    else
      isFlashing = false;

    updateRedraw();
  }
  
//    public void setWidth(int w) 
//    {
//      if( w <= 0)
//        w = getDefaultWidth();
//      else
//        if(w < minWidth)
//  	w = minWidth;
    
//      super.setWidth(w);
    
//      if(w < cornerSizeMax)
//        cornerSize = w;
//      else
//        cornerSize = cornerSizeMax;

//      redraw();
//      fitToText();
//    }

  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {          
    // Send a bang message to the system inlet
    if (Squeack.isDown(squeack))
	((FtsMessConstObject)ftsObject).sendBang();
  }

//    // redefined from base class
//    public void setFont( Font theFont)
//    {
//      super.setFont( theFont);

//      cornerSizeMax = getFontMetrics().stringWidth("x");

//      setWidth(getWidth());
//      redraw();
//    }

  public void redefine( String text) 
  {
    ((FtsMessConstObject)ftsObject).setMessage( text);

    computeRenderer();
    renderer.update();

    redraw();
    redrawConnections();
  }

  public void redefined()
  {
    SwingUtilities.invokeLater(new Runnable(){
	public void run()
	{
	  redraw();
	  fitToText();
	}
      });
  }

  /* Inspector */    
  public void inspect()
  {
  }

  // ----------------------------------------
  // Text area offset
  // ----------------------------------------

  private static final int TEXT_X_OFFSET = 3;
  private static final int TEXT_Y_OFFSET = 2;

  public int getTextXOffset()
  {
    return TEXT_X_OFFSET;
  }

  public int getTextYOffset()
  {
    return TEXT_Y_OFFSET;
  }

  public int getTextWidthOffset()
  {
    return 5;
  }

  public int getTextHeightOffset()
  {
    return 5;
  }
    
  public Color getTextForeground()
  {
    if(isFlashing) 
      return Color.white;
    else if(ftsObject.isError())
      {
	if(isSelected())
	  return Color.gray.darker();
	else
	  return Color.gray;
      }
    else
      return Color.black;
  }

  public Color getTextBackground()
  {
    if(isFlashing) 
      return Color.black;
    else if(itsSketchPad.isLocked())
      return Color.white;
    else
      {
	if(isSelected())
	  return Color.lightGray.darker();
	else
	  return Color.lightGray;
      }
  }

  public boolean isMultiline()
  {
    return true;
  }

  // ----------------------------------------
  // Paint stuff
  // ----------------------------------------

  public void paint(Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    g.setColor(getTextBackground());
    
    if(itsSketchPad.isLocked())
      g.fillRect( x + 1, y + 1, w - 2, h - 2);
    else
      g.fill3DRect( x, y + 1, w - 1, h - 2, true);
    
    if(ftsObject.isError())
      {
	if(isSelected())
	  g.setColor(Color.gray.darker());
	else
	  g.setColor(Color.gray);
      }
    else
      g.setColor(Color.black);

    g.drawLine( x, y, x + cornerSize, y);
    g.drawLine( x, y, x, y + cornerSize/3);
    g.drawLine( x, y + h - 1, x + cornerSize, y + h - 1);
    g.drawLine( x, y + h - 1, x, y + h - 1 - cornerSize/3);

    paintInlets(g);
    paintOutlets(g);
    
    drawContent(g);
  }

  public void updatePaint(Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    g.setColor(getTextBackground());
    g.fillRect( x + 1, y + 1, w - 2, h - 2);
    
    g.setColor(Color.black);
    g.drawLine( x, y, x + cornerSize, y);
    g.drawLine( x, y, x, y + cornerSize/3);
    g.drawLine( x, y + h - 1, x + cornerSize, y + h - 1);
    g.drawLine( x, y + h - 1, x, y + h - 1 - cornerSize/3);
  
    drawContent(g);
  }
}






