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

import ircam.jmax.editors.sequence.renderers.*;
import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.toolkit.*;

import java.beans.*;

/**
 * the standard, partition-like adapter for a TrackEvent
 * in the Sequence database.
 * The x-coordinates corresponds to time, y and label to pitch, lenght to duration.
 */
public class PartitionAdapter extends Adapter implements PropertyChangeListener{
  
  /**
   * constructor.
   * It creates and assigns its mappers (X, Y, Lenght, Heigth, Label), 
   * and set the initial values for the zoom, transpose and invertion fields.
   */
  public PartitionAdapter(Geometry g, SequenceGraphicContext gc) 
  {
    super(g);
    XMapper = TimeMapper.getMapper();
    YMapper = PitchMapper.getMapper();
    LenghtMapper = DurationMapper.getMapper();
    HeigthMapper = IntervalMapper.getMapper();
    LabelMapper = PitchLabelMapper.getMapper();
      
    itsName = "Standard Adapter";
    this.gc = gc;
  }


  /**
   * inherited from Adapter.
   * it returns the X value of the event,
   * making the needed cordinate conversions.
   */
  public int getX(Event e) 
  {
    int temp = super.getX(e);
    
    if (geometry.getXInvertion()) temp = -temp;

    return (int) ((temp+geometry.getXTransposition())*geometry.getXZoom()) + ScoreBackground.KEYEND;
  }

    /**
   * it returns the X graphic value corresponding to the x
   * logical value.
   */
  public int getX(double x) 
  {
    if (geometry.getXInvertion()) x = -x;

    return (int) ((x+geometry.getXTransposition())*geometry.getXZoom()) + ScoreBackground.KEYEND;
  }

  public int getWidth(double w)
  {
      return (getX(w)-getX(0.0));
  }
  /**
   * inherited from Adapter.
   * Returns the time associated with the value of an X coordinate after
   * the coordinate conversion.
   */
  public double getInvX(int x) 
  {
    if (geometry.getXInvertion()) return (double) (geometry.getXTransposition() -(x-ScoreBackground.KEYEND)/geometry.getXZoom());
    
    else return (double) ((x-ScoreBackground.KEYEND)/geometry.getXZoom() - geometry.getXTransposition());
    
  }

  public int getInvWidth(int w)
  {
    return (int)(getInvX(w)-getInvX(0));
  }

  /**
   * inherited from Adapter.
   * it returns the Y value of the event,
   * making the needed cordinate conversions.
   */
    public int getY(Event e) 
    {  
	int q, r;
	int temp = super.getY(e);
    
	if(viewMode==MidiTrackEditor.PIANOROLL_VIEW)
	    {
		if (geometry.getYInvertion()) temp = -temp;
		temp = (int) ((temp+geometry.getYTransposition())*geometry.getYZoom());  
	    }
	else//NMS_VIEW
	    {
		q = temp/12;
		r = getRestFromIntervall(temp - q*12);
		temp = PartitionBackground.SC_BOTTOM-9-(q*7+r)*4 - 3;
	    }
	return temp-getVerticalTransp();//@@@@@@@@@@@@@@@@@@@@@@@@@
    }
    /**
     * it returns the Y graphic value of the event from the y logic value,
     * making the needed cordinate conversions.
     */
    public int getY(int y) 
    {  
	int q, r;
	int temp = y;
    
	if(viewMode==MidiTrackEditor.PIANOROLL_VIEW)
	    {
		if (geometry.getYInvertion()) temp = -temp;
		temp = (int) ((temp+geometry.getYTransposition())*geometry.getYZoom());  
	    }
	else//NMS_VIEW
	    {
		q = temp/12;
		r = getRestFromIntervall(temp - q*12);
		temp = PartitionBackground.SC_BOTTOM-9-(q*7+r)*4 - 3;
	    }
	return temp/*-getVerticalTransp()*/;//@@@@@@@@@@@@@@@@@@@@@@@@@
    }

  public void setInvY(Event e, int y)
  {
      super.setY(e, y);
  }

  /**
   * inherited from Adapter.
   * Returns the parameter associated with the value of an Y coordinate
   */  
  public int getInvY(int y) 
  {
    int temp , rest, q, r;

    y = y+getVerticalTransp();//@@@@@@@@@@@@@@

    if(viewMode==MidiTrackEditor.PIANOROLL_VIEW)
    {
      if (geometry.getYInvertion()) temp = (int) (geometry.getYTransposition() -(int)(y/geometry.getYZoom()));
      else temp = (int) (y/geometry.getYZoom() - geometry.getYTransposition());  
    }
    else//NMS_VIEW
    {
	temp = (PartitionBackground.SC_BOTTOM-9-y)/4;
	rest = (PartitionBackground.SC_BOTTOM-9-y) - temp*4;

	q = temp/7;
	r = temp - q*7;
	temp = q*12 + getIntervallFromRest(r, rest);
    }
    
    if(temp<minPitch) temp = minPitch;
    else if(temp>maxPitch) temp = maxPitch;

    return temp;
  }

    int getIntervallFromRest(int r, int rest)
    {
	switch(r)
	    {
	    case 1: 
		if(rest<2)
		    return 1;
		else
		    return 2;
	    case 2: 
		if(rest<2)
		    return 3;
		else
		    return 4;
	    case 3: return 5;
	    case 4: 
		if(rest<2)
		    return 6;
		else
		    return 7;
	    case 5: 
		if(rest<2)
		    return 8;
		else
		    return 9;
	    case 6: 
		if(rest<2)
		    return 10;
		else
		    return 11;
	    default: return 0;
	    }
    }

