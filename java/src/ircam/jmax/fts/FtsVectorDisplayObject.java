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
  int size;

  public FtsVectorDisplayObject(Fts fts, FtsObject parent)
  {
    super(fts, parent, null, "vecdisplay", "");
    
    ninlets = 1;
    noutlets = 0;
  }

  public int getSize()
  {
    return size;
  }  

  public void setSize(int n)
  {
    sendArgs[0].setInt(n); 
    sendMessage(FtsObject.systemInlet, "size", 1, sendArgs);
  }  

  public void setRange(int n)
  {
    sendArgs[0].setInt(n); 
    sendMessage(FtsObject.systemInlet, "range", 1, sendArgs);
  }  

  public void display(int nArgs, FtsAtom args[])
  {
    ((FtsDisplayListener) listener).display(nArgs, args);
  }

  public void bounds(int nArgs, FtsAtom args[])
  {
    float min = args[0].getFloat();
    float max = args[1].getFloat();

    ((FtsDisplayListener) listener).bounds(min, max);
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





