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

package ircam.jmax.editors.table;

import ircam.jmax.toolkit.*;
import ircam.jmax.utils.*;

/**
 * A simple coordinates converter in a Table window. 
 * It handles the conversion of x, y coordinates, an origin, and two zoom factors.
 */
public class TableAdapter {


  public TableAdapter()
  {
  }

  /**
   * add a listener for the xZoom factor */
  public void addXZoomListener(ZoomListener zl)
  {
    xZoomListeners.addElement(zl);
  }

  /**
   * add a listener for the yZoom factor */
  public void addYZoomListener(ZoomListener zl)
  {
    yZoomListeners.addElement(zl);
  }


  /**
   * remove a listener for the xZoom factor */
  public void removeXZoomListener(ZoomListener zl)
  {
    xZoomListeners.removeElement(zl);
  }

  /**
   * remove a listener for the yZoom factor */
  public void removeYZoomListener(ZoomListener zl)
  {
    yZoomListeners.removeElement(zl);
  }

  /**
   * returns the graphic x corresponding to the index */
  public int getX(int index)
  {
    return (int) ((index-oX) * itsXZoom);
  }

  /**
   * returns the index in the table corresponding to the graphic x */
  public int getInvX(int x)
  {
    return (int) (x/itsXZoom+oX);
  }

  /**
   * returns the graphic y corresponding to the value.
   * The y coordinates are implicitily inverted. */
  public int getY(int value)
  {
    if (value < 0);

    return (int) (oY-value * itsYZoom);
  }

  /**
   * returns the value corresponding to the graphic y */ 
  public int getInvY(int y)
  {
    return (int) ((oY-y)/itsYZoom);
  }

  /**
   * A method that allows setting the X zoom factor under the form
   * of a rational number */
  public void setXZoomFraction(int numerator, int denominator)
  {
    setXZoom(((float)numerator)/denominator);
  }

  /**
   * A method that allows setting the Y zoom factor under the form
   * of a rational number. Use this to avoid graphical undersampling. */
  public void setYZoomFraction(int numerator, int denominator)
  {
    setYZoom(((float)numerator)/denominator);
  }

  private void notifyZoomChanged(int whichOne, float value)
  {
    MaxVector listeners;
    if (whichOne == X_ZOOM) listeners = xZoomListeners; 
    else if (whichOne == Y_ZOOM) listeners = yZoomListeners; 
    else return; //can add other zooms here

    ZoomListener zl;
    for (int i = 0; i< listeners.size(); i++)
      {
	zl = (ZoomListener) listeners.elementAt(i);
	zl.zoomChanged(value);
      }
  }

  //--- Fields & accessors ---//
  float itsXZoom;
  float itsYZoom;

  MaxVector xZoomListeners = new MaxVector();
  MaxVector yZoomListeners = new MaxVector();

  public static int X_ZOOM = 0;
  public static int Y_ZOOM = 1;

  /** the first index in the table we're showing */
  int oX; 
  /** the graphic x offset of oX */
  int xOffset;
  /** the first value in the table we're vertically showing */
  int oY;
  /** the graphic y offset of oY */
  int yOffset;

  public float getXZoom()
  {
    return itsXZoom;
  }

  public void setXZoom(float zoom)
  {
    itsXZoom = zoom;
    notifyZoomChanged(X_ZOOM, zoom);
  }

  public float getYZoom()
  {
    return itsYZoom;
  }

  public void setYZoom(float zoom)
  {
    itsYZoom = zoom;
    notifyZoomChanged(Y_ZOOM, zoom);
  }

  public int getOY()
  {
    return oY;
  }

  public void setOY(int theOY)
  {
    oY = theOY;
  }

  public int getOX()
  {
    return oY;
  }

  public void setOX(int theOY)
  {
    oY = theOY;
  }

  public int getYOffset()
  {
    return yOffset;
  }

  public void setYOffset(int theOY)
  {
    yOffset = theOY;
  }

  public int getXOffset()
  {
    return xOffset;
  }

  public void setXOffset(int theOX)
  {
    xOffset = theOX;
  }

}



