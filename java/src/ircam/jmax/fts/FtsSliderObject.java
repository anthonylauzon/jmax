//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.fts;

import ircam.jmax.*;

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

  /**
   * Create a FtsObject object;
   */

  public FtsSliderObject(Fts fts, FtsObject parent, String description, int objId)
  {
    super(fts, parent, "slider", description, objId);
  }


  /** Set the Min Value for the slider.
    Tell the server.
    */

  public void setMinValue(int value)
  {
    minValue = value;
    fts.getServer().putObjectProperty(this, "minValue", minValue);
  }

  /** Set the Max Value for the slider.
    Tell the server.
    */

  public void setMaxValue(int value)
  {
    maxValue = value;
    fts.getServer().putObjectProperty(this, "maxValue", maxValue);
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
    else
      super.localPut(name, value);
  }
}






