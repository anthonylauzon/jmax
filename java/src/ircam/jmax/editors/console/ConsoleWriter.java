//
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

/**
 * The Print Writer associated with the console
 * Changed to OutputStream by MDC, in order to work
 * with the System.out.
 */

package ircam.jmax.editors.console;
import java.io.*;

class ConsoleWriter extends OutputStream
{
  Console itsConsole;
  StringBuffer buffer = new StringBuffer();

  public ConsoleWriter(Console theConsole)
  {
    super();
    itsConsole = theConsole;
  }

  public void write(int b) 
  {
    buffer.append((char)b);

    if (b == '\n')
      flush();
  }

  /*  public void write(char cbuf[],
		    int off,
		    int len) throws IOException
  {
    flush();
    itsConsole.getTextArea().append(new String(cbuf));
  }
  */

  public void flush()
  {
    itsConsole.Put(buffer.toString());
    buffer.setLength(0);
    itsConsole.itsTextArea.setCaretPosition(itsConsole.itsTextArea.getText().length());
  }

  public void close()
  {
  }  
}

  

