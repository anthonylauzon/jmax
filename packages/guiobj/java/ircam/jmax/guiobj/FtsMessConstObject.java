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

import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.fts.client.*;

public class FtsMessConstObject extends FtsIntValueObject
{
  private String message;

  public FtsMessConstObject(FtsServer fts, FtsObject parent, int id, String className, FtsAtom args[], int offset, int length)
  {
    super(fts, parent, id, className, args[offset].stringValue.equals(className) ? "" : args[offset].stringValue);
    setNumberOfInlets(1);
    setNumberOfOutlets(1);

    message = getDescription();
  }

  public void setMessage( String message)
  {
    this.message = message;

    args.clear();
    args.addRawString( message);
    
    try
      {
	send(FtsSymbol.get("set"), args);
      }
    catch( IOException e)
      {
	System.err.println("FtsMessageObject: I/O Error sending set Message!");
	e.printStackTrace(); 
      }
  }

  public String getMessage()
  {
    return message;
  }
}





