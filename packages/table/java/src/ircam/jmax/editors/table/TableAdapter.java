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

package ircam.jmax.editors.table;

import ircam.jmax.toolkit.*;
import ircam.jmax.utils.*;
import java.awt.*;

/**
 * A simple coordinates converter in a Table window. 
 * It handles the conversion of x, y coordinates, an origin, and two zoom factors.
 */
public class TableAdapter {

  /**
   * Constructor */
  public TableAdapter()
  {
  }


  /**
   * Constructor with the data model and the dimension of the componenet 
   * that will contain the model. This constructor takes care of setting
   * the right zoom parameters for a graphic representation in the given
   * dimension. 
   * The computed horizontal zoom ensures that all the points will be 
   * contained in the given horizontal dimension.
   * The computed vertical zoom ensures that the table values range,
   * at least minVisibleValue points, will be contained
   * in the given vertical dimension, */
  public TableAdapter(TableDataModel tm, Dimension d, int minVisibleValue)
  {
    float fx = findZoomRatioClosestTo(((float)(d.width))/tm.getSize());
    setXZoom(fx);
    
    int maxY = Math.abs(tm.max());
    int minY = Math.abs(tm.min());
    if (minY > maxY)
      maxY = minY;

    //maxY now contains the max absolute value in the table
    if(maxY < minVisibleValue) maxY = minVisibleValue;

    float fy = findZoomRatioClosestTo(((float)(d.height))/(2*maxY));
    setYZoom(fy);
    
    setOY(maxY);
  } 

  /**
   * Utility routine to find a float number under the form n/1 or 1/n closest
   * to the given float. This kind of ratios are usefull to avoid graphical
   * interpolation problems */
  private float findZoomRatioClosestTo(float f)
  {
    if (f >1) 
	return Math.round(f);
    else
	return (float)(((float)1)/Math.ceil(1/f));
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
   * of a rational number. Use this to avoid pixel interpolation. */
  public void setYZoomFraction(int numerator, int denominator)
  {
    setYZoom(((float)numerator)/denominator);
  }

  private void notifyZoomChanged(int whichOne, float value)
  {
    MaxVector listeners;
    if (whichOne == X_ZOOM) listeners = xZoomListeners; 
    else if (whichOne == Y_ZOOM) listeners = yZoomListeners; 
    else return; //can add other zooms here (?)

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



