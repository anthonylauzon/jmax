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

import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

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
  }

  // ----------------------------------------
  // ``Args'' property
  // ----------------------------------------

  public String getArgs()
  {
    // get the correct String from the object
    return ftsObject.getComment();
  }

  public void redefine( String text) 
  {
    ((FtsCommentObject)ftsObject).setComment( text);

    super.redefine(text);
  }

  public void computeRenderer()
  {
    String args = getArgs();
    Icon icon = null;

    // Change the renderer if needed

    if ((args != null) && (args.length() > 0)  && args.charAt(0) == '%')
      icon = Icons.get(args);

    if (icon != null)
      renderer = new IconRenderer(this, icon);
    else if (! (renderer instanceof TextRenderer))
      renderer = new TextRenderer(this);
  }

  // ----------------------------------------
  // Text area offset
  // ----------------------------------------
  public static final int TEXT_X_OFFSET = 2;
  public static final int TEXT_Y_OFFSET = 2;

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
    return 4;
  }

  public Color getTextForeground()
  {
      return Color.black;
  }

  public Color getTextBackground()
  {
    if (itsSketchPad.isLocked())
      return Color.white;
    else
      {
	if (isSelected())
	  return Color.gray;
	else
	  return itsSketchPad.getBackground();
      }
  }

  public boolean isMultiline()
  {
    return true;
  }

  public void paint(Graphics g) 
  {
    if ( !itsSketchPad.isLocked())
      {
	if (isSelected())
	  g.setColor(Color.gray);
	else
	  g.setColor(itsSketchPad.getBackground());

	g.fill3DRect( getX(), getY(), getWidth(), getHeight(), true); 
      }

    //text
   drawContent( g);
  }

  //--------------------------------------------------------
  // minimumSize()
  //--------------------------------------------------------
  public Dimension getMinimumSize() 
  {
    return null;
  }
}

