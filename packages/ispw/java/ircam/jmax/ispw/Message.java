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

package ircam.jmax.ispw;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.interactions.*;

//
// The "message box" graphic object.
//

public class Message extends Editable implements FtsMessageListener, FtsIntValueListener
{
  boolean isFlashing = false;

  public Message( FtsGraphicObject theFtsObject)
  {
    super( theFtsObject);
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
    return 8;
  }

  public int getTextHeightOffset()
  {
    return 4;
  }

  public boolean isMultiline()
  {
    return true;
  }

  public void redefine( String text) 
  {
    ((FtsMessageObject)ftsObject).setMessage( text);
  }

  public void redefined()
  {
    messageChanged( ((FtsMessageObject)ftsObject).getMessage());
  }

  // Set the text when FTS change the message content

  public void messageChanged(String message)
  {
    if(isEditing())
      itsSketchPad.abortTextEditing();

    redraw();
    redrawConnections();

    //super.redefine( message);
    computeRenderer();
    renderer.update();

    redraw();
    redrawConnections();
  }

  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {          
    // Send a bang message to the system inlet
    if (Squeack.isDown(squeack))
	((FtsMessageObject)ftsObject).sendBang();
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

  public Color getTextForeground()
  {
      return Color.black;
  }

  public Color getTextBackground()
  {
    if( isFlashing) 
      return Settings.sharedInstance().getUIColor();
    else
      {
	if (isSelected()) 
	  return Settings.sharedInstance().getUIColor().darker();
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

  public void updatePaint(Graphics g) 
  {
    g.setColor(getTextBackground());
    g.fillRect( getX()+1, getY()+1, getWidth()-2, getHeight()-2);
    
    drawContent( g);
  }
}







