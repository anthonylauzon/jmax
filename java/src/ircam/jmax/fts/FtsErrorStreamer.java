//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.fts;

import java.io.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;

/**
 * This class implement a listener on the stream. <br>
 * When instantiated with the startFtsErrorStream static method,
 * it listen an input stream, and pop up a window where the input 
 * stream is copied.<br>
 * The window is created only when at least one character has been
 * read; this class is used to show the FTS standard error. <p>
 * <b>bug:</b> The window use a constant title that prevent its
 * use in other situation; it should also go in a generic library.
 */

class FtsErrorStreamer implements Runnable
{
  InputStream in;
  TextWindow window = null;
  PrintStream out = null;
  boolean running = true;
  static FtsErrorStreamer errorStreamer;

  static void startFtsErrorStreamer(InputStream in)
  {  
    Thread streamer;

    errorStreamer = new FtsErrorStreamer(in);
    streamer = new Thread(errorStreamer, "Error Streamer");
    streamer.setDaemon(true);
    streamer.start();
  }

  static void stopFtsErrorStreamer()
  {
    errorStreamer.running = false;
  }

  FtsErrorStreamer(InputStream in)
  {
    this.in = in;
  }

  public void run()
  {
    while (running)
      {
	try
	  {
	    int c;

	    c = in.read();

	    if ((c == -1) && (! running))
	      return;

	    if (out == null)
	      {
		window = new TextWindow("FTS Standard Error");
		MaxWindowManager.getWindowManager().addWindow(window);
		out = window.getPrintStream();
		window.pack();
		window.show();

		out.println("Output to this window come from the Fts Standard Error");
		out.println("In general, it means that you just discovered a jMax bug");
		out.println("Please, submit a bug report describing");
		out.println("the situation that generated this output");
		out.println("For information on how submit a bug for jMax");
		out.println("please look at http://www.ircam.fr/Bugs/");
		out.println("");
	      }

	    if (c == -1)
	      {
		running = false;
		out.println("FTS crashed.\n");
	      }
	    else
	      {
		out.write(c);
	      }
	  }
	catch (IOException e)
	  {
	    if (out == null)
	      {
		window = new TextWindow("FTS Standard Error");

		out = window.getPrintStream();
		window.pack();
		window.show();
	      }

	    out.println("Exception in FTS I/O, giving up  !!!\n");

	    running = false;
	  }
      }
  }
}





