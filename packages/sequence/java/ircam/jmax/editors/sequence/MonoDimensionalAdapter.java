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


package ircam.jmax.editors.sequence;

import ircam.jmax.toolkit.*;
import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.editors.sequence.track.Event;
import ircam.jmax.editors.sequence.renderers.*;

import java.beans.*;

/**
 * An adapter that treats the y parameter as a constant*/
public class MonoDimensionalAdapter extends PartitionAdapter {

    public MonoDimensionalAdapter(Geometry geometry, SequenceGraphicContext gc, int constant)
    {
	super(geometry, gc);
	this.constant = constant;
	
	YMapper = IntegerMapper.getMapper();
	viewMode = MonoTrackEditor.PEAKS_VIEW;
    }

    public void setLabelMapper(StringMapper mapper)
    {
	LabelMapper = mapper;
    }

    /**
     * inherited from Adapter.
     * it returns the Y value of the event,
     * making the needed cordinate conversions.
     */
    public int getY(Event e) 
    {  
	int value = YMapper.get(e) - minValue;
	int height = gc.getGraphicDestination().getSize().height;
	int range = getRange();
	float step = (float)((float)height/(float)range);
	
	return height - (int)(value*step);
    }

    /**
     * it returns the Y graphic value of the event from the y logic value,
     * making the needed cordinate conversions.
     */
    public int getY(int y) 
    {  
	int value = y - minValue;
	int height = gc.getGraphicDestination().getSize().height;
	int range = getRange();
	float step = (float)((float)height/(float)range);
	return height - (int)(value*step);
    }

    /**
     * inherited from Adapter.
     * Returns the parameter associated with the value of an Y coordinate
     */  
    public int getInvY(int y) 
    {
	int temp;
	int height = gc.getGraphicDestination().getSize().height;
	int range = getRange();
	float step = (float)((float)range/(float)height);
	
	temp = (int)((height-y)*step) + minValue;
	
	if(temp<minValue) temp = minValue;
	else if(temp>maxValue) temp = maxValue;

	return temp;
    }

    /**
     * inherited from Adapter.
     * returns the lenght value of the event,
     * making the needed cordinate conversions (zooming).
     */
    public int getLenght(Event e) 
    {
	return super.getLenght(e);
    }

  /**
   * set the duration of the event associated with the graphic lenght l.
   */
    public void setLenght(Event e, int l){}

    /**
     * returns the heigth of this event */
    public int getHeigth(Event e) 
    {
	int y = getY(e);
	int y0 = getY(0);
	/* for now return a height positive or negative*/
	//if(y>y0)  return (y-y0);
	/*else*/ return (y0-y);
    }

    public int getRange()
    {
	return (maxValue-minValue);
    }

    public int getMaximumValue()
    {
	return maxValue;
    }
    public int getMinimumValue()
    {
	return minValue;
    }

    ////////////////// PropertyChangeListener interface
    public void propertyChange(PropertyChangeEvent e)
    {
	if(e.getPropertyName().equals("maximumValue"))
	    maxValue = ((Integer)e.getNewValue()).intValue();
	else if(e.getPropertyName().equals("minimumValue"))
	    minValue = ((Integer)e.getNewValue()).intValue();
	else 
	    if(e.getPropertyName().equals("viewMode"))
		setViewMode(((Integer)e.getNewValue()).intValue());
    }

    //------------- Fields
    int constant;    
    int maxValue = 127;
    int minValue = 0;
}





