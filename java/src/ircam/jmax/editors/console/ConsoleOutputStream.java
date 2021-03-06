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

package ircam.jmax.editors.console;

import java.io.*;
import ircam.jmax.widgets.ConsoleArea;

/**
 * The console output stream, used to construct a PrintStream
 * that can be used as System.out.
 */

public class ConsoleOutputStream extends OutputStream {
  private ConsoleArea consoleArea;
  private StringBuffer buffer;
  private int prev;

  public ConsoleOutputStream( ConsoleArea consoleArea)
  {
    this.consoleArea = consoleArea;
    buffer = new StringBuffer();
  }

  private void appendToConsole()
  {
    consoleArea.append( buffer.toString());
    buffer.setLength(0);
  }

  public void write( int b) 
  {
    /* We have to handle the "\r\n" sequence on the windows
       systems. */
    if (b == '\r')
      appendToConsole();
    else if (b == '\n') 
      {
	if (prev != '\r') 
	  {
	    appendToConsole();
	  }
      } 
    else
      buffer.append( (char)b );
      
    prev = b;
  }

  public void flush()
  {
  }

  public void close()
  {
  }  
}

  

