//
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.editors.explode;

/**
 * a mapper into the time value of the ScrEvents
 */
public class ChannelMapper extends Mapper {
  
  /**
   * set the given channel in the given event
   */
  public void set(ScrEvent e, int value) 
  {
    e.setChannel(value);
  }
  
  /**
   * get the channel of the given event
   */
  public int get(ScrEvent e) 
  {
    return e.getChannel();
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