    int getRestFromIntervall(int i)
    {
	switch(i)
	    {
	    case 2: return 1;
	    case 3:
	    case 4: return 2;
	    case 5: 
	    case 6: return 3;
	    case 7: 
	    case 8: return 4;
	    case 9: return 5;
	    case 10:
	    case 11: return 6;
	    default: return 0;
	    }
    }

    public int getAlteration(Event e)
    {
	int q, r;
	int temp = super.getY(e);
	q = temp/12;
	r = temp-q*12;
	switch(r)
	    {
	    case 1:
	    case 6:
	    case 8: return ALTERATION_DIESIS;
	    case 3:
	    case 10: return ALTERATION_BEMOLLE;
	    default: return ALTERATION_NOTHING;
	    }
    }
  /**
   * set the parameter of the event associated with the graphic y
   */
  public void setY(Event e, int y) 
  {
    super.setY(e, getInvY(y));
  }

  /**
   * inherited from Adapter.
   * returns the lenght value of the event in graphic coordinates,
   * making the needed cordinate conversions (zooming).
   */
  public int getLenght(Event e)
  {
    return (int)(super.getLenght(e)*geometry.getXZoom());
  }
  /**
   * inherited from Adapter.
   * returns the lenght value of the event in logic coordinates,
   */
  public double getInvLenght(Event e)//*@*// 
  {
    if (LenghtMapper != null) return LenghtMapper.get(e);
    else return 0.0;
  }

  /**
   * set the duration of the event associated with the graphic lenght l.
   */
  public void setLenght(Event e, int l) 
  {
    LenghtMapper.set(e, (double)((double)l/geometry.getXZoom()) );
  }

  /**
   * returns the heigth of this event */
  public int getHeigth(Event e) 
  {
    if (HeigthMapper != null) 
      return NOTE_DEFAULT_HEIGTH*(HeigthMapper.get(e)+1);
    
    return NOTE_DEFAULT_HEIGTH;
  }
    
  public void setHeigth(Event e, int heigth)
  {
    HeigthMapper.set(e, heigth/NOTE_DEFAULT_HEIGTH - 1);
  }

  public void setType(Event e, String type)
  {
    e.setProperty("type", type);
  }

  public String getType(Event e)
  {
    Object type = e.getProperty("type");

    if(type instanceof String) return (String)type;
    else return type.toString();
  }

  /**
   * From the MappingListener interface.
   * Reassign the mappers according to the change, and ask
   * a repaint of the graphic.
   */
    public void mappingChanged(String graphicName, String scoreName) 
    {
	/*if (graphicName.equals("y")) 
	{
	if (scoreName.equals("pitch"))
	YMapper = PitchMapper.getMapper();
	else if (scoreName.equals("duration"))
	YMapper = DurationMapper.getMapper();
	else if (scoreName.equals("velocity"))
	YMapper = VelocityMapper.getMapper();
	else if (scoreName.equals("channel"))
	YMapper = ChannelMapper.getMapper();
	}
	else if (graphicName.equals("lenght")) 
	{
	if (scoreName.equals("pitch"))
	LenghtMapper = PitchMapper.getMapper();
	else if (scoreName.equals("duration"))
	LenghtMapper = DurationMapper.getMapper();
	else if (scoreName.equals("velocity"))
	LenghtMapper = VelocityMapper.getMapper();
	else if (scoreName.equals("channel"))
	LenghtMapper = ChannelMapper.getMapper();
	}
	else if (graphicName.equals("label")) 
	{
	if (scoreName.equals("pitch"))
	LabelMapper = PitchLabelMapper.getMapper();
	else if (scoreName.equals("duration"))
	LabelMapper = DurationLabelMapper.getMapper();
	else if (scoreName.equals("velocity"))
	LabelMapper = VelocityLabelMapper.getMapper();
	else if (scoreName.equals("channel"))
	LabelMapper = ChannelLabelMapper.getMapper();
	}*/
  }
    public boolean isDisplayLabels()
    {
	return displayLabels;
    }
    public void setDisplayLabels(boolean display)
    {
	displayLabels = display;
    }

    public void setViewMode(int mode)
    {
	viewMode = mode;
    }
    public int getViewMode()
    {
	return viewMode;
    }

    public int getRangeHeight()
    {
	return (getY(minPitch)-getY(maxPitch) + 2*ScoreBackground.SC_TOP);
    }

    public int getVerticalTransp()
    {
	int delta;
	int max = getY(maxPitch);
	if(maxPitch<127) delta = max-ScoreBackground.SC_TOP;
	else delta = 0;

	return delta;
    }

    public int getMaxPitch()
    {
	return maxPitch;
    }
    public int getMinPitch()
    {
	return minPitch;
    }

    ////////////////// PropertyChangeListener interface
    public void propertyChange(PropertyChangeEvent e)
    {
	if(e.getPropertyName().equals("maximumPitch"))
	    maxPitch = ((Integer)e.getNewValue()).intValue();
	else if(e.getPropertyName().equals("minimumPitch"))
	    minPitch = ((Integer)e.getNewValue()).intValue();
	else 
	    if(e.getPropertyName().equals("viewMode"))
		setViewMode(((Integer)e.getNewValue()).intValue());
    }

    //------------- Fields
    public static final int NOTE_DEFAULT_HEIGTH = 3;
    int viewMode = MidiTrackEditor.PIANOROLL_VIEW;
    int maxPitch = 127;
    int minPitch = 0;
    boolean displayLabels = true;

    public static final int ALTERATION_DIESIS = 1;
    public static final int ALTERATION_BEMOLLE = -1;
    public static final int ALTERATION_NOTHING = 0;
    SequenceGraphicContext gc;
}







