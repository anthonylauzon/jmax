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
 * A generic FTS object with an int value; intbox and sliders, for example
 */

public class FtsIntValueObject extends FtsObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  int value; 

  /**
   * Create a FtsObject object;
   */

  public FtsIntValueObject(Fts fts, FtsObject parent, String className, String description, int objId)
  {
    super(fts, parent, className, null, description, objId);
  }

  public void setValue(int value)
  {
    this.value = value;
    fts.getServer().putObjectProperty(this, "value", value);
  }

  public int getValue()
  {
    return value;
  }

  public void updateValue()
  {
    fts.getServer().askObjectProperty(this, "value");
  }
       
  /* Over write the localPut message to handle value changes;
   */

  protected void localPut(String name, int newValue)
  {
    if (name == "value")
      {
	value = newValue;

	if (listener instanceof FtsIntValueListener)
	  ((FtsIntValueListener) listener).valueChanged(newValue);
      }
    else
      super.localPut(name, newValue);
  }
}






