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
// Authors: Riccardo Borghesi, Francois Dechelle, Norbert Schnell.
// 

package ircam.jmax.guiobj;

import java.awt.*;
import java.io.*;
import java.util.*;
import java.text.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;

public class FtsScopeObject extends FtsVectorDisplayObject
{
  public static float DEFAULT_PERIOD = (float)100.0;
  public static float DEFAULT_THRESHOLD = (float)0.5;
  float period    = DEFAULT_PERIOD;
  float threshold = DEFAULT_THRESHOLD;
  
  public final static float THRESHOLD_AUTO = (float)0.0; 
  public final static float THRESHOLD_OFF  = (float)1.0; 

  public FtsScopeObject(Fts fts, FtsObject parent, String variable, String className, int nArgs, FtsAtom args[])
  {
    super(fts, parent, variable, className, nArgs, args);
    
    min = (float)-1.0;
    max = (float)1.0;
  }

  public void setOnset(int n)
  {
    sendArgs[0].setInt(n); 
    sendMessage(FtsObject.systemInlet, "onset", 1, sendArgs);
  }   
  
  public void requestSetPeriod(float period)
  {
    sendArgs[0].setFloat(period); 
    sendMessage(FtsObject.systemInlet, "setPeriod", 1, sendArgs);
  }    
  
  public void setPeriod(int nArgs , FtsAtom args[])
  {      
    period = args[0].getFloat();    
  } 
  
  public float getPeriod()
  {
    return period;
  }  
  public void requestSetThreshold(float th)
  {
      if(th==THRESHOLD_AUTO)
	  sendArgs[0].setString("auto"); 
      else if(th==THRESHOLD_OFF)
	  sendArgs[0].setString("off"); 
      else
	  sendArgs[0].setFloat(th);
      
      sendMessage(FtsObject.systemInlet, "setThreshold", 1, sendArgs);  
  } 
  
  public void setThreshold(int nArgs , FtsAtom args[])
  {      
      if(args[0].isString())
	  {
	      String tType = args[0].getString();
	      if(tType.equals("auto"))
		  this.threshold = THRESHOLD_AUTO;
	      else
		  this.threshold = THRESHOLD_OFF;
	  }
      else
	  this.threshold = args[0].getFloat();    
  } 
  
  public float getThreshold()
  {
    return threshold;
  }  
}