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
 * A generic FTS object with an float value; floatbox and sliders, for example
 */

public class FtsFloatValueObject extends FtsObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  float value; 

  /**
   * Create a FtsObject object;
   */

  public FtsFloatValueObject(Fts fts, FtsObject parent, String className, String description, int objId)
  {
    super(fts, parent, className, null, description, objId);
  }

  public void setValue(float value)
  {
    this.value = value;
    fts.getServer().putObjectProperty(this, "value", value);
  }

  public float getValue()
  {
    return value;
  }

  public void updateValue()
  {
    fts.getServer().askObjectProperty(this, "value");
  }
       
  /* Over write the localPut message to handle value changes;
   */

  protected void localPut(String name, float newValue)
  {
    if (name == "value")
      {
	value = newValue;
	
	if (listener instanceof FtsFloatValueListener)
	  ((FtsFloatValueListener) listener).valueChanged(newValue);
      }
    else
      super.localPut(name, newValue);
  }
}






