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

  static void startFtsErrorStreamer(InputStream in)
  {
    (new Thread(new FtsErrorStreamer(in), "Error Streamer")).start();
  }

  FtsErrorStreamer(InputStream in)
  {
    this.in = in;
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





