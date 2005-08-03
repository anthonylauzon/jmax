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


package ircam.jmax.editors.sequence;

import ircam.jmax.toolkit.*;
import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.editors.sequence.track.Event;
import ircam.jmax.editors.sequence.renderers.*;

import java.beans.*;

/**
* An adapter that treats the y parameter as a constant*/
public class IntegerAdapter extends MonoDimensionalAdapter{
  
  public IntegerAdapter(Geometry geometry, SequenceGraphicContext gc, int constant)
{
    super(geometry, gc, constant);
    
    YMapper = IntegerMapper.getMapper();
    viewMode = MonoTrackEditor.PEAKS_VIEW;
}

public String getType( Event e)
{
  return "int";
}

public void setType( Event e, String type){}

/**
* inherited from Adapter.
 * it returns the Y value of the event,
 * making the needed cordinate conversions.
 */
public int getY(Event e) 
{  
  int value = (int)(YMapper.get(e) - minValue);
  int height = gc.getGraphicDestination().getSize().height;
  float range = getRange();
  float step = (float)((float)height/range);
  
  return height - (int)(value*step);
}

/**
* it returns the Y graphic value of the event from the y logic value,
 * making the needed cordinate conversions.
 */
public int getY(int y) 
{  
	int value = (int)(y - minValue);
	int height = gc.getGraphicDestination().getSize().height;
	float range = getRange();
	float step = (float)((float)height/range);
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
  float range = getRange();
  float step = (float)(range/(float)height);
  
  temp = (int)((height-y)*step + minValue);

  if(temp<minValue) temp = (int)minValue;
  else if(temp>maxValue) temp = (int)maxValue;
  
  return temp;
}

/**
* inherited from Adapter.
 * returns the lenght value of the event,
 * making the needed cordinate conversions (zooming).
 */
public int getLenght(Event e) 
{
	if(getViewMode() == MonoTrackEditor.PEAKS_VIEW)
    return IntegerEventRenderer.INTEGER_WIDTH;
	else
  {//STEP_VIEW
		Event next = gc.getDataModel().getNextEvent(e);
		
		if(next != null)		    
		    return (getX(next.getTime()) - getX(e.getTime()));
		else
		    return (gc.getGraphicDestination().getSize().width - getX(e.getTime()));
  }
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

public float getRange()
{
	return (maxValue-minValue);
}

public float getMaximumValue()
{
	return maxValue;
}
public float getMinimumValue()
{
	return minValue;
}

public boolean isHorizontalMovementBounded()
{
	if(getViewMode() == IntegerTrackEditor.BREAK_POINTS_VIEW)
    return true;
	else
    return false;
}

public boolean isHorizontalMovementAllowed()
{
	if(getViewMode() == IntegerTrackEditor.STEPS_VIEW)
    return false;
	else
    return true;
}

public boolean isDrawable()
{
	return true;
}

public int getViewMode()
{
  return viewMode;
}

////////////////// PropertyChangeListener interface
public void propertyChange(PropertyChangeEvent e)
{
	if(e.getPropertyName().equals("maximumValue"))
    maxValue = ((Float)e.getNewValue()).floatValue();
	else if(e.getPropertyName().equals("minimumValue"))
    minValue = ((Float)e.getNewValue()).floatValue();
	else 
    if(e.getPropertyName().equals("viewMode"))
      setViewMode(((Integer)e.getNewValue()).intValue());
}

//------------- Fields
int constant;    
float maxValue = (float)127.0;
float minValue = (float)0.0;
}





