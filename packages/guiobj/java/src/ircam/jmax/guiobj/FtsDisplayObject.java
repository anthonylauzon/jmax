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

import java.io.*;
import java.util.*;
import java.text.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.fts.client.*;

public class FtsDisplayObject extends FtsGraphicObject
{
  static
  {
    FtsObject.registerMessageHandler( FtsDisplayObject.class, FtsSymbol.get("set"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsDisplayObject)obj).display(args.getString(0));
	}
      });
  }

  public FtsDisplayObject(FtsServer server, FtsObject parent, int id, FtsAtom args[], int offset, int length)
  {
    super(server, parent, id, args, offset, length);
    
    ninlets = 1;
    noutlets = 0;
  }

  public void display(String content)
  {
    if (listener instanceof FtsMessageListener)
      ((FtsMessageListener) listener).messageChanged(content);
  }
}





