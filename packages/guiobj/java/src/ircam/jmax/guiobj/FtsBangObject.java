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
 * A generic FTS object with an int value.
 * Used for intbox and sliders, for example.
 * If the listener of this object is an instance
 * of FtsIntValueListener, fire it when the we got a new value
 * from the server.
 */

public class FtsBangObject extends FtsIntValueObject
{
    static
    {
	ircam.ftsclient.FtsObject.registerMessageHandler( FtsBangObject.class, FtsSymbol.get("setFlash"), new FtsMessageHandler(){
		public void invoke( ircam.ftsclient.FtsObject obj, int argc, ircam.ftsclient.FtsAtom[] argv)
		{
		    ((FtsBangObject)obj).setFlash(argv[0].intValue);
		}
	    });
    }

  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  int flashDuration;
  protected FtsArgs args = new FtsArgs();

  /* for the message box */
  public FtsBangObject(FtsServer server, FtsObject parent, FtsSymbol className, int nArgs, FtsAtom args[], int id) 
  {
      super( server, parent, className, nArgs, args, id);
  }

    public void setDefaults()
    {
	setWidth(Bang.DEFAULT_WIDTH);
	setHeight(Bang.DEFAULT_WIDTH);
    }

    public void setFlashDuration(int fd)
    {
	flashDuration = fd;
	args.clear();
	args.add(fd);
	try{
	    send( FtsSymbol.get("setFlashDuration"), args);
	}
	catch(IOException e)
	    {
		System.err.println("FtsBangObject: I/O Error sending setFlashDuration Message!");
		e.printStackTrace(); 
	    }
	
	setDirty();
    }

    public int getFlashDuration()
    {
	return flashDuration;
    }

    /* Over write the localPut message to handle value changes;
     */

    protected void setFlash(int newValue)
    {
	flashDuration = newValue;
    }    
}






