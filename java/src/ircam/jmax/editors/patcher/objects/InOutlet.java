//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
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

