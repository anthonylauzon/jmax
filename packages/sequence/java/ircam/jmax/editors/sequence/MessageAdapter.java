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
public class MessageAdapter extends MonoDimensionalAdapter{

    public MessageAdapter(Geometry geometry, SequenceGraphicContext gc, int constant)
    {
	super(geometry, gc, constant);
	
	YMapper = IntegerMapper.getMapper();
	LabelMapper = MessageMapper.getMapper();
    }

    /**
     * inherited from Adapter.
     * returns the lenght value of the event,
     * making the needed cordinate conversions (zooming).
     */
    /*public int getLenght(Event e) 
      {
      return (int)(LenghtMapper.get(e)*((float)0.2));
      }*/

  /**
   * set the duration of the event associated with the graphic lenght l.
   */
    public void setLenght(Event e, int l){}

    /*public int getInvWidth(int w)
      {
      //return (int)(getInvX(w)-getInvX(0));
      if (geometry.getXInvertion())
      return (int)(-w/(float)0.2);
      else
      return (int)(w/(float)0.2);
      }*/
}





