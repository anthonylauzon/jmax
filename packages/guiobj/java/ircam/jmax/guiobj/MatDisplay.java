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
import java.awt.image.*;
import java.awt.event.*;
//import javax.swing.*;
import javax.swing.JPopupMenu;
import javax.swing.SwingUtilities;
import java.util.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.menus.*;
import ircam.jmax.editors.patcher.interactions.*;
import ircam.jmax.toolkit.*;

public class MatDisplay extends GraphicObject 
{
  private static final int minWidth = 18;
  private static final int minHeight = 18;

  /* silent agreement with client */
  static final int DEFAULT_WIDTH = 130;
  static final int DEFAULT_HEIGHT = 130;

  private static final Color defaultMinColor = new Color(227, 255, 227);
  private static final Color defaultMaxColor = Color.black;

  int width = 130;
  int height = 130;
  MemoryImageSource source;
  Image image;
  IndexColorModel icm;
  Color minColor = defaultMinColor;
  Color maxColor = defaultMaxColor;
  int imageWidth;

  ImageObserver observer;

  public MatDisplay(FtsGraphicObject theFtsObject)
  {
    super(theFtsObject);

    int width = (((FtsMatDisplayObject)ftsObject).nSize) * (((FtsMatDisplayObject)ftsObject).nZoom);
    int height = (((FtsMatDisplayObject)ftsObject).mSize) * (((FtsMatDisplayObject)ftsObject).mZoom);
    byte pixels[] = ((FtsMatDisplayObject)ftsObject).pixels;

    icm = generateColorModel(Color.white, Color.black);
    source = new MemoryImageSource(width, height, icm, pixels, 0, width);
    image = Toolkit.getDefaultToolkit().createImage(source);

    observer = new ImageObserver() {
	public boolean imageUpdate( Image img, int infoflags, int x, int y, int width, int height)
	{
	  return true;
	}
      };
  }

  public void setDefaults()
  {    
    setWidth( DEFAULT_WIDTH);
    setHeight( DEFAULT_HEIGHT);
  }

  public void display()
  {
    int width = (((FtsMatDisplayObject)ftsObject).nSize) * (((FtsMatDisplayObject)ftsObject).nZoom);
    int height = (((FtsMatDisplayObject)ftsObject).mSize) * (((FtsMatDisplayObject)ftsObject).mZoom);
    imageWidth = width;

    if((width * height) > 0)
      {
	byte pixels[] = ((FtsMatDisplayObject)ftsObject).pixels;
	image = Toolkit.getDefaultToolkit().createImage(new MemoryImageSource(width, height, icm, pixels, 0, width));
	/*redraw();*/
	updateRedraw();
      }
  }

  public void setColor(Color min, Color max)
  {
    minColor = min;
    maxColor = max;

    icm = generateColorModel(minColor, maxColor);

    display();
  }
  
  public IndexColorModel generateColorModel(Color min, Color max) {
    byte[] r = new byte[256];
    byte[] g = new byte[256];
    byte[] b = new byte[256];
    int r1, g1, b1, r2, g2, b2;    

    r1 = min.getRed(); //Color wich is associated to index 0
    g1 = min.getGreen();
    b1 = min.getBlue();

    r2 = max.getRed(); //Color wich is associated to index 255
    g2 = max.getGreen();
    b2 = max.getBlue();

    for(int i=0;i<256;i++)
      {
	r[i] = (byte)( ((r2-r1)*i)/255 + r1 );
	g[i] = (byte)( ((g2-g1)*i)/255 + g1 ); 
	b[i] = (byte)( ((b2-b1)*i)/255 + b1 );
      }
    
    return new IndexColorModel(8, 256, r, g, b);
  }

  public void setCurrentBounds(int x, int y, int w, int h)
  {
    super.setCurrentBounds( x, y, w, h);
    
    if(w <= 2)
      w = 2;
    
    if(h <= 2)
      h = 2;

    width = w - 2;
    height = h - 2;

    ((FtsMatDisplayObject)ftsObject).setWindowSize(height, width);
  }

  public void setWidth(int w) 
  {
    if(w <= 0)
      w = DEFAULT_WIDTH;
    else if (w < minWidth)
      w = minWidth;

    width = w - 2;

    ((FtsMatDisplayObject)ftsObject).setWindowSize(height, width);

    super.setWidth(w);
  }

  public void setHeight(int h)
  {
    if(h <= 0)
      h = DEFAULT_HEIGHT;
    else if (h < minHeight)
      h = minHeight;
   
    height = h - 2 ;
    ((FtsMatDisplayObject)ftsObject).setWindowSize(height, width);

    super.setHeight(h);
  }

  public void redefined()
  {
    setDefaults();
  }

  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {          
  }

  protected SensibilityArea findSensibilityArea( int mouseX, int mouseY)
  {
    if ((mouseY >= getY() + getHeight() - ObjectGeometry.V_RESIZE_SENSIBLE_HEIGHT)
	&& (mouseX >= getX() + getWidth() / 2))
      {
	return SensibilityArea.get(this, Squeack.VRESIZE_HANDLE);
      }
    else
      return super.findSensibilityArea( mouseX, mouseY);
  }

  public Color getBackgroundColor()
  {
    if (isSelected())
      return minColor.darker();
    else
      return minColor;
  }

  public void drawMatrix(Graphics g, int x, int y)
  {
    int scroll = ((FtsMatDisplayObject)ftsObject).getScaledScroll();

    if(scroll == 0)
      //g.drawImage(image, x+1, y+1, itsSketchPad);
      g.drawImage(image, x+1, y+1, observer);
    else
      {
	/*g.drawImage(image, x+1 - scroll, y+1, itsSketchPad);
	  g.drawImage(image, x+1 - scroll + imageWidth, y+1, itsSketchPad);*/
	g.drawImage(image, x+1 - scroll, y+1, observer);
	g.drawImage(image, x+1 - scroll + imageWidth, y+1, observer);
      }
  }

  public void paint(Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    //background
    g.setColor(getBackgroundColor());
    g.fillRect( x, y, w - 1, h - 1);
    
    g.setColor(Color.black);
    g.drawRect( x, y, w - 1, h - 1);
    paintInlets(g);
    Rectangle oldClip = g.getClipRect();
    Rectangle viewRect = SwingUtilities.computeIntersection(x+1,y+1,w-2,h-2, itsSketchPad.getEditorContainer().getViewRectangle());
    g.setClip( viewRect);  
    
    drawMatrix(g, x, y);

    g.setClip(oldClip);
  }

  public void updatePaint(Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    Rectangle oldClip = g.getClipRect();
    Rectangle viewRect = SwingUtilities.computeIntersection(x+1,y+1,w-2,h-2, itsSketchPad.getEditorContainer().getViewRectangle());
    g.setClip( viewRect);  
    
    drawMatrix(g, x, y);
    
    g.setClip(oldClip);
  }

  public JPopupMenu getRunModePopUpMenu()
  {
    MatDispRModePopUp.update(this);
    return MatDispRModePopUp.popup;
  }

  public ObjectControlPanel getControlPanel()
  {
    ObjectControlPanel panel = new MatDispControlPanel();
    panel.update( this);
    return panel;
  }
  
  public boolean isInspectable()
  {
    return true;
  }
}
