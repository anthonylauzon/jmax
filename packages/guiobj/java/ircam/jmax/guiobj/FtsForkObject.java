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

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.fts.client.*;

public class FtsForkObject extends FtsGraphicObject
{
    public FtsForkObject(FtsServer server, FtsObject parent, int id, FtsAtom args[], int offset, int length)
    {
	super(server, parent, id, args, offset, length);
	setNumberOfInlets(1);
	setNumberOfOutlets(2);
    }

    public void setDefaults()
    {
	setWidth(Fork.DEFAULT_WIDTH);
	setHeight(Fork.CONST_HEIGHT);
    }
    
    public void requestSetOutlets(int nOuts)
    {
	args.clear();
	args.addInt(nOuts);
	
	try{
	    send( FtsSymbol.get("set_outlets"), args);
	}
	catch(IOException e)
	    {
		System.err.println("FtsForkObject: I/O Error sending set_outlets Message!");
		e.printStackTrace(); 
	    }  
    }
}
