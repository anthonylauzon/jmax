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
import ircam.jmax.utils.*;
import ircam.jmax.editors.patcher.*;

//
// The graphic inlet contained in subpatchers
//

public class Inlet extends InOutlet {

  public Inlet(ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super(theSketchPad, theFtsObject, ((FtsInletObject) theFtsObject).getPosition());
  }

  public void paint(Graphics g) 
  {
    if (isSelected())
      g.setColor( Settings.sharedInstance().getSelectedColor());
    else
      g.setColor( Settings.sharedInstance().getObjColor());

    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    g.fill3DRect( x + 1, y + 1, w - 2,  h - 2, true);

    //the triangle
    Color color = g.getColor();

//    g.setColor( color.brighter());
//     int xPoints[] = { x + 1, x + w/2, x + w - 2};
//     int yPoints[] = { y + 1, y + h/2 - 1, y + 1};
//     g.fillPolygon( xPoints, yPoints, 3);
//
//      g.setColor( color.darker());
//      g.drawLine( x + 2, y + 2, x + w/2, y + h/2);

    int xpwd2 = x + w/2;
    int S = 4;

    g.setColor( color.brighter());
    g.drawLine( xpwd2, y + 2 + S, xpwd2 + S, y + 2);

    g.setColor( color.darker());
    g.drawLine( xpwd2 - S - 1, y + 2, xpwd2 - 1, y + 2 + S);

    int ys = y + h - getFontMetrics().getDescent() - 1;
    String s = "" + itsId;
    int xs = xpwd2 - getFontMetrics().stringWidth( s)/2;

    g.setFont( getFont());
    g.setColor( Color.black);
    g.drawString( s, xs, ys);

    super.paint( g);
  }

  public void popUpEdit(Point p)
  {
    ircam.jmax.utils.ChooseNumberPopUp.choose(itsSketchPad,
			     new NumberChoosenListener()
			     {
			       public void numberChoosen(int v) { changeNo(v);}
			     },
			       0,
			       itsSketchPad.getFtsPatcher().getNumberOfInlets() + 4,
			       p);
  }
}
