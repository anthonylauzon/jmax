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
 * A generic FTS object with an float value.
 * Used for floatbox.
 * If the listener of this object is an instance
 * of FtsFloatValueListener, fire it when the we got a new value
 * from the server.
 */

public class FtsFloatValueObject extends FtsObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  float value; 
  public FtsFloatValueObject(Fts fts, FtsObject parent, String variable, String className, int nArgs, FtsAtom args[])
  {
    super(fts, parent, null, className, className);
  }

  /** Set the value. Tell it to the server, also */

  public void setValue(float value)
  {
    this.value = value;
    getFts().getServer().putObjectProperty(this, "value", value);
  }

  /** Get the current value */

  public float getValue()
  {
    return value;
  }

  /** Ask the server for the latest value */

  public void updateValue()
  {
    getFts().getServer().askObjectProperty(this, "value");
  }
       
  /** Over write the localPut message to handle value changes.
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





