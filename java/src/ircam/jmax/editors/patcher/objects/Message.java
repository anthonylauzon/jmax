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

import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.interactions.*;

//
// The "message box" graphic object.
//

class Message extends Editable implements FtsMessageListener, FtsIntValueListener
{
  boolean isFlashing = false;

  public Message(ErmesSketchPad theSketchPad, FtsObject theFtsObject)
  {
    super(theSketchPad, theFtsObject);
  }

  // ----------------------------------------
  // ``Args'' property
  // ----------------------------------------

  public String getArgs()
  {
    // get the correct String from the object's "Message" property, that may change

    return ((FtsMessageObject)ftsObject).getMessage();
  }


  // ----------------------------------------
  // Text area offset
  // ----------------------------------------
  private static final int TEXT_X_OFFSET = 3;
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
    return 4;
  }

  public void redefine( String text) 
  {
    ((FtsMessageObject)ftsObject).setMessage( text);

    super.redefine(text);
  }

  // Set the text when FTS change the message content

  public void messageChanged(String message)
  {
    updateRedraw();
  }

  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {
    // Send a bang message to the system inlet
 
    if (Squeack.isDown(squeack))
      ftsObject.sendMessage( -1, "bang", null);
  }

  public void valueChanged(int value) 
  {
    if (value != 0)
      isFlashing = true;
    else
      isFlashing = false;

    updateRedraw();
  }

  // ----------------------------------------
  // ``TextBackground'' property
  // ----------------------------------------

  public Color getTextBackground()
  {
    if (! itsSketchPad.isLocked()) 
      {
	if( isFlashing) 
	  return Settings.sharedInstance().getSelectedColor();
	else
	  {
	    if (isSelected()) 
	      return Settings.sharedInstance().getSelectedColor();
	    else
	      return Color.white;
	  }
      }
    else 
      {
	if ( isFlashing || isSelected())
	  return Settings.sharedInstance().getSelectedColor();
	else 
	  return Color.white;
      }
  }

  public void paint(Graphics g) 
  {
    g.setColor(getTextBackground());
    g.fillRect( getX()+1, getY()+1, getWidth()-2, getHeight()-2);
    
    drawContent( g);

    super.paint( g);
  }
}
