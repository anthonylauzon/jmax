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

package ircam.jmax.fts;

import java.io.*;
import java.util.*;
import java.text.*;

import ircam.jmax.*;

public class FtsVectorDisplayObject extends FtsObject
{
  public static final int MAX_SIZE = 1024;
  private static final float defaultMin = (float)-1.0;
  private static final float defaultMax = (float)1.0;

  int size = 0;
  int range = 0;
  int values[] = new int[MAX_SIZE];
  int nValues = 0;
  float min = defaultMin;
  float max = defaultMax;
  int zero = 0; /* y position of zero axis */

  public FtsVectorDisplayObject(Fts fts, FtsObject parent, String className)
  {
    super(fts, parent, null, className, "");
    
    ninlets = 1;
    noutlets = 0;
  }

  public void computeZero()
  {
    if(min < (float)(0.0))
      zero = (int)((float)(range - 1) * min / (min - max) + (float)0.5);
    else
      zero = 0;
  }

  public int getSize()
  {
    return size;
  }  

  public void setSize(int n)
  {
    nValues = 0;

    size = n;

    sendArgs[0].setInt(n); 
    sendMessage(FtsObject.systemInlet, "size", 1, sendArgs);
  }  

  public int getRange()
  {
    return range;
  }  

  public void setRange(int n)
  {
    nValues = 0;

    range = n;

    sendArgs[0].setInt(n); 
    sendMessage(FtsObject.systemInlet, "range", 1, sendArgs);

    computeZero();
  }  

  public int getZero()
  {
    return zero;
  }  

  public int getNValues()
  {
    return nValues;
  }  

  public int[] getValues()
  {
    return values;
  }  

  public void display(int nArgs, FtsAtom args[])
  {
    int i;

    for(i=0; i<nArgs; i++)
      values[i] = args[i].getInt();
    
    nValues = nArgs;

    ((FtsDisplayListener) listener).display();
  }

  public void bounds(int nArgs, FtsAtom args[])
  {
    nValues = 0;

    min = args[0].getFloat();
    max = args[1].getFloat();

    computeZero();

    if(listener != null)
      ((FtsDisplayListener) listener).display();
  }
    
  /* argument vector for sending messages to server */
  public final static int NUM_ARGS = 2;
  public static FtsAtom[] sendArgs = new FtsAtom[NUM_ARGS];
  
  static
  {
    for(int i=0; i<NUM_ARGS; i++)
      sendArgs[i]= new FtsAtom();
  }
}
