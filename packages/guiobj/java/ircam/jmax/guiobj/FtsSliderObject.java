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
import ircam.fts.client.*;

import java.io.*;

/**
 * A Proxy for FTS  sliders.
 */

public class FtsSliderObject extends FtsIntValueObject
{
  static
  {
    FtsObject.registerMessageHandler( FtsSliderObject.class, FtsSymbol.get("setMinValue"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsSliderObject)obj).setCurrentMinValue( args.getInt(0));
	}
      });
    FtsObject.registerMessageHandler( FtsSliderObject.class, FtsSymbol.get("setMaxValue"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsSliderObject)obj).setCurrentMaxValue( args.getInt(0));
	}
      });
    FtsObject.registerMessageHandler( FtsSliderObject.class, FtsSymbol.get("setOrientation"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsSliderObject)obj).setCurrentOrientation( args.getInt( 0));
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
  protected transient FtsArgs args = new FtsArgs();
    
  /**
   * Create a FtsObject object;
   */
  public FtsSliderObject(FtsServer server, FtsObject parent, int id, String className, FtsAtom args[], int offset, int length)
  {
      super(server, parent, id, className, args, offset, length);

      setNumberOfInlets(1);
      setNumberOfOutlets(1);

      maxValue = 127;
      minValue = 0;
  }

  /** Set the Min Value for the slider.
    Tell the server.
    */

  public void setMinValue(int value)
  {
    minValue = value;

    args.clear();
    args.addSymbol(FtsSymbol.get("minValue"));
    args.addInt(value);
    
    try{
      sendProperty( args);
    }
    catch(IOException e)
      {
	System.err.println("FtsSliderObject: I/O Error sending setMinValue Message!");
	e.printStackTrace(); 
      }
  }

  void setCurrentMinValue( int value)
  {
    minValue = value;
    ((Slider)getObjectListener()).setCurrentMinValue( value);
  }

  /** Set the Max Value for the slider.
    Tell the server.
    */

  public void setMaxValue(int value)
  {
    maxValue = value;

    args.clear();
    args.addSymbol(FtsSymbol.get("maxValue"));
    args.addInt(value);
    try{
	sendProperty( args);
    }
    catch(IOException e)
	{
	    System.err.println("FtsSliderObject: I/O Error sending setMaxValue Message!");
	    e.printStackTrace(); 
	}
  }

  void setCurrentMaxValue( int value)
  {
    maxValue = value;
    ((Slider)getObjectListener()).setCurrentMaxValue( value);
  }

  /** Set the orientation */

  public void setOrientation(int or)
  {
    orientation = or;

    args.clear();
    args.addSymbol(FtsSymbol.get("orientation"));
    args.addInt(or);
    try{
	sendProperty(args);
    }
    catch(IOException e)
	{
	    System.err.println("FtsSliderObject: I/O Error sending setOrientation Message!");
	    e.printStackTrace(); 
	}
  }


  public void setCurrentOrientation(int or)
  {
    orientation = or;
    ((Slider)getObjectListener()).setCurrentOrientation( or);
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






