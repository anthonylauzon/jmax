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

package ircam.jmax.editors.bpf;

import ircam.jmax.editors.bpf.renderers.*;
import ircam.jmax.toolkit.*;

import java.beans.*;

/**
 * the standard, partition-like adapter for a TrackEvent
 * in the Sequence database.
 * The x-coordinates corresponds to time, y and label to pitch, lenght to duration.
 */
public class BpfAdapter {
  
  /**
   * constructor.
   * It creates and assigns its mappers (X, Y, Lenght, Heigth, Label), 
   * and set the initial values for the zoom, transpose and invertion fields.
   */
  public BpfAdapter(Geometry g, BpfGraphicContext gc) 
  {
      this.gc = gc;
      geometry = g;
  }

    public Geometry getGeometry()
    {
	return geometry;
    }

  /**
   * inherited from Adapter.
   * it returns the X value of the event,
   * making the needed cordinate conversions.
   */
  public int getX(BpfPoint p) 
  {
      return getX(p.getTime());
  }

    /**
   * it returns the X graphic value corresponding to the x
   * logical value.
   */
  public int getX(float x) 
  {
    return (int) ((x+geometry.getXTransposition())*geometry.getXZoom()) + DX;
  }

  public int getWidth(float w)
  {
      return (getX(w)-getX((float)0.0));
  }
  /**
   * inherited from Adapter.
   * Returns the time associated with the value of an X coordinate after
   * the coordinate conversion.
   */
  public float getInvX(int x) 
  {
    x = x - DX;
    return (float) (x/geometry.getXZoom() - geometry.getXTransposition());    
  }

  public int getInvWidth(int w)
  {
    return (int)(getInvX(w)-getInvX(0));
  }

  /**
   * set the time of the event associated with the graphic X
   */
   public void setX(BpfPoint p, int x) 
    {  
	int index = gc.getFtsObject().indexOf(p);
	gc.getFtsObject().requestSetPoint(index, getInvX(x), p.getValue());
    }


  /**
   * inherited from Adapter.
   * it returns the Y value of the event,
   * making the needed cordinate conversions.
   */
  public int getY(BpfPoint p) 
  {  
    return getY(p.getValue());
  }
    /**
     * it returns the Y graphic value of the event from the y logic value,
     * making the needed cordinate conversions.
     */
  public int getY(float y) 
  {  
    float value = y - gc.getDataModel().getMinimumValue();
    int height = gc.getGraphicDestination().getSize().height -2*DY;
    float range = gc.getDataModel().getRange();
    float step = (float)((float)height/range);
    return height - (int)(value*step) + DY;
  }

  public void setInvY(BpfPoint p, int y)
  {
    //super.setY(e, y);
  }

  /**
   * inherited from Adapter.
   * Returns the parameter associated with the value of an Y coordinate
   */  
  public float getInvY(int y) 
  {
    y = y - DY;

    float temp;
    int height = gc.getGraphicDestination().getSize().height -2*DY;
    float range = gc.getDataModel().getRange();
    float step = (float)(range/(float)height);
	
    temp = (((height-y)*step) + gc.getDataModel().getMinimumValue());

    return temp;
  }

  /**
   * set the parameter of the event associated with the graphic y
   */
    public void setY(BpfPoint p, int y) 
    {
	int index = gc.getFtsObject().indexOf(p);
	gc.getFtsObject().requestSetPoint(index, p.getTime(), getInvY(y));
    }

  /**
   * inherited from Adapter.
   * returns the lenght value of the event in graphic coordinates,
   * making the needed cordinate conversions (zooming).
   */
    public int getLenght(BpfPoint p)
    {
	return PointRenderer.POINT_RADIUS*2;
    }
  /**
   * inherited from Adapter.
   * returns the lenght value of the event in logic coordinates,
   */
   public float getInvLenght(BpfPoint p)
    {
	return getInvX(PointRenderer.POINT_RADIUS*2);
    }

    public float getInvHeight(int w)
    {
	return (getInvY(w)-getInvY(0));
    }
    /**
     * returns the heigth of this event */
    public int getHeigth(BpfPoint p) 
    {
	int y = getY(p);
	int y0 = getY(0);
	return (y0-y);
    }
    
    public boolean isDisplayLabels()
    {
	return displayLabels;
    }
    public void setDisplayLabels(boolean display)
    {
	displayLabels = display;
    }

    public boolean isDrawable()
    {
	return true;
    }

    public float getXZoom()
    {
	return geometry.getXZoom()*1000/2;
    }

    //------------- Fields
    int constant;    
    boolean displayLabels = true;
    Geometry geometry;

    BpfGraphicContext gc;

    public static int DX = 15;
    public static int DY = 20;
}







