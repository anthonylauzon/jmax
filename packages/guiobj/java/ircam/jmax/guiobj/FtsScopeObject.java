//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.guiobj;

import java.awt.*;
import java.io.*;
import java.util.*;
import java.text.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.fts.client.*;

public class FtsScopeObject extends FtsVectorDisplayObject
{
  static
  {
    FtsObject.registerMessageHandler(FtsScopeObject.class, FtsSymbol.get("setThreshold"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsScopeObject)obj).setCurrentThreshold(args.getAtoms()[0]);
	}
      });
    FtsObject.registerMessageHandler(FtsScopeObject.class, FtsSymbol.get("setPeriod"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsScopeObject)obj).setCurrentPeriod((float)args.getDouble(0));
	}
      });
  }

  public static float DEFAULT_PERIOD = (float)100.0;
  public static float DEFAULT_THRESHOLD = (float)0.5;
  float period    = DEFAULT_PERIOD;
  float threshold = DEFAULT_THRESHOLD;
  
  public final static float THRESHOLD_AUTO = (float)0.0; 
  public final static float THRESHOLD_OFF  = (float)1.0; 

  public FtsScopeObject(FtsServer server, FtsObject parent, int id, String className, FtsAtom args[], int offset, int length)
  {
    super(server, parent, id, className, args, offset, length);
    
    min = (float)-1.0;
    max = (float)1.0;
  }

  public void setOnset(int n)
  {
    args.clear();
    args.addInt(n);

    try{
      send( FtsSymbol.get("onset"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsScopeObject: I/O Error sending onset Message!");
	e.printStackTrace(); 
      }
  }   
  
  public void setPeriod(float period)
  {
    if(this.period == period) return;

    args.clear();
    args.addDouble((double)period);

    try{
      send( FtsSymbol.get("setPeriod"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsScopeObject: I/O Error sending period Message!");
	e.printStackTrace(); 
      }
  }    
  
  public void setCurrentPeriod(float period)
  {      
    this.period = period;    
  } 
  
  public float getPeriod()
  {
    return period;
  }  
  public void setThreshold(float th)
  {
    if(this.threshold == th) return;

    args.clear();

    if(th==THRESHOLD_AUTO)
      args.addSymbol(FtsSymbol.get( "auto"));
    else if(th==THRESHOLD_OFF)
      args.addSymbol(FtsSymbol.get( "off"));
    else
      args.addDouble((double)th);

    try{
      send(FtsSymbol.get("setThreshold"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsScopeObject: I/O Error sending setThreshold Message!");
	e.printStackTrace(); 
      }
  } 
  
  public void setCurrentThreshold(FtsAtom arg)
  {      
    if(arg.isSymbol())
      {
	String tType = arg.symbolValue.toString();
	if(tType.equals("auto"))
	  this.threshold = THRESHOLD_AUTO;
	else	  
	  this.threshold = THRESHOLD_OFF;
      }
    else
      this.threshold = (float)arg.doubleValue;
  }
  
  public float getThreshold()
  {
    return threshold;
  }  
}
