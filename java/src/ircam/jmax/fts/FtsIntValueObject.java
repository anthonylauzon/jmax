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

package ircam.jmax.fts;

import java.io.*;

import ircam.fts.client.*;
import ircam.jmax.*;
import ircam.jmax.editors.patcher.objects.NumberBox;

/**
 * A generic FTS object with an int value.
 * Used for intbox and sliders, for example.
 * If the listener of this object is an instance
 * of FtsIntValueListener, fire it when the we got a new value
 * from the server.
 */

public class FtsIntValueObject extends FtsGraphicObject
{
  static
  {
    FtsObject.registerMessageHandler( FtsIntValueObject.class, FtsSymbol.get("value"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsIntValueObject)obj).setCurrentValue( args.getInt( 0));
	}
      });
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  int value; 
  protected transient FtsArgs args = new FtsArgs();

  public FtsIntValueObject(FtsServer server, FtsObject parent, int id, String className, FtsAtom args[], int offset, int length)
  {
    super(server, parent, id, className, args, offset, length);
    setNumberOfInlets(1);
    setNumberOfOutlets(1);
  }

  public FtsIntValueObject(FtsServer server, FtsObject parent, int id, String className, String arguments)
  {
    super(server, parent, id, className, arguments);
    setNumberOfInlets(1);
    setNumberOfOutlets(1);
  }

  public void setValue(int value)
  {
    this.value = value;
    args.clear();
    args.addInt(value);
    try{
      send( FtsSymbol.get("value"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsIntValueObject: I/O Error sending setValue Message!");
	e.printStackTrace(); 
      }
  }
  
  /** Get the current value */
  
  public int getValue()
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
	System.err.println("FtsIntValueObject: I/O Error sending getValue Message!");
	e.printStackTrace(); 
      }
  }
  
  public void sendBang()
  {
    try{
      send(FtsSymbol.get("click"));
    }
    catch(IOException e)
      {
	System.err.println("FtsIntValueObject: I/O Error sending bang Message!");
	e.printStackTrace(); 
      }
  }
  /* Over write the localPut message to handle value changes;
   */
  
  void setCurrentValue(int newValue)
  {
    value = newValue;
    
    if (listener instanceof FtsIntValueListener)
      ((FtsIntValueListener) listener).valueChanged(newValue);
  }
}





