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

package ircam.jmax.guiobj;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.awt.geom.*;
import java.awt.image.*;

import ircam.jmax.fts.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.interactions.*;
import ircam.jmax.toolkit.*;

//
// The "toggle" graphic object.
//

public class Toggle extends GraphicObject implements FtsIntValueListener
{
  static transient final int DEFAULT_WIDTH = 20;
  private transient static final int MINIMUM_WIDTH = 15;

  private transient static final Color itsCrossColor = new Color(0, 0, 128);

  private transient boolean isToggled = false;

  private BufferedImage buff;
  private Graphics2D buffG;

  public Toggle(FtsGraphicObject theFtsObject) 
  {
    super(theFtsObject);

    updateOffScreenBuffer();
  }

  public void setDefaults()
  {
    super.setWidth( DEFAULT_WIDTH);
    super.setHeight( DEFAULT_WIDTH);
  }

  void updateOffScreenBuffer()
  {
    int w = getWidth() - 6;
    if( w <= 0) w = DEFAULT_WIDTH - 6;
    
    buff = new BufferedImage( w, w, BufferedImage.TYPE_INT_RGB);
    buffG = buff.createGraphics();
    buffG.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_OFF);
  }

  // redefined from base class

  public void setWidth( int theWidth)
  {
    if ( theWidth <= 0)
      setWidth( DEFAULT_WIDTH);
    else if ( theWidth < MINIMUM_WIDTH)
      theWidth = MINIMUM_WIDTH;

    super.setWidth( theWidth);
    super.setHeight( theWidth);
  
    updateOffScreenBuffer();
  }

  // redefined from base class

  public void setHeight( int theHeight)
  {
  }

  public void setCurrentBounds( int x, int y, int w, int h)
  {
    super.setCurrentBounds( x, y, w, h);
    updateOffScreenBuffer();
  }

  public boolean isSquare()
  {
    return true;
  }

  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {
    if (Squeack.isDown(squeack))
	((FtsToggleObject)ftsObject).sendBang();
  }

  public void valueChanged(int value) 
  {
    isToggled = (value == 1);

    drawContent( buffG, 0, 0, getWidth() - 6, getWidth() - 6);
    updateRedraw();
  }

  public void redefined()
  {
    setDefaults();
  }

  public void paint(Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    if ( !isSelected())
	g.setColor( Settings.sharedInstance().getUIColor());
    else
	g.setColor( Settings.sharedInstance().getUIColor().darker());
    
    g.fill3DRect( x + 1, y + 1, w - 2, h - 2, true);

    if (isToggled) 
      {
	((Graphics2D)g).setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_OFF);
	g.setColor( itsCrossColor);
	g.drawLine( x + 4, y + 4, x + w - 6, y + h - 6);
	g.drawLine( x + w - 6, y + 4, x + 4,y + h - 6);
	((Graphics2D)g).setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
      }
    
    super.paint( g);
  }

  public void updatePaint(Graphics g) 
  {
    g.drawImage( buff, getX() + 3, getY() + 3, itsSketchPad);  
  }  

  public void drawContent( Graphics g, int x, int y, int w, int h)
  {
    g.setColor( Settings.sharedInstance().getUIColor());
    g.fillRect( x, y, w, h);

    if (isToggled) 
      {
	g.setColor( itsCrossColor);
	g.drawLine( x + 1, y + 1, x + w - 3, y + h - 3);
	g.drawLine( x + w - 3, y + 1, x + 1, y + h - 3);
      }
  }

  public ObjectControlPanel getControlPanel()
  {
    return new ToggleControlPanel( this);
  }

  public boolean isInspectable()
  {
    return true;
  }
}
