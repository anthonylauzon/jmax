package ircam.jmax.utils;
import java.awt.*;
import java.io.*;

/** 
 *A simple OutputStream associated with the TextWindow 
*/

public class TWOutputStream extends OutputStream
{
  TextWindow itsTextWindow;
  StringBuffer buffer = new StringBuffer(256);
  int count = 0;
	
  public TWOutputStream(TextWindow theTextWindow)
  {
    super();
    itsTextWindow = theTextWindow;
  }
	
  public void write(int b) /*throws IOException*/
  {
    buffer.append((char)b);
    count++;

    if ((b == '\n') || (count > 1024))
      {
	itsTextWindow.itsTextArea.appendText(buffer.toString());
	buffer.setLength(0);
	count = 0;
      }
  }

  public void write(String s)
  {
    itsTextWindow.itsTextArea.appendText(s);
  }
}
