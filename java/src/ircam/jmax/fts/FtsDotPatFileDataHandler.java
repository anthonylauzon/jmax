package ircam.jmax.fts; 

import java.io.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/** An instance of this data handler can load MaxData from
 *  a tcl file obeyng the "jmax" command conventions
 * 
 * It can be instantiate with a filename extension, so that
 * that extension will be automatically reconized.
 */

public class FtsDotPatFileDataHandler extends MaxFileDataHandler
{
  /** Check for .pat  files */

  protected boolean canLoadFrom(File file)
  {
    return file.getName().endsWith(".pat");
  }

  /** Make the real instance */

  protected MaxData makeInstance(File file) throws MaxDataException
  {
    FtsObject patcher;

    try
      {
	patcher = FtsDotPatParser.importPatcher(MaxApplication.getFtsServer(), file);
      }
    catch (java.io.IOException e)
      {
	throw new MaxDataException("I/O error loading from " + file);
      }
    catch (FtsException e)
      {
	throw new MaxDataException("Parsing error " + e + " loading " + file);
      }

    FtsPatchData obj = new FtsPatchData();

    obj.setPatcher(patcher);

    return obj;
  }

  public void saveInstance(MaxData instance) throws MaxDataException
  {
    throw new MaxDataException("Cannot save in .pat file format");
  }

  protected boolean canSaveTo(File file)
  {
    return false;
  }
}





