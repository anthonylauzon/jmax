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

package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;
/*****************/
//jdk117-->jdk1.3//
import java.awt.geom.*;
/****************/
import java.awt.image.*;
import java.awt.image.ImageObserver;
import java.util.*;
import javax.swing.*;

import ircam.jmax.fts.*;
import ircam.jmax.utils.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.interactions.*;
import ircam.jmax.editors.patcher.menus.*;

//
// The "bang" graphic object.
//

public class Bang extends GraphicObject implements FtsIntValueListener, ImageObserver
{
    private Color itsFlashColor = Settings.sharedInstance().getUIColor();
    private static final int DEFAULT_WIDTH = 20;
    private static final int MINIMUM_WIDTH = 15;
    private static final int CIRCLE_ORIGIN = 3;
    private static final int DEFAULT_FLASH_DURATION = 125;

    private static Hashtable imageTable = new Hashtable();

    boolean flashing = false;
    boolean isResizing = false;
    int flashColorIndex = ColorPopUpMenu.getColorIndex("yellow")+1;  

    public Bang( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
    {
	super( theSketchPad, theFtsObject);

	int width = getWidth();

	if (width == -1)
	    setWidth( DEFAULT_WIDTH);
	else if (width <= MINIMUM_WIDTH)
	    setWidth( MINIMUM_WIDTH);
    }

    // redefined from base class
    public void setWidth( int theWidth)
    {
	if (theWidth < MINIMUM_WIDTH)
	    theWidth = MINIMUM_WIDTH;
    
	//don't save images during the resize
	if(!isResizing)
	    updateImages(theWidth, flashColorIndex);

	super.setWidth( theWidth);
	super.setHeight( theWidth);  
    }

    // redefined from base class
    public void setHeight( int theHeight)
    {
    }

    public boolean isSquare()
    {
	return true;
    }

    public void gotSqueack(int squeack, Point mouse, Point oldMouse)
    {
	if (Squeack.isDown(squeack))
	    ftsObject.sendMessage( -1, "bang", null);
    }

    public void resizing(boolean isRes)
    {
	isResizing = isRes;

	//save images at the end of resize
	if(!isResizing)
	    updateImages(getWidth(), flashColorIndex);
    }

    public void valueChanged(int value) 
    {
	int flash = value;

	if (flash <= 0)
	    {
		itsFlashColor = Settings.sharedInstance().getUIColor();
		flashing = false;
	    }
	else
	    {
		itsFlashColor = ColorPopUpMenu.getColorByIndex(flash);
		if(flash!=flashColorIndex)
		    {
			flashColorIndex = flash;
			updateFlashingImage(getWidth(), flashColorIndex);	
		    }
		flashing = true;
	    }
	updateRedraw();
    }

    //called from the popupmenu
    public void setColor(int index)
    {
	flashColorIndex = index;
	updateFlashingImage(getWidth(), flashColorIndex);	
	
	super.setColor(index);
    }
    
    void updateImages(int w, int color)
    {
	BangKey key = new BangKey(w, 0);
	if(!imageTable.containsKey(key))
	    {
		Image image = itsSketchPad.createImage(w-5, w-5);
		if(image==null) return;
	      
		itsSketchPad.prepareImage(image, this);
		
		/*Graphics g = image.getGraphics();
		  g.setColor( Settings.sharedInstance().getUIColor());		
		  g.fillRect(0, 0, w-5, w-5);
		  g.setColor(Color.black);		
		  g.drawOval(1, 1, w-7, w-7);*/

		/*****************/
		//jdk117-->jdk1.3//
		Graphics2D g2 = (Graphics2D)image.getGraphics();
		g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, 
				    RenderingHints.VALUE_ANTIALIAS_ON);
		
		g2.setPaint(Settings.sharedInstance().getUIColor());
		g2.fill(new Rectangle2D.Double(0, 0, w-5, w-5));
		//g2.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, 1.0f));
		g2.setPaint(Color.black);
		g2.draw(new Ellipse2D.Double(1, 1, w-7, w-7));
		/*****************/
		
		imageTable.put(key, image);	      
	    }
	updateFlashingImage(w, color);      
    }

