//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
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

  

