//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;

//
// The "out" graphic object used in subpatchers.
//

class ErmesObjOut extends ErmesObjInOut {

  public ErmesObjOut(ErmesSketchPad theSketchPad, FtsObject theFtsObject)
  {
    super(theSketchPad, theFtsObject, ((FtsOutletObject) theFtsObject).getPosition());
  }

  void redefine( String text)
  {
    ((FtsOutletObject)itsFtsObject).setPosition( itsId);
  }
  
  public void Paint_specific(Graphics g) 
  {
    if ( !itsSelected)
      g.setColor( Settings.sharedInstance().getObjColor());
    else 
      g.setColor( Settings.sharedInstance().getSelectedColor());

    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    g.fill3DRect( x + 1, y + 1, w - 2,  h - 2, true);

    //the triangle
    Color color = g.getColor();

    int xpwd2 = x + w/2;
    int S = 4;

    g.setColor( color.brighter());
    g.drawLine( xpwd2, y + h - 3 - S, xpwd2 + S, y + h - 3);

    g.setColor( color.darker());
    g.drawLine( xpwd2 - 1 - S, y + h - 3, xpwd2 - 1, y + h - 3 - S);

    int ys = y + itsFontMetrics.getAscent() + 1;
    String s = "" + itsId;
    int xs = xpwd2 - itsFontMetrics.stringWidth( s)/2;

    g.setFont( getFont());
    g.setColor( Color.black);
    g.drawString( s, xs, ys);

    super.Paint_specific( g);
  }

  public void MouseDown_specific(MouseEvent evt, int x, int y) 
  {
    if ( itsSketchPad.isLocked() || evt.getClickCount() == 1) 
      {
	itsSketchPad.ClickOnObject( this, evt, x, y);
      }
    else 
      {	//we want to choose among the different Outlet number
	itsSketchPad.itsOutPop.SetNewOwner( this); //make the Choice pointing to this
	itsSketchPad.itsOutPop.show( itsSketchPad, getX(), getY());
      }
  }
}
