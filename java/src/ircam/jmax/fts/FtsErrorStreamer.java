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
  FtsServer server;
  InputStream in;
  TextWindow window = null;
  PrintStream out = null;

  static void startFtsErrorStreamer(InputStream in, FtsServer server)
  {
    Thread streamer;

    streamer = new Thread(new FtsErrorStreamer(in, server), "Error Streamer");
    streamer.setDaemon(true);
    streamer.start();
  }

  FtsErrorStreamer(InputStream in, FtsServer server)
  {
    this.in = in;
    this.server = server;
  }

  public void run()
  {
    boolean running = true;

    while (running)
      {
	try
	  {
	    int c;

	    c = in.read();

	    if (out == null)
	      {
		window = new TextWindow("FTS Standard Error");
		MaxWindowManager.getWindowManager().addWindow(window);
		out = window.getPrintStream();
		window.pack();
		window.show();
	      }


	    if (c == -1)
	      {
		running = false;
		server.ftsQuitted();
		out.println("FTS quitted/crashed !!!\n");
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





