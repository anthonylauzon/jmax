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
public class AnythingAdapter extends MonoDimensionalAdapter{

    public AnythingAdapter(Geometry geometry, SequenceGraphicContext gc, int constant)
    {
	super(geometry, gc, constant);
	
	YMapper = IntegerMapper.getMapper();
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
	int height = gc.getGraphicDestination().getSize().height;
	return (height - AnythingEventRenderer.ANY_HEIGHT);
    }

    /**
     * it returns the Y graphic value of the event from the y logic value,
     * making the needed cordinate conversions.
     */
    public int getY(int y) 
    {  
	return y;
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
	return AnythingEventRenderer.ANY_LENGTH;
    }

  /**
   * set the duration of the event associated with the graphic lenght l.
   */
    public void setLenght(Event e, int l){}

    /**
     * returns the heigth of this event */
    public int getHeigth(Event e) 
    {
	return AnythingEventRenderer.ANY_HEIGHT;
    }

    ////////////////// PropertyChangeListener interface
    public void propertyChange(PropertyChangeEvent e){}

    //------------- Fields
    int constant;    
    int maxValue = 127;
    int minValue = 0;
}





