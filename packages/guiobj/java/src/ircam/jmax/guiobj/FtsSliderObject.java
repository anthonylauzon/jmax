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

package ircam.jmax.guiobj;

import ircam.jmax.*;
import ircam.jmax.fts.*;

/**
 * A Proxy for FTS  sliders.
 */

public class FtsSliderObject extends FtsIntValueObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  int minValue;
  int maxValue;
  int orientation;

  /**
   * Create a FtsObject object;
   */
  public FtsSliderObject(Fts fts, FtsObject parent, String variable, String className, int nArgs, FtsAtom args[])
  {
      super(fts, parent, variable, className, nArgs, args);
      localPut("maxValue", 127);
      localPut("minValue", 0);
  }


  /** Set the Min Value for the slider.
    Tell the server.
    */

  public void setMinValue(int value)
  {
    minValue = value;
    getFts().getServer().putObjectProperty(this, "minValue", minValue);
  }

  /** Set the Max Value for the slider.
    Tell the server.
    */

  public void setMaxValue(int value)
  {
    maxValue = value;
    getFts().getServer().putObjectProperty(this, "maxValue", maxValue);
  }

  /** Set the orientation */

  public void setOrientation(int or)
  {
    orientation = or;
    getFts().getServer().putObjectProperty(this, "orientation", orientation);
  }

  /** Get the Min Value for the slider. */

  public int  getMinValue()
  {
    return minValue;
  }

  /** Get the Max Value for the slider. */

  public int getMaxValue()
  {
    return maxValue;
  }

  public int getOrientation()
  {
    return orientation;
  }

  /** Over write the localPut message to handle the min/max properties;
     It does *not* call the listener on max and min values.
     There is no listener for max/min changes.
   */

  protected void localPut(String name, int value)
  {
    if (name == "minValue")
      {
	minValue = value;
      }
    else if (name == "maxValue")
      {
	maxValue = value;
      }
    else if (name == "orientation")
      {
	orientation = value;
      }
    else
      super.localPut(name, value);
  }
}





