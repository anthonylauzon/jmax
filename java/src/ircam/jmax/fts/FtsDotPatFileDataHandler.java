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

  /** We can load from a file start with the "max v2" string*/

  protected boolean canLoadFrom(File file)
  {
    try
      {
	FileReader fr = new FileReader(file);

	char buf[] = new char[6];
    
	fr.read(buf);
	fr.close();

	if ((new String(buf)).equals("max v2"))
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





