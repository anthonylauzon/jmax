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

abstract public class MaxTclFileDataHandler extends MaxFileDataHandler
{
  String extension;
  
  protected MaxTclFileDataHandler(String extension)
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
}

