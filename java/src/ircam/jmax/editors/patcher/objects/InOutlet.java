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

//
// The abstract base class graphic in/outlet contained in subpatchers
//

abstract public class InOutlet extends GraphicObject {

  protected int itsId;
  private static final int DEFAULT_WIDTH = 20;
  private static final int MINIMUM_WIDTH = 10;

  InOutlet( ErmesSketchPad theSketchPad, FtsObject theFtsObject, int id) 
  {
    super(theSketchPad, theFtsObject);

    itsId = id;

    int width = getWidth();
    if (width == -1)
      setWidth( DEFAULT_WIDTH);
    else if (width <= MINIMUM_WIDTH)
      setWidth( width);
  }

  // redefined from base class

  public void setWidth( int theWidth)
  {
    if (theWidth < MINIMUM_WIDTH)
      theWidth = MINIMUM_WIDTH;

    super.setWidth( theWidth);
    super.setHeight( theWidth);
  }

  // redefined from base class
  public void setHeight( int theHeight)
  {
  }

  public void changeNo( int n) 
  {
    if (itsId != n)
      {
	itsId = n;
	((FtsInOutletObject) ftsObject).setPosition(n);
	updateInOutlets();
      }

    redraw();
  }

  public Dimension getMinimumSize() 
  {
    return null;
  }
}

