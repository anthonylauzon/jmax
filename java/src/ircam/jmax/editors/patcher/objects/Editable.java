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
import java.util.*;

import javax.swing.*;

import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.interactions.*;

//
// The base class of the ermes objects which are user-editable
// (Message, Standard, Patcher).
//

abstract public class Editable extends GraphicObject implements FtsInletsListener, FtsOutletsListener
{
  boolean editing = false;
  ircam.jmax.editors.patcher.ObjectRenderer renderer; // don't ask me why here we need the whole path

  Editable( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super( theSketchPad, theFtsObject);

    computeRenderer();

    if ((renderer instanceof TextRenderer) && (getWidth() == -1))
      {
	renderer.update();
	setWidth( getFontMetrics().stringWidth( "pack 1 2 3") + 2*getTextXOffset());
      }
    else
      updateDimensions();
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

  // By default, get an image renderer if there is an icon named as the class name,
  // otherwise text.

  protected void computeRenderer()
  {
    Renderer r;
    // Change the renderer if needed

    Icon icon = Icons.get(ftsObject.getClassName());

    if (icon != null)
      renderer = new IconRenderer(this, icon);
    else if (! (renderer instanceof TextRenderer))
      renderer = new TextRenderer(this);
  }


  public void redefine( String text) 
  {
    computeRenderer();

    updateDimensions();

    redraw();

    super.redefine(text);
  }


  // redefined from base class

  public  void setWidth(int w) 
  {
    // renderer.update();
    if (renderer.canResizeWidthTo(w - getTextWidthOffset()))
      {
	super.setWidth(w);
	super.setHeight(renderer.getHeight() + getTextHeightOffset());
      }
  }

  // redefined from base class, only when not editing
  // This way, changing the height from outside is forbidden

  public void setHeight( int v)
  {
    if (editing)
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

  public void fitToText()
  {
    renderer.update();
    super.setWidth(((TextRenderer)renderer).getTextWidth() + getTextWidthOffset());
    if(!((TextRenderer)renderer).isMultiLine())
      super.setHeight(renderer.getHeight() + getTextHeightOffset());
    else
      super.setHeight(((TextRenderer)renderer).getTextHeight() + getTextHeightOffset());
    redraw();
  }

  // ----------------------------------------
  // ``Args'' property
  // ----------------------------------------

  abstract public String getArgs();

  // ----------------------------------------
  // ``TextBackground'' property
  // ----------------------------------------

  abstract public Color getTextBackground();

  // Properties to position correctly the text editor

  abstract public int getTextXOffset();
  abstract public int getTextYOffset();
  abstract public int getTextWidthOffset();
  abstract public int getTextHeightOffset();

  public void setEditing(boolean v)
  {
    editing = v;
  }

  // ----------------------------------------
  // Property handling
  // ----------------------------------------

  public void inletsChanged(int n)
  {
    updateInOutlets();
    redraw();
    redrawConnections();
    itsSketchPad.getDisplayList().updateConnectionsFor(this);
  }

  public void outletsChanged(int n)
  {
    updateInOutlets();
    redraw();
    redrawConnections();
    itsSketchPad.getDisplayList().updateConnectionsFor(this);
  }


  public void drawContent(Graphics g) 
  {
    if (editing)
      return;

    renderer.setBackground(getTextBackground());

    renderer.render(g, 
		    getX() + getTextXOffset(),
		    getY() + getTextYOffset(),
		    getWidth() - getTextWidthOffset(),
		    getHeight() - getTextHeightOffset());
  }

  // Text Sensibility area 

  protected SensibilityArea findSensibilityArea( int mouseX, int mouseY)
  {
    int dx = mouseX - (getX() + getTextXOffset());
    int dy = mouseY - (getY() + getTextYOffset());

    if ((dx >= 0) && (dx < getWidth() - getTextWidthOffset()) &&
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
}










