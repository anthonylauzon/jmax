//
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.editors.patcher.*;

//
// The generic "extern" object in ermes. (example: adc1~)
//
public class Standard extends Editable implements FtsObjectErrorListener
{
  boolean ignoreError = false;

  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------
  Standard( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super( theSketchPad, theFtsObject);
  }

  // ----------------------------------------
  // ``Args'' property
  // ----------------------------------------
  public String getArgs()
  {
    // get the correct String from the object
    return ftsObject.getDescription();
  }

  public void errorChanged(boolean value) 
  {
    ignoreError = false;
    redraw();
  }

  /* Inspector */

  public void inspect() 
  {
  }

  public void redefine( String text) 
  {
    try 
      {
	ignoreError = false;
	ftsObject = ftsObject.getFts().redefineFtsObject( ftsObject, text);

	if (ftsObject.isError())
	  {
	    itsSketchPad.showMessage(ftsObject.getErrorDescription());
	  }
      } 
    catch (FtsException e) 
      {
	System.out.println("Error in redefining object, action cancelled");
      }

    super.redefine(text);
  }


  public void editContent()
  {
    itsSketchPad.waiting();
    ftsObject.getFts().editPropertyValue( ftsObject, new MaxDataEditorReadyListener() {
      public void editorReady(MaxDataEditor editor)
	{itsSketchPad.stopWaiting();}
    });
  }

  public boolean hasContent()
  {
    return true;
  }

  public void setIgnoreError(boolean v)
  {
    ignoreError = v;
  }

  // ----------------------------------------
  // Text area offset
  // ----------------------------------------

  private static final int TEXT_X_OFFSET = 4;
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
    return 6;
  }

  public int getTextHeightOffset()
  {
    return 5;
  }

  public Color getTextBackground()
  {
    if (isSelected())
      return Settings.sharedInstance().getSelectedColor();
    else
      return Color.white;
  }

  // ----------------------------------------
  // Paint stuff
  // ----------------------------------------

  public void paint(Graphics g) 
  {
    if ((! ignoreError) && ftsObject.isError())
      g.setColor( Color.red);
    else
      {
	if (isSelected())
	  g.setColor( Settings.sharedInstance().getSelectedColor());
	else
	  g.setColor( Settings.sharedInstance().getObjColor());
      } 

    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    g.fill3DRect( x+1, y+1, w-2, h-2, true);

    drawContent( g);

    super.paint( g);
  }
}