    void updateFlashingImage(int w, int color)
    {
	BangKey key = new BangKey(w, color);
	if(!imageTable.containsKey(key))
	  {
	      Image image = itsSketchPad.createImage(w-5, w-5);
	      if(image==null) return;
	      
	      itsSketchPad.prepareImage(image, this);

	      /*****************/
	      //jdk117-->jdk1.3//

	      /*Graphics g = image.getGraphics();
		g.setColor( Settings.sharedInstance().getUIColor());
		g.fillRect(0, 0, w-5, w-5);
	      
		g.setColor(ColorPopUpMenu.getColorByIndex(color));
		g.fillOval(1, 1, w-7, w-7);
	      
		g.setColor(Color.black);
		g.drawOval(1, 1, w-7, w-7);*/
	      
	      Graphics2D g2 = (Graphics2D)image.getGraphics();
	      g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, 
				  RenderingHints.VALUE_ANTIALIAS_ON);
	      g2.setPaint(Settings.sharedInstance().getUIColor());
	      g2.fill(new Rectangle2D.Double(0, 0, w-5, w-5));
	      g2.setPaint(ColorPopUpMenu.getColorByIndex(color));
	      Ellipse2D.Double el = new Ellipse2D.Double(1, 1, w-7, w-7);
	      g2.fill(el);
	      g2.setPaint(Color.black);
	      g2.draw(el);
	      /*****************/

	      imageTable.put(key, image);
	  }	
    }

    /* ImageObserver interface*/
    public boolean imageUpdate(Image img, int infoflags, int x, int y, int width, int height)
    {
	return true;
    }

    public void paint( Graphics g) 
    {
	int x = getX();
	int y = getY();
	int w = getWidth();
	int h = getHeight();
	Image image;

	if(isSelected())
	    g.setColor( Settings.sharedInstance().getUIColor().darker());
	else
	    g.setColor( Settings.sharedInstance().getUIColor());
	
	g.fill3DRect( x + 1, y + 1, w - 2, h - 2, true);

	/*****************/
	//jdk117-->jdk1.3//
	((Graphics2D)g).setRenderingHint(RenderingHints.KEY_ANTIALIASING, 
	  RenderingHints.VALUE_ANTIALIAS_ON);

	if(isSelected() || isResizing)
	    {		
		if(flashing)
		    {
			g.setColor(itsFlashColor);
			g.fillOval( x + CIRCLE_ORIGIN,
				    y + CIRCLE_ORIGIN,
				    w - 2*(CIRCLE_ORIGIN+1)+1,
				    h - 2*(CIRCLE_ORIGIN+1)+1);
		    }
		
		g.setColor(Color.black);
		g.drawOval(x + CIRCLE_ORIGIN,
			   y + CIRCLE_ORIGIN,
			   w - 2*(CIRCLE_ORIGIN+1) +1,
			   h - 2*(CIRCLE_ORIGIN+1) +1);
		
	    }
	else
	    {
		if(flashing)
		    image = (Image)imageTable.get(new BangKey(w, flashColorIndex));
		else
		    image = (Image)imageTable.get(new BangKey(w, 0));
		
		if(image!=null)
		    g.drawImage(image, x+2, y+2, this);
		else
		    updateImages(w, flashColorIndex);
	    }

	/*****************/
	//jdk117-->jdk1.3//
	((Graphics2D)g).setRenderingHint(RenderingHints.KEY_ANTIALIASING, 
					 RenderingHints.VALUE_ANTIALIAS_OFF);

	super.paint( g);
    }
  public void updatePaint(Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();
    
    Image image;

    if(flashing)
	image = (Image)imageTable.get(new BangKey(w, flashColorIndex));
    else
	image = (Image)imageTable.get(new BangKey(w, 0));

    if(image!=null)
	g.drawImage(image, x+2, y+2, this);
    else
	updateImages(w, flashColorIndex);
  }

  //popup interaction 
  public void popUpUpdate(boolean onInlet, boolean onOutlet, SensibilityArea area)
  {
    super.popUpUpdate(onInlet, onOutlet, area);
    BangPopUpMenu.update(this);
    ObjectPopUp.addMenu(BangPopUpMenu.getInstance());
  }
  public void popUpReset()
  {
    super.popUpReset();
    ObjectPopUp.removeMenu(BangPopUpMenu.getInstance());
  }
    /*
     * Double key class (image width and flash color) to the image HashTable 
     */
  public class BangKey 
    {
	int itsWidth;
	int itsColorIndex;

	public BangKey(int width, int colorIndex)
	{
	    itsWidth = width;
	    itsColorIndex = colorIndex;
	}
	
	public int getWidth()
	{
	    return itsWidth;
	}
	
	public int getColorIndex()
	{
	    return itsColorIndex;
	}
	public int hashCode()
	{
	    return itsWidth+itsColorIndex;
	}   
	public boolean equals(Object key)
	{
	    return ((itsWidth == ((BangKey)key).getWidth())&&(itsColorIndex == ((BangKey)key).getColorIndex()));
	}
    }
}






