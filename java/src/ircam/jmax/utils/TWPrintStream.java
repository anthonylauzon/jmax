package ircam.jmax.utils;
import java.awt.*;
import java.io.*;

/** TW PrintStream: a simple PrintStream associated with TextWindows*/

/* Obsolete class: Print Stream is already a filter (by the way, deprecated
   in 1.1, we should use PrintWriter), so we don't need to specialize
   it, just to pass our OutputStream at creation time
   */

public class TWPrintStream extends PrintStream {
	TWOutputStream itsOutputStream;
	
	public TWPrintStream(TWOutputStream theOutputStream) {
		super(theOutputStream);
		itsOutputStream = theOutputStream;
	}
	
	public void println(String s) {
		itsOutputStream.write(s);
		itsOutputStream.write('\n');
	}
	
	//etc...
}
