package ircam.jmax.mda; 

import java.io.*;
import tcl.lang.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;


/** An instance of this data handler can load MaxData from
 *  a tcl file obeyng the "jmax" command conventions
 * 
 * A TCL Data file is a file that start with the jmax command
 * at the *beginning* of the first line.
 */

public class MaxTclFileDataHandler extends MaxFileDataHandler
{
  public MaxTclFileDataHandler()
  {
    super();
  }

  /** We can load from a file start with the "jmax " string*/

  protected boolean canLoadFrom(File file)
  {
    try
      {
	FileReader fr = new FileReader(file);

	char buf[] = new char[5];
    
	fr.read(buf);
	fr.close();

	if ((new String(buf)).equals("jmax "))
	  return true;
	else
	  return false;
      }
    catch (FileNotFoundException e)
      {
	return false;
      }
    catch (IOException e)
      {
	return false;
      }
  }

  /** Make the real instance */

  protected MaxData makeInstance(File file) throws MaxDataException
  {
    Interp interp = MaxApplication.getTclInterp();

    try
      {
	interp.evalFile(file.getPath());
      }
    catch (TclException e)
      {
	throw new MaxDataException("Tcl error: " + interp.getResult());
      }

    try
      {
	return (MaxData) ReflectObject.get(interp, interp.getResult());
      }
    catch (TclException e)
      {
	throw new MaxDataException("Format error loading file " + file);
      }
  }

  /** Save: basic support for saving a tcl file in the "jmax" format 
   * jmax <doc-type> [<version>] <name> <info> { <doc-body> }
   * It use a "IndentOutputStream", but the data method should make
   * sure that they can save to a normal stream, also.
   * This version do not save a version number.
   */

  public void saveInstance(MaxData instance) throws MaxDataException
  {
    /* Open the stream, put the "jmax" header, and then save
       the patcher inside
       */

    File file = ((MaxFileDataSource) instance.getDataSource()).getFile();

    try
      {
	FileOutputStream stream  = new 	FileOutputStream(file);
	IndentedPrintWriter pw = new IndentedPrintWriter(stream); // should be an "indent print writer"

	pw.println("jmax " + instance.getDataType().getName() +
		   " " + instance.getName() +
		   " {" + instance.getInfo() +
		   "} {");
	
	pw.indentMore();
	((MaxTclData) instance).saveContentAsTcl(pw);
	pw.indentLess();
	pw.println();
	pw.println("}");
	pw.close();
      }
    catch (IOException e)
      {
	throw new MaxDataException("I/O error saving " + instance + " to " + file);
      }
  }
}

