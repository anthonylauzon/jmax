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


package ircam.jmax.fts;

import java.io.*;
import java.util.*;

/**
 * This class is used to show the FTS standard error by listening on
 * the FTS process error stream.<BR>
 * When instantiated with the startFtsErrorStream static method,
 * it listen an input stream, and writes the result in a file name .fts-stderr
 * located in user's home directory
 */

class FtsErrorStreamer implements Runnable
{
  private InputStream in;
  private PrintStream out;
  private boolean running;
  private boolean first = true;
  private String fileName;

  private static FtsErrorStreamer errorStreamer;
  
  static void startFtsErrorStreamer(InputStream in)
  {  
    errorStreamer = new FtsErrorStreamer(in);
  }

  static void stopFtsErrorStreamer()
  {
    errorStreamer.running = false;
  }

  FtsErrorStreamer( InputStream in)
  {
    this.in = in;
    running = true;

    Properties p = System.getProperties();
    OutputStream f = null;
    fileName =  p.getProperty( "user.home") + p.getProperty( "file.separator") + ".fts-stderr";
    try
      {
	f = new FileOutputStream( fileName);
      }
    catch ( FileNotFoundException e)
      {
	return;
      }

    out = new PrintStream( f);

    out.println("*** Fts Standard Error ***");
    out.println("");

    Thread t = new Thread( this, "Error Streamer");
    t.setDaemon(true);
    t.start();
  }

  public void run()
  {
    while (running)
      {
	try
	  {
	    int c = in.read();

	    if ((c == -1) && (! running))
	      return;

	    if (c == -1)
	      {
		running = false;
		out.println("FTS crashed.\n");

		System.out.println( "FTS crashed\n");
	      }
	    else
	      {
		  if(first)
		      {
			  System.out.println("error log written to "+fileName);

			  first = false;
		      }
		out.write(c);
	      }
	  }
	catch (IOException e)
	  {
	    out.println("Exception in FTS I/O, giving up  !!!\n");

	    running = false;
	  }
      }
  }
}





