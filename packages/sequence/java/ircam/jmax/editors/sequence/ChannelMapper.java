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

import ircam.jmax.editors.sequence.track.*;

/**
 * a mapper into the time value of the SequenceEvents
 */
public class ChannelMapper extends Mapper {
  
  /**
   * set the given channel in the given event
   */
    public void set(Event e, int value) 
  {
    e.setProperty("channel", new Integer(value));
  }
  
  /**
   * get the channel of the given event
   */
    public int get(Event e) 
  {
    return ((Integer)e.getProperty("channel")).intValue();
  }

    public String getName()
    {
	return "channel";
    }

  /**
   * access the static instance
   */
  static public Mapper getMapper()
  {
    if (itsChannelMapper == null)
      itsChannelMapper = new ChannelMapper();
    
    return itsChannelMapper;
  }

  //--- Fields

  static ChannelMapper itsChannelMapper;
}

