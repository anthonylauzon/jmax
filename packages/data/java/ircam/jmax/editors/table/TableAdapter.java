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

package ircam.jmax.editors.table;

import ircam.jmax.*;
import ircam.jmax.toolkit.*;
import java.awt.*;
import javax.swing.*;
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
  public TableAdapter(TableDataModel tm, TableGraphicContext gc)
  {
    this.gc = gc;
    this.dataModel = tm;

    int maxY = Math.abs(tm.max());
    int minY = Math.abs(tm.min());
    
    if (minY > maxY)
      maxY = minY;
    
    //maxY now contains the max absolute value in the table
    if(maxY < gc.getVerticalMaximum()) maxY = gc.getVerticalMaximum();
    
    setDefaultZooms();
} 

  public void setDefaultZooms()
  {
    setXZoom( (float)1.0);
   }
  
  public void zoomToWindow()
  {
    setXZoom( findZoomRatioClosestTo( (float)gc.getGraphicDestination().getSize().width/dataModel.getSize()));
  }
  
  /**
   * Utility routine to find a double number under the form n/1 or 1/n closest
   * to the given double. This kind of ratios are usefull to avoid graphical
   * interpolation problems */
  public float findZoomRatioClosestTo(float f)
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
   * remove a listener for the xZoom factor */
  public void removeXZoomListener(ZoomListener zl)
  {
    xZoomListeners.removeElement(zl);
  }

  /**
   * returns the graphic x corresponding to the index */
  public int getX(int index)
  {
    return (int) ((index-xTransposition) * itsXZoom);
  }

  /**
   * returns the index in the table corresponding to the graphic x */
  public int getInvX(int x)
  {
    return (int) (x/itsXZoom+xTransposition);
  }

  /**
   * returns the graphic y corresponding to the value.
   * The y coordinates are implicitily inverted. */
  public int getY(double value)
  {
    if( !gc.isIvec()) value = ((double)value*100); 
    
    double V = value - gc.getVerticalMinimum();
    int h = gc.getGraphicDestination().getSize().height;
    float range = gc.getVerticalRange();
    float step = (float)h/range;
    return (int)(h - V * step);
  }

  /**
   * returns the value corresponding to the graphic y */ 
  public double getInvY(int y)
  {
    double value;
    int h = gc.getGraphicDestination().getSize().height;
    float range = gc.getVerticalRange();
    float step = range/(float)h;
    
    value = (int)((h - y) * step + gc.getVerticalMinimum());
    if(value < gc.getVerticalMinimum()) value = (int)gc.getVerticalMinimum();
    else if(value > gc.getVerticalMaximum()) value = (int)gc.getVerticalMaximum();
    
    if( !gc.isIvec()) value = (double)(value/100.0); 
    
    return value;
  }

  /**
   * A method that allows setting the X zoom factor under the form
   * of a rational number */
  public void setXZoomFraction(int numerator, int denominator)
  {
    setXZoom(((float)numerator)/denominator);
  }

  private void notifyZoomChanged(int whichOne, float value, float oldValue)
  {
    MaxVector listeners = xZoomListeners;
    ZoomListener zl;
    for (int i = 0; i< listeners.size(); i++)
    {
      zl = (ZoomListener) listeners.elementAt(i);
      zl.zoomChanged( value, oldValue);
    }
  }
  
  //--- Fields & accessors ---//
  float itsXZoom;

  MaxVector xZoomListeners = new MaxVector();
  MaxVector transpositionListeners = new MaxVector();

  public static int X_ZOOM = 0;
  /** the first index in the table we're showing */
  int xTransposition; 
  /** the graphic x offset of oX */
  int xOffset;

  public float getXZoom()
  {
    return itsXZoom;
  } 

  public void setXZoom(float zoom)
  {
    float old = itsXZoom;
    itsXZoom = zoom;
    notifyZoomChanged(X_ZOOM, zoom, old);
  }

  public int getXTransposition()
  {
    return xTransposition;
  }

  public void setXTransposition(int xT)
  {
    xTransposition = xT;
    notifyTranspositionChanged(xTransposition);
  }

  public void addTranspositionListener(TranspositionListener tl)
  {
    transpositionListeners.addElement(tl);
  }
  public void removeTranspositionListener(TranspositionListener tl)
  {
    transpositionListeners.removeElement(tl);
  }
  private void notifyTranspositionChanged(int value)
  {
    TranspositionListener tl;
    for (int i = 0; i< transpositionListeners.size(); i++)
    {
      tl = (TranspositionListener) transpositionListeners.elementAt(i);
      tl.transpositionChanged( value);
    }
  }  
  
  public int getXOffset()
  {
    return xOffset;
  }

  public void setXOffset(int theOX)
  {
    xOffset = theOX;
  }

  TableGraphicContext gc;
  TableDataModel dataModel;
}



