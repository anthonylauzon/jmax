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

public class FtsXYPadObject extends FtsGraphicObject
{
  static
  {
    FtsObject.registerMessageHandler( FtsXYPadObject.class, FtsSymbol.get("setXMinValue"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsXYPadObject)obj).setCurrentXMinValue( args.getInt(0));
	}
      });
    FtsObject.registerMessageHandler( FtsXYPadObject.class, FtsSymbol.get("setYMinValue"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsXYPadObject)obj).setCurrentYMinValue( args.getInt(0));
	}
      });
    FtsObject.registerMessageHandler( FtsXYPadObject.class, FtsSymbol.get("setXMaxValue"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsXYPadObject)obj).setCurrentXMaxValue( args.getInt(0));
	}
      });
    FtsObject.registerMessageHandler( FtsXYPadObject.class, FtsSymbol.get("setYMaxValue"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsXYPadObject)obj).setCurrentYMaxValue( args.getInt(0));
	}
      });
    FtsObject.registerMessageHandler( FtsXYPadObject.class, FtsSymbol.get("position"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsXYPadObject)obj).setCurrentPosition(args.getInt(0),args.getInt(1));
	}
      });
  }
  
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  int xMinValue;
  int xMaxValue;
  int yMinValue;
  int yMaxValue;
  int xValue;
  int yValue;

  protected transient FtsArgs args = new FtsArgs();
    
  /**
   * Create a FtsObject object;
   */
  public FtsXYPadObject(FtsServer server, FtsObject parent, int id, String className, FtsAtom args[], int offset, int length)
  {
    super(server, parent, id, className, args, offset, length);
    
    setNumberOfInlets(2);
    setNumberOfOutlets(2);
    
    xMaxValue = 127;
    xMinValue = 0;
    
    yMaxValue = 127;
    yMinValue = 0;
    
    xValue = 64;
    yValue = 64;
  }

  /** Set the Min Values for the pad.
    Tell the server.
    */

  public void setXMinValue(int value)
  {
    xMinValue = value;

    args.clear();
    args.addInt(value);
    
    try{
      send( FtsSymbol.get("setXMinValue"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsXYPadObject: I/O Error sending setXMinValue Message!");
	e.printStackTrace(); 
      }
  }
  
  public void setYMinValue(int value)
  {
    yMinValue = value;
    
    args.clear();
    args.addInt(value);
    
    try{
      send( FtsSymbol.get("setYMinValue"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsXYPadObject: I/O Error sending setYMinValue Message!");
	e.printStackTrace(); 
      }
  }

  void setCurrentXMinValue( int value)
  {
    xMinValue = value;
    ((XYPad)getObjectListener()).setCurrentXMinValue( value);
  }

  void setCurrentYMinValue( int value)
  {
    yMinValue = value;
    ((XYPad)getObjectListener()).setCurrentYMinValue( value);
  }
  
  /** Set the Max Values for the pad.
      Tell the server.
  */

  public void setXMaxValue(int value)
  {
    xMaxValue = value;
    
    args.clear();
    args.addInt(value);
    try{
      send( FtsSymbol.get("setXMaxValue"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsXYPadObject: I/O Error sending setXMaxValue Message!");
	e.printStackTrace(); 
      }
  }

  public void setYMaxValue(int value)
  {
    yMaxValue = value;
    
    args.clear();
    args.addInt(value);
    try{
      send( FtsSymbol.get("setYMaxValue"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsXYPadObject: I/O Error sending setYMaxValue Message!");
	e.printStackTrace(); 
      }
  }

  void setCurrentXMaxValue( int value)
  {
    xMaxValue = value;
    ((XYPad)getObjectListener()).setCurrentXMaxValue( value);
  }
  
  void setCurrentYMaxValue( int value)
  {
    yMaxValue = value;
    ((XYPad)getObjectListener()).setCurrentYMaxValue( value);
  }

  /** set the X and Y values and tell the server */

  public void setXYValues(int x, int y)
  {
    xValue = x;
    yValue = y;
    
    args.clear();
    args.addInt(x);
    args.addInt(y);
    
    try{send( FtsSymbol.get("position"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsXYPadObject: I/O Error sending position Message!");
	e.printStackTrace(); 
      }
  }

  void setCurrentPosition(int x, int  y)
  {
    xValue = x;
    yValue = y;
    ((XYPad)getObjectListener()).setCurrentXYValues( x, y);
  }
  
  /** Get the Min Values for the pad. */

  public int  getXMinValue()
  {
    return xMinValue;
  }

  public int  getYMinValue()
  {
    return yMinValue;
  }

  /** Get the Max Values for the pad. */

  public int getXMaxValue()
  {
    return xMaxValue;
  }

  public int getYMaxValue()
  {
    return yMaxValue;
  }
  
  public int getXValue()
  {
    return xValue;
  }

  public int getYValue()
  {
    return yValue;
  }
}






