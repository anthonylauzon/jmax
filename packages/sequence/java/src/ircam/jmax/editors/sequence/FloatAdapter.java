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


package ircam.jmax.editors.sequence;

import ircam.jmax.toolkit.*;
import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.editors.sequence.track.Event;
import ircam.jmax.editors.sequence.renderers.*;

import java.beans.*;

/**
 * An adapter that treats the y parameter as a constant*/
public class FloatAdapter extends IntegerAdapter implements VerticalZoomable, VerticalScrollable{

    public FloatAdapter(Geometry geometry, SequenceGraphicContext gc, int constant)
    {
	super(geometry, gc, constant);
	maxValue = 1;
	yZoomFactor = 1;
	yTransposition = 0;
    }

    /**
     * inherited from Adapter.
     * it returns the Y value of the event,
     * making the needed cordinate conversions.
     */
    public int getY(Event e) 
    {  
	float value = (FloatMapper.getMapper().get(e)+yTransposition)*yZoomFactor - (float)minValue;
	int height = gc.getGraphicDestination().getSize().height;
	int range = getRange();
	float step = (float)((float)height/(float)range);
	
	return height - (int)(value*step);
    }

    /**
     * it returns the Y graphic value of the event from the y logic value,
     * making the needed cordinate conversions.
     */
    public int getY(float y) 
    {  
	float value = (y+yTransposition)*yZoomFactor - (float)minValue;
	int height = gc.getGraphicDestination().getSize().height;
	int range = getRange();
	float step = (float)((float)height/(float)range);
	return height - (int)(value*step);
    }

    public void setY(Event e, int y) 
    {
	FloatMapper.getMapper().set(e, getFloatInvY(y));
    }
    
    //VerticalZoomable interface
    public void incrYZoom(int delta) 
    {
	yZoomFactor = (yZoomFactor*100+delta)/(float)100;
	if(yZoomFactor<0.01) yZoomFactor = (float)0.01;
	if(yZoomFactor>10) yZoomFactor = (float)10.0;
    	
	/*float max = getFloatInvY(0);
	  float min = getFloatInvY(gc.getGraphicDestination().getSize().height);

	  System.err.println("max "+max+" min "+min);
	  System.err.println("maximum "+getMaximumValue()+" minimum "+getMinimumValue());
	  
	  if(max > getMaximumValue())
	  gc.getTrack().setProperty("maximumValue", new Integer((int)max+1));
	  if(min < getMinimumValue())
	  gc.getTrack().setProperty("minimumValue", new Integer((int)min-1));*/
    }
    public void setYZoom(int factor)
    {
	yZoomFactor = factor/(float)100;
    }

    public int getYZoom()
    {
	return (int)(yZoomFactor*100);
    }

    //VarticalScrollable interface
    public void setYTransposition(float value)
    {
	yTransposition = value;
    }
    public float getYTransposition()
    {
	return yTransposition;
    }
    public void incrYTransp(int delta) 
    {
	float del;
	if(delta>=0)
	    del = getInvHeight(delta);
	else
	    del = -getInvHeight(-delta);
	
	int height = gc.getGraphicDestination().getSize().height;

	if((getFloatInvY(height)-del>=minValue)&&(getFloatInvY(0)-del<=maxValue))
	    yTransposition += del;
    }
    /**
     * inherited from Adapter.
     * Returns the parameter associated with the value of an Y coordinate
     */  
    public float getFloatInvY(int y) 
    {
	float temp;
	int height = gc.getGraphicDestination().getSize().height;
	int range = getRange();
	float step = (float)((float)range/(float)height);
	
	temp = (((height-y)*step) + (float)minValue)/yZoomFactor-yTransposition;

	return temp;
    }

    public float getInvHeight(int w)
    {
	return (getFloatInvY(w)-getFloatInvY(0));
    }

    /**
     * returns the heigth of this event */
    public int getHeigth(Event e) 
    {
	float y = getY(e);
	float y0 = getY((float)0.0);
	return (int)(y0-y);
    }

    public boolean isVerticalZoomable()
    {
	return true;
    }
    public boolean isVerticalScrollable()
    {
	return true;
    }

    float yZoomFactor;
    float yTransposition;
}





