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

import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.interactions.*;

//
// The "comment" graphic object
//

public class Comment extends Editable
{
  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------

  public Comment(FtsGraphicObject theFtsObject) 
  {
    super(theFtsObject);
    setDefaultColors();
  }

  public void setDefaults()
  {    
    setWidth(  DEFAULT_WIDTH);
    setHeight( renderer.getHeight() + getTextHeightOffset());
  }

  void setDefaultColors()
  {
    color = DEFAULT_COLOR;
    colorId = DEFAULT_COLOR.getRGB();
    alpha = 0;
    createAllColors( color);
  }

  public void setHeight( int h)
  {
    if( !isEditing())
      super.forceHeight( h);
    else
      if( getHeight() < h)
	super.forceHeight( h);
  }

  public void setWidth( int w)
  {
    super.forceWidth( w);
  }

  // ----------------------------------------
  // ``Args'' property
  // ----------------------------------------

  public String getArgs()
  {
    return ftsObject.getComment();
  }

  public void redefine( String text) 
  {
    ((FtsCommentObject)ftsObject).setComment( text);

    super.redefine(text);
  }

  public void computeRenderer()
  {
    renderer = new TextRenderer(this);
  }

  public void setFont( Font f)
  {
    super.forceFont( f);
   
    int h = renderer.getHeight() + getTextHeightOffset();
    if( getHeight() < h)
    setHeight( h);
    redraw();
  }
  
  public void updateDimensions()
  {
    renderer.update();
  }

  public void updateDimensionsNoConnections()
  {
    renderer.update();
  }
  // ----------------------------------------
  // Text area offset
  // ----------------------------------------
  public static final int TEXT_X_OFFSET = 1;
  public static final int TEXT_Y_OFFSET = 1;

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
    return 2;
  }

  public int getTextHeightOffset()
  {
    return 2;
  }

  public Color getTextForeground()
  {
      return Color.black;
  }

  public Color getTextBackground()
  {
    if( isEditing())
      if( isSelected())
	return selectedOpaqueColor;
      else
	return opaqueColor;
    else
      if( itsSketchPad.isLocked())
	if( isSelected())
	  return selectedColor;
	else
	  return color;
      else
	if( isSelected())
	  return editSelectedColor;
	else
	  return editColor;
  }

  public boolean isMultiline()
  {
    return true;
  }
  
  /***************** Background Color *********************************/
  public void setColor( Color c)
  {
    color = new Color( c.getRed(), c.getGreen(), c.getBlue(), alpha);
    colorId = color.getRGB();
    createAllColors( color);
	
    ((FtsCommentObject)ftsObject).setColor( colorId);
    renderer.update();
    redraw();
  }

  public void setTransparency( int alpha)
  {
    if( this.alpha != alpha)
      {
	this.alpha = alpha;
	this.color = new Color( color.getRed(), color.getGreen(), color.getBlue(), alpha);
	colorId = color.getRGB();
	createAllColors( color);

	((FtsCommentObject)ftsObject).setColor( colorId);
	renderer.update();
	redraw();
      }
  }

  public int getTransparency()
  {
    return alpha;
  }

  public void setCurrentColor( int colorId)
  {
    if(colorId != this.colorId)
      {
	this.colorId = colorId;
	this.color = new Color(colorId, true);
	alpha = color.getAlpha();
	createAllColors( color);

	renderer.update();
	redraw();
      }
  }

  public int getColorRGB()
  {
    return colorId;
  }

  public Color getColor()
  {
    return color;
  }

  void createAllColors( Color color)
  {
    Color darker = color.darker();
    selectedColor = new Color( darker.getRed(), darker.getGreen(), darker.getBlue(), color.getAlpha());
    selectedOpaqueColor = darker;
    opaqueColor = new Color( color.getRed(), color.getGreen(), color.getBlue());
    if( color.getAlpha() < 10)
      {
	editColor = new Color( color.getRed(), color.getGreen(), color.getBlue(), 14);
	editSelectedColor = new Color( darker.getRed(), darker.getGreen(), darker.getBlue(), 16);
      }    
    else
      {
	editColor = color;
	editSelectedColor = selectedColor;
      }
  }

  public void paint(Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();
    
    g.setColor( getTextBackground());    
    g.fillRect( x, y, w-1, h-1);
    
    g.setColor( selectedColor);
    if( !itsSketchPad.isLocked())
      g.draw3DRect( x, y, w-1, h-1, true);
    else
      g.drawRect( x, y, w-1, h-1);
    
    //text
    drawContent( g);
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
  
  public ObjectControlPanel getControlPanel()
  {
    return this.controlPanel;
  }

  /**************  popup interaction ********************/ 
  public void popUpUpdate(boolean onInlet, boolean onOutlet, SensibilityArea area)
  {
    super.popUpUpdate(onInlet, onOutlet, area);
    getControlPanel().update(this);
    ObjectPopUp.getInstance().add((JPanel)getControlPanel());
    ObjectPopUp.getInstance().revalidate();
    ObjectPopUp.getInstance().pack();
  }
  public void popUpReset()
  {
    super.popUpReset();
    ObjectPopUp.getInstance().remove((JPanel)getControlPanel());
  }

  private transient Color color, selectedColor, selectedOpaqueColor, opaqueColor, editColor, editSelectedColor;
  int colorId, alpha;  
  public static CommentControlPanel controlPanel = new CommentControlPanel();
  private static Color DEFAULT_COLOR = new Color( 255, 255, 255, 0);
  static final int DEFAULT_WIDTH = 130;
}

