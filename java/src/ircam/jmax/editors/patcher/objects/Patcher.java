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

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.utils.*;
import ircam.jmax.editors.patcher.*;

//
// The "patcher" graphic object.
//

class Patcher extends Editable implements FtsObjectErrorListener
{
  // ----------------------------------------
  // Constructor
  // ----------------------------------------

  Patcher( ErmesSketchPad theSketchPad, FtsObject theFtsObject)
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

  boolean errorsInside = false;
  public void errorChanged(boolean value) 
  {
    errorsInside = value;
    redraw();
  }

  public void redefine( String text) 
  {
    ( (FtsPatcherObject)ftsObject).redefinePatcher( text);
    
    super.redefine(text);
  }
	

  public void editContent()
  {
    itsSketchPad.waiting();

    ftsObject.getFts().editPropertyValue(ftsObject, new MaxDataEditorReadyListener() {
      public void editorReady(MaxDataEditor editor)
	{itsSketchPad.stopWaiting();}
    });
  }

  public boolean hasContent()
  {
    return true;
  }


  // ----------------------------------------
  // Text area offset
  // ----------------------------------------
  private static final int TEXT_X_OFFSET = 4;
  private static final int TEXT_Y_OFFSET = 3;
  
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
    return 7;
  }

  public int getTextHeightOffset()
  {
    return 7;
  }

  public Color getTextForeground()
  {
      return Color.black;
  }

  public Color getTextBackground()
  {
    if (errorsInside)
	if (isSelected()) 
	    return Color.pink.darker();
	else
	    return Color.pink;
    else
	if (isSelected()) 
	    return Settings.sharedInstance().getObjColor().darker();
	else 
	    return Settings.sharedInstance().getObjColor();
  }

  // ----------------------------------------
  // Inspector
  // ----------------------------------------

  public void inspect() 
  {
  }

  //--------------------------------------------------------
  // Paint stuff
  //--------------------------------------------------------

  public void paint( Graphics g) 
  {
      if (errorsInside)
	  if (isSelected())
	      g.setColor( Color.pink.darker());
	  else
	      g.setColor( Color.pink);
      else
	  if (isSelected())
	      g.setColor( Settings.sharedInstance().getObjColor().darker());
	  else 
	      g.setColor( Settings.sharedInstance().getObjColor());
      
      g.fill3DRect( getX() + 1, getY() + 1, getWidth() - 2, getHeight() - 2, true);
      g.draw3DRect( getX() + 2, getY() + 2, getWidth() - 5, getHeight() - 5, false);
      
      drawContent( g);
      
      super.paint( g);
  }
}
