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

package ircam.jmax.toolkit;

// import javax.swing.*;
import javax.swing.ImageIcon;
import javax.swing.JPopupMenu;

import java.awt.*;
import java.awt.image.*;

/**
 * The base class for tools: it handles the name and the icon,
 * the mounting of the interaction modules, and their activation/deactivation.
 */
abstract public class Tool implements StatusBarClient{

  /**
   * constructor 
   */
  public Tool(String theName, ImageIcon theImageIcon) 
  {
    setName(theName);
    setIcon(theImageIcon);
    cursor = Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR);
  }

  /**
   * called when this tool is "mounted" on a Graphic context.
   * It switches the Graphic context and mount the default IM, if no one
   * is currently active.
   */ 
  public void reActivate(GraphicContext theGc)
  {
    gc = theGc;
    mountIModule(getDefaultIM());
  }
  
  /**
   * override to return the default (initial) Interaction module
   */
  abstract public InteractionModule getDefaultIM();

  /**
   * called when this object is unmounted */
  public void deactivate() 
  {
  }

  /**
   * the method used to mount another interaction module in a tool
   */
  static public void mountIModule(InteractionModule im) 
  {
    currentInteractionModule = im;
    currentInteractionModule.takeInteraction(gc);
  }


  /**
   * the method used to mount an interaction module in a tool
   * AND specify an initial point for the interaction.
   * This method is used when the IM is mounted when the interaction
   * is already begun.
   */
  static public void mountIModule(InteractionModule im, int x, int y) 
  {
    mountIModule(im);
    im.interactionBeginAt(x, y);

  }

  /**  
   * returns its name
   */
  public String getName() 
  {
    return itsName;
  }

  /**
   * sets its name
   */
  public void setName(String theName) 
  {
    itsName = theName;
  }


  /**
   * sets its icon
   */
  public void setIcon(ImageIcon theIcon) 
  {
    itsIcon = theIcon;
  }

  /**
   * gets the icon
   */
  public ImageIcon getIcon() 
  {
    return itsIcon;
  }

  public JPopupMenu getMenu()
  {
    return null;
  }

  public Cursor getCursor()
  {
    return cursor;
  }

  public void setCursor( ImageIcon icon)
  {
    this.cursor = loadCursor( icon, null);
  }

  public void setCursor( ImageIcon icon, Point hotSpot)
  {
    this.cursor = loadCursor( icon, hotSpot);
  }

  public Cursor loadCursor( ImageIcon cursorIcon, Point hotSpot)
  {
    if (cursorIcon == null)
      return null;

    Image image = cursorIcon.getImage();
    ImageObserver observer = new ImageObserver() {
	public boolean imageUpdate( Image img, int infoflags, int x, int y, int width, int height)
	{
	  return false;
	}
      };
    int imageWidth = image.getWidth(observer);
    int imageHeight = image.getHeight(observer);

    Dimension bestSize = Toolkit.getDefaultToolkit().getBestCursorSize( imageWidth, imageHeight);
    BufferedImage bi = new BufferedImage( bestSize.width, bestSize.height, BufferedImage.TYPE_INT_ARGB);
    bi.createGraphics().drawImage( image, 0, 0, observer);
    
    Point hs = hotSpot;
    if( hs == null)
      if ( bi.getHeight( observer) < 1)
	hs = new Point(0,0);
      else
	hs = new Point(0, 1);
    
    return Toolkit.getDefaultToolkit().createCustomCursor( bi, hs, itsName + " cursor");
  }

  //---- Fields
  String itsName;
  ImageIcon itsIcon;
  protected static GraphicContext gc;
  static InteractionModule currentInteractionModule;
  Cursor cursor;
}




