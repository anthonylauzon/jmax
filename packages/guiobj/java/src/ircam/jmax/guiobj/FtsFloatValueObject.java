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
import ircam.ftsclient.*;

import java.io.*;

/**
 * A generic FTS object with an float value.
 * Used for floatbox.
 * If the listener of this object is an instance
 * of FtsFloatValueListener, fire it when the we got a new value
 * from the server.
 */

public class FtsFloatValueObject extends FtsGraphicObject
{
   static
    {
	ircam.ftsclient.FtsObject.registerMessageHandler(FtsFloatValueObject.class, FtsSymbol.get("setValue"), new FtsMessageHandler(){
		public void invoke( ircam.ftsclient.FtsObject obj, int argc, ircam.ftsclient.FtsAtom[] argv)
		{
		    ((FtsFloatValueObject)obj).setCurrentValue(argv[0].floatValue);
		}
	    });
    }

  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  private float value; 
  
  public FtsFloatValueObject(FtsServer server, FtsObject parent, FtsSymbol className, int nArgs, FtsAtom args[], int id)
  {
    super(server, parent, className, nArgs, args, id);
  }

  /** Set the value. Tell it to the server, also */

  public void setValue(float value)
  {
    this.value = value;
   
    args.clear();
    args.add(value);
    try{
	send( FtsSymbol.get("setValue"), args);
    }
    catch(IOException e)
	{
	    System.err.println("FtsFloatValueObject: I/O Error sending setValue Message!");
	    e.printStackTrace(); 
	}
  }

  /** Get the current value */

  public float getValue()
  {
    return value;
  }

  /** Ask the server for the latest value */

  public void updateValue()
  {
      try{
	  send(FtsSymbol.get("getValue"));
      }
      catch(IOException e)
	  {
	      System.err.println("FtsFloatValueObject: I/O Error sending getValue Message!");
	      e.printStackTrace(); 
	  }
  }
       
  /** Over write the localPut message to handle value changes.
   */

  protected void setCurrentValue(float newValue)
  {
      value = newValue;
	
      if (listener instanceof FtsFloatValueListener)
	  ((FtsFloatValueListener) listener).valueChanged(newValue);
  }
}






