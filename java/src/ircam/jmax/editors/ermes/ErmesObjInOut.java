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
// The abstract base class graphic in/outlet contained in subpatchers
//
abstract class ErmesObjInOut extends ErmesObject {

  protected int itsId;
  private static final int DEFAULT_WIDTH = 20;
  private static final int MINIMUM_WIDTH = 10;

  ErmesObjInOut( ErmesSketchPad theSketchPad, FtsObject theFtsObject, int id) 
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
  protected void setWidth( int theWidth)
  {
    if (theWidth < MINIMUM_WIDTH)
      theWidth = MINIMUM_WIDTH;

    super.setWidth( theWidth);
    super.setHeight( theWidth);
  }

  // redefined from base class
  protected void setHeight( int theHeight)
  {
  }

  void ChangeNo( int n) 
  {
    if (itsId != n) 
      {
	itsId = n;
	redefine( "");
      }

    itsSketchPad.repaint();
  }

  public Dimension getMinimumSize() 
  {
    return null;
  }
}

