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

import ircam.fts.client.*;
import ircam.jmax.*;
import ircam.jmax.editors.console.*;

/** Java class for the dsp control remote data class.
 *
 * It offer visibility and control over a number of DSP related
 * server parameter and status.
 * It is implemented as a Java Beans, with Java Beans Properties 
 * corresponding to DSP server properties.
 */

public class FtsConsole extends FtsObject
{
  static
  {
    FtsObject.registerMessageHandler( FtsConsole.class, FtsSymbol.get("print_line"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ConsoleWindow.append( args.getString(0)); 
	}
      });
  }
    
  public FtsConsole(FtsArgs args) throws IOException
  {
    super(MaxApplication.getServer(), MaxApplication.getServer().getRoot(), FtsSymbol.get("console_stream"), args);
  }
}











