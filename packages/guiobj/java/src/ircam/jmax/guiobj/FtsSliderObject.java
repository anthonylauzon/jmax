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
 * A Proxy for FTS  sliders.
 */

public class FtsSliderObject extends FtsIntValueObject
{
   static
    {
	ircam.ftsclient.FtsObject.registerMessageHandler( FtsSliderObject.class, FtsSymbol.get("setMinValue"), new FtsMessageHandler(){
		public void invoke( ircam.ftsclient.FtsObject obj, int argc, ircam.ftsclient.FtsAtom[] argv)
		{
		    ((FtsSliderObject)obj).minValue = argv[0].intValue;
		}
	    });
	ircam.ftsclient.FtsObject.registerMessageHandler( FtsSliderObject.class, FtsSymbol.get("setMaxValue"), new FtsMessageHandler(){
		public void invoke( ircam.ftsclient.FtsObject obj, int argc, ircam.ftsclient.FtsAtom[] argv)
		{
		    ((FtsSliderObject)obj).maxValue = argv[0].intValue;
		}
	    });
	ircam.ftsclient.FtsObject.registerMessageHandler( FtsSliderObject.class, FtsSymbol.get("setOrientation"), new FtsMessageHandler(){
		public void invoke( ircam.ftsclient.FtsObject obj, int argc, ircam.ftsclient.FtsAtom[] argv)
		{
		    ((FtsSliderObject)obj).orientation = argv[0].intValue;
		}
	    });
    }

  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  int minValue;
  int maxValue;
  int orientation;
  protected FtsArgs args = new FtsArgs();
    
  /**
   * Create a FtsObject object;
   */
  public FtsSliderObject(FtsServer server, FtsObject parent, FtsSymbol className, int nArgs, FtsAtom args[], int id)
  {
      super(server, parent, className, nArgs, args, id);

      maxValue = 127;
      minValue = 0;
  }

  public void setDefaults()
  {
      setWidth(20);
      setHeight(137);
  }

  /** Set the Min Value for the slider.
    Tell the server.
    */

  public void setMinValue(int value)
  {
    minValue = value;

    args.clear();
    args.add(value);
    try{
	send( FtsSymbol.get("setMinValue"), args);
    }
    catch(IOException e)
	{
	    System.err.println("FtsSliderObject: I/O Error sending setMinValue Message!");
	    e.printStackTrace(); 
	}
  }

  /** Set the Max Value for the slider.
    Tell the server.
    */

  public void setMaxValue(int value)
  {
    maxValue = value;

    args.clear();
    args.add(value);
    try{
	send( FtsSymbol.get("setMaxValue"), args);
    }
    catch(IOException e)
	{
	    System.err.println("FtsSliderObject: I/O Error sending setMaxValue Message!");
	    e.printStackTrace(); 
	}
  }

  /** Set the orientation */

  public void setOrientation(int or)
  {
    orientation = or;

    args.clear();
    args.add(or);
    try{
	send( FtsSymbol.get("setOrientation"), args);
    }
    catch(IOException e)
	{
	    System.err.println("FtsSliderObject: I/O Error sending setOrientation Message!");
	    e.printStackTrace(); 
	}
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
}






