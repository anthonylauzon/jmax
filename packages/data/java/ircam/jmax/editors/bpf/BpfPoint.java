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


package ircam.jmax.editors.bpf;

import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;

/**
 * The class representing an event in a track. These objects have a time 
 * and a value (a FtsRemoteData).
 */

public class BpfPoint extends Object /*implements Drawable*/
{
    public BpfPoint()
    {
	this.time = DEFAULT_TIME;
	this.value = DEFAULT_VALUE;
    }

    public BpfPoint(float time, float value)
    {
	this.time = time;
	this.value = value;
    }

    /**
     * Get the initial time for this event */
    public float getTime()
    {
	return time;
    }
    
    public void setTime(float time)   
    {
	this.time = time;
    }

    /**
     * Returns the value of this event */
    public float getValue()
    {
	return value;
    }

    /** Set the Value corresponding to this event */
    public void setValue(float value)
    {
	this.value = value;
    }

    /*public ObjectRenderer getRenderer()
      {
      if(type == POINT_TYPE)
      return PointRenderer.getRenderer();
      else
      return SustainRenderer.getRenderer();
      }*/

    //--- Fields
    private float time;
    private float value;

    public static float DEFAULT_TIME = 0;
    public static float DEFAULT_VALUE = 0;
}







