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

import ircam.jmax.editors.bpf.renderers.*;

import ircam.jmax.toolkit.*;

/**
 * An utility class a graphic only event in a track.  
 */

public class UtilBpfPoint extends BpfPoint/*implements Drawable*/
{
    public UtilBpfPoint()
    {
	this.time = BpfPoint.DEFAULT_TIME;
	this.value = BpfPoint.DEFAULT_VALUE;
    }
    public UtilBpfPoint(float time, float value)
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
    
    /**
     * Set the initial time for this event. Use move() when the event is into a 
     * model (i.e. always for the editors, the exception are Events created on-the-fly 
     * by specific intereaction modules), in order to keep the DB consistency. */
    public void setTime(float time)   
    {
	this.time = time;
    }

    /**
     * This is the method that must be called by the editors to
     * change the initial time of an event. It takes care of
     * keeping the data base consistency */
    public void move(float time)
    {
	if (time < 0) time = 0;
	else setTime(time);
    }

    public void setValue(float value)
    {
	this.value = value;
    }

    public float getValue()
    {
	return value;
    }

    public void setOriginal(BpfPoint pt)
    {
	point = pt;
    }
    public BpfPoint getOriginal()
    {
	return point;
    }
    
    int deltaX = 0;
    public int getDeltaX(BpfAdapter adapter)
    {
	//return (adapter.getX(this) - adapter.getX(trackEvent));
	return deltaX;
    }
    public void setDeltaX(int dx)
    {
	deltaX = dx;
    }
    public int getDeltaY(BpfAdapter adapter)
    {
	return (adapter.getY(this) - adapter.getY(point));
    }

    //--- Fields
    private float time;
    private float value;
    private boolean inGroup = false;
    private BpfPoint point;
}









