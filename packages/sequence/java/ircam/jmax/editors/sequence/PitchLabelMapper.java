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

import ircam.jmax.editors.sequence.track.*;

/**
 * a mapper into the pitch value of the SequenceEvents
 */
public class PitchLabelMapper extends StringMapper {
  
  /**
   * set the given pitch in the given event
   */
    public void set(Event e, String val) {}

  /**
   * get the pitch from the given event
   */
    public String get(Event e) 
    {
	return (""+((Integer)e.getProperty("pitch")).intValue());
    }

    public String getName()
    {
	return "pitch";
    }
  /**
   * access the static instance of the class */
  static StringMapper getMapper() 
  {
    return itsPitchMapper;
  }

  //-- Fields

  static PitchLabelMapper itsPitchMapper = new PitchLabelMapper();
}




