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

package ircam.jmax.ispw;

import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;

/**
 * Class implementing the proxy of a message box.
 * 
 * If the listener of this object is an instance
 * of FtsMessageListener, fire it when the we got a new message content
 * from the server.
 */

public class FtsMessageObject extends FtsIntValueObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  String message; // the message content
  
  public FtsMessageObject(Fts fts, FtsObject parent, String variable, String className, int nArgs, FtsAtom args[])
    {
	super(fts, parent, "messbox",  FtsMessageObject.preParseMessage(FtsParse.unparseArguments(nArgs, args)));
    
	ninlets = 1;
	noutlets = 1;
    
	message =  FtsMessageObject.preParseMessage(FtsParse.unparseArguments(nArgs, args));
  }

  /** Set the message content. Tell the server, too */

  public void setMessage(String message)
  {
    this.message = message;
    getFts().getServer().sendSetMessage(this, message);
    setDirty();
  }

  /** Get the message content. */

  public String getMessage()
  {
    return message;
  }
       
  /** Over write the handle message to handle message box changes. */

  public void handleMessage(String selector, int nArgs, FtsAtom args[])
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    this.message = FtsMessageObject.preParseMessage(FtsParse.unparseArguments(nArgs, args));
    setDirty();

    if (listener instanceof FtsMessageListener)
      ((FtsMessageListener) listener).messageChanged(message);
  }

  private static String preParseMessage(String text)
  {
      int index = text.indexOf(';', 0);
      int size = text.length();
      while(index >= 0)
      { 
	  if(index<size-2)
	      {
		  if(!text.substring(index+1, index+2).equals("\n"))
		      if(text.substring(index+1, index+2).equals(" "))
			  text = text.substring(0, index+1)+"\n"+text.substring(index+2);
		      else
			  text = text.substring(0, index+1)+"\n"+text.substring(index+1);
	      }
	  else
	      {
		  if((index == size-2)&&(text.endsWith(" ")))
		      text = text.substring(0, index);
	      }
	  
	  index = text.indexOf(';', index+1);
      }
      return text;
  }
}




