package ircam.jmax.mda; 

import java.io.*;
import tcl.lang.*;

import ircam.jmax.*;


/** An instance of this data handler can load MaxData from
 *  a tcl file obeyng the "jmax" command conventions
 * 
 * It can be instantiate with a filename extension, so that
 * that extension will be automatically reconized.
 */

public class MaxTclFileDataHandler extends MaxFileDataHandler
{
  String extension;
  
  public MaxTclFileDataHandler(String extension)
  {
    super();

    this.extension = extension;
  }

  /** We can load from a file, whose name end by extension */

  protected boolean canLoadFrom(File file)
  {
    return file.getName().endsWith(extension);
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
	PrintWriter pw = new PrintWriter(stream); // should be an "indent print writer"

	pw.println("jmax " + instance.getDataType().getName() +
		   " " + instance.getName() +
		   " " + instance.getInfo() +
		   "{");
	
	((MaxTclData) instance).saveContentAsTcl(pw);
	
	pw.println("}");
      }
    catch (IOException e)
      {
	throw new MaxDataException("I/O error saving " + instance + " to " + file);
      }
  }
}

