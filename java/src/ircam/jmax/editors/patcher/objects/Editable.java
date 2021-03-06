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

package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;

import ircam.jmax.fts.*;
import ircam.fts.client.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.menus.*;
import ircam.jmax.editors.patcher.interactions.*;

//
// The base class of the ermes objects which are user-editable
// (Message, Standard, Patcher).
//

abstract public class Editable extends GraphicObject implements FtsInletsListener, FtsOutletsListener
{
  boolean editing = false;
  int defaultWidth = -1;
  public transient ircam.jmax.editors.patcher.ObjectRenderer renderer; // don't ask me why here we need the whole path

  public Editable(FtsGraphicObject theFtsObject) 
  {
    super(theFtsObject);
    computeRenderer();
    renderer.update();
  }

  public void setDefaults()
  {
    setWidth(getDefaultWidth());
  }

  public void updateDimensions()
  {
    renderer.update();
    super.setHeight(renderer.getHeight() + getTextHeightOffset());
    super.setWidth(renderer.getWidth() + getTextWidthOffset());
  }

  public void updateDimensionsNoConnections()
  {
    renderer.update();
    super.setHeightNoConnections(renderer.getHeight() + getTextHeightOffset());
  }

  protected void computeRenderer()
  {
    Renderer r;
    renderer = new TextRenderer(this);
  }

  /* it's still there only to redefine Patcher object */
  /* when will be asynchronous remove them and use "redefined" */
  public void redefine(String text) 
  {
    computeRenderer();
    
    updateDimensions();
    
    if(itsSketchPad.isAutomaticFitToText()){
      redraw();
      fitToText();
    }
    else
      redraw();
    
    super.redefine(text);    
  }

  public int getMinimumWidth(){
    if(renderer instanceof TextRenderer)
      return ((TextRenderer)renderer).getColWidth() + getTextWidthOffset();
    else 
      return renderer.getWidth();
  }

  public int getDefaultWidth()
  {
    if(defaultWidth==-1)
      defaultWidth = getFontMetrics().stringWidth( "pack 1 2 3") + 2*getTextXOffset();
      
      return defaultWidth;      
  }

  public void setDefaultWidth(int w)
  {
    defaultWidth = w;
  }

  // redefined from base class
  public  void setWidth(int w) 
  {
    if( w <= 0)
      super.setWidth( getMinimumWidth() + getVariableWidth());
    else
      if( renderer.canResizeWidthTo(w - getVariableWidth() - getTextWidthOffset()))
	super.setWidth( w);
      else
	super.setWidth( getMinimumWidth() + getVariableWidth());
 
    int height = renderer.getHeight() + getTextHeightOffset();
    if( getHeight() != height) 
      super.setHeight( height);	
  }

  public void forceWidth( int w)
  {
    super.setWidth( w);
  }

  // redefined from base class, only when not editing
  // This way, changing the height from outside is forbidden

  public void setHeight( int v)
  {
    if (editing)
      super.setHeight(v);
  }

  public void forceHeight( int v)
  {
    super.setHeight(v);
  }

  // redefined from base class

  public void setFont( Font f)
  {
    super.setFont( f);
    renderer.update();
    super.setHeight(renderer.getHeight() + getTextHeightOffset());
    redraw();
  }

  public void forceFont( Font f)
  {
    super.setFont( f);
    renderer.update();
  }

  public void setCurrentFont(Font font)
  {
    super.setCurrentFont(font);
    renderer.update();
  }
  public void fitToText()
  {
    renderer.update();
    int w = ((TextRenderer)renderer).getTextWidth() + getVariableWidth() + getTextWidthOffset() + 2;

    if(w < getMinimumWidth())
      super.setWidth( getMinimumWidth() + getVariableWidth());
    else
      super.setWidth( w);

    if(!((TextRenderer)renderer).isMultiLine())
      super.setHeight(((TextRenderer)renderer).getRHeight() + getTextHeightOffset());
    else
      super.setHeight(((TextRenderer)renderer).getTextHeight() + getTextHeightOffset());

    redraw();
  }

  public void setWidthToText(String text)
  {
    int tLength = SwingUtilities.computeStringWidth(getFontMetrics(), text)+ getTextWidthOffset() + getVariableWidth() + 2; 	
    
    if(tLength < getDefaultWidth()) tLength = getDefaultWidth();

    super.setWidth(tLength);
  }

  // ----------------------------------------
  // ``Args'' property
  // ----------------------------------------

  abstract public String getArgs();

  // ----------------------------------------
  // ``TextBackground'' property
  // ----------------------------------------

  abstract public Color getTextBackground();
  abstract public Color getTextForeground();

  // Properties to position correctly the text editor

  abstract public int getTextXOffset();
  abstract public int getTextYOffset();
  abstract public int getTextWidthOffset();
  abstract public int getTextHeightOffset();

  abstract public boolean isMultiline();
  
  public void setEditing(boolean v)
  {
    editing = v;
  }
  public boolean isEditing()
  {
    return editing;
  }

   // ----------------------------------------
  // Property handling
  // ----------------------------------------

  public void inletsChanged(int n)
  {
    updateInOutlets();
    if(itsSketchPad != null)
    {
      redraw();
      redrawConnections();
      itsSketchPad.getDisplayList().updateConnectionsFor(this);
    }
  }
  public void outletsChanged(int n)
  {
    updateInOutlets();
    if(itsSketchPad != null)
    {
      redraw();
      redrawConnections();
      itsSketchPad.getDisplayList().updateConnectionsFor(this);
    }
  }
  public void drawContent(Graphics g) 
  {
    if (editing)
      return;

    renderer.setBackground(getTextBackground());
    renderer.setForeground(getTextForeground());

    renderer.render(g, 
                    getX() + getTextXOffset(),
                    getY() + getTextYOffset(),
                    getWidth() - getVariableWidth() - getTextWidthOffset(),
                    getHeight() - getTextHeightOffset());
  }

  // Text Sensibility area 

  protected SensibilityArea findSensibilityArea( int mouseX, int mouseY)
  {
    int dx = mouseX - (getX() + getTextXOffset());
    int dy = mouseY - (getY() + getTextYOffset());

    if ((dx >= 0) && (dx < getWidth() - getVariableWidth() - getTextWidthOffset()) &&
	(dy >= 0) && (dy < getHeight() - getTextHeightOffset()))
      {
	return SensibilityArea.get(this, Squeack.TEXT);
      }
    else
      return super.findSensibilityArea( mouseX, mouseY);
  }

  // Edit

  public void edit(Point point)
  {
    itsSketchPad.textEditObject(this, point);
  }

  JSeparator mySeparator = new JSeparator();
  public void popUpUpdate(boolean onInlet, boolean onOutlet, SensibilityArea area)
  {
    super.popUpUpdate(onInlet, onOutlet, area);
    ObjectPopUp.getInstance().add( mySeparator);
    TextPopUpMenu.update(this);
    ObjectPopUp.addMenu(TextPopUpMenu.getInstance());
  }
  public void popUpReset()
  {
    super.popUpReset();
    ObjectPopUp.removeMenu(TextPopUpMenu.getInstance());
    ObjectPopUp.getInstance().remove( mySeparator);
  }

  /************** Undo/Redo *******************/
  public void undo()
  {
    super.undo();
    forceHeight( uh);
  }
  public void redo()
  {
    super.redo();
    forceHeight( rh);
  }
}

