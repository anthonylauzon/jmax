
/**
 * The Print Writer associated with the console
 *
 */

package ircam.jmax.editors.console;
import java.io.*;

class ConsoleWriter extends Writer {
  Console itsConsole;
  StringBuffer temp = new StringBuffer();

  public ConsoleWriter(Console theConsole){
    super();
    itsConsole = theConsole;
  }

  public void write(char cbuf[],
		    int off,
		    int len) throws IOException {				 
		      temp.setLength(0);
		      temp.append(cbuf, off, len);
		      itsConsole.text.append(temp.toString());
		      System.out.println("mi hanno chiesto di stampare"+temp.toString());
  }
  public void flush() {
  }
  public void close() {
  }  
}

  

