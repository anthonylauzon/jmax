//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
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
 * A specific  FTS object to handle sliders.
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


  public void setMinValue(int value)
  {
    minValue = value;
    fts.getServer().putObjectProperty(this, "minValue", minValue);
  }

  public void setMaxValue(int value)
  {
    maxValue = value;
    fts.getServer().putObjectProperty(this, "maxValue", maxValue);
  }

  public int  getMinValue()
  {
    return minValue;
  }

  public int getMaxValue()
  {
    return maxValue;
  }

  /* Over write the localPut message to handle value changes;
     It does *not* call the listener on max and min values.
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






