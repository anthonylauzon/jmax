/*
 * MaxSourceCmd.java
 */

package ircam.jmax.tcl;

import tcl.lang.*;
import java.io.*;

import ircam.jmax.*;

/*
 * This class implements  a version of the built-in "source"  that do not
 * recursively halt the sourcing of file in case of an error, but just print
 * the error and return.
 * Also, this version provide a number of predefined local variables loading the file
 * (currently, $dir, that point to the current directory)
 * It work together with the _sysSourceFile command and the _doSourceFile tcl function
 * from the intrinsics.tcl file.
 */

class MaxSourceCmd implements Command
{
  public void cmdProc(Interp interp, TclObject argv[])  throws   TclException
  {
    if (argv.length == 2)
      {
	File file;
	String fileName;
	String dirName;

	fileName = argv[1].toString();

	file = new File(fileName);

	if (file.isAbsolute())
	  dirName = file.getParent();
	else  if (file.getParent() == null)
	  dirName = MaxApplication.getProperty("user.dir");
	else
	  dirName = ( MaxApplication.getProperty("user.dir") + 
		      MaxApplication.getProperty("file.separator") +
		      file.getParent());

	if (dirName != null)
	  interp.eval("_doSourceFile " + dirName + " " + fileName);
	else
	  interp.eval("_doSourceFile {} " + fileName);
      }
    else
      throw new TclNumArgsException(interp, 1, argv, "fileName");
  }
}




