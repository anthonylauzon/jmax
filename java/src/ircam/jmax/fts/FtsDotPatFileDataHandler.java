package ircam.jmax.fts; 

import java.io.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/** An instance of this data handler can load MaxData from
 *  a tcl source obeyng the "jmax" command conventions.
 *  Currently, implemented only for files, not URL.
 * It can be instantiate with a filename extension, so that
 * that extension will be automatically reconized.
 */

public class FtsDotPatFileDataHandler extends MaxDataHandler
{
  public FtsDotPatFileDataHandler()
  {
  }

  /** Check for .pat  files */

  /** We can load from a file start with the "max v2" string*/

  public boolean canLoadFrom(MaxDataSource source)
  {
    if ((source instanceof MaxFileDataSource) && super.canLoadFrom(source))
      {
	File file = ((MaxFileDataSource) source).getFile();

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
    else
      return false;
  }

  /** Make the real instance */

  protected MaxData loadInstance(MaxDataSource source) throws MaxDataException
  {
    File file = ((MaxFileDataSource) source).getFile();
    FtsContainerObject patcher;

    try
      {
	FtsServer.getServer().setFlushing(false);
	patcher = FtsDotPatParser.importPatcher(FtsServer.getServer(), file);
	FtsServer.getServer().setFlushing(true);
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
    obj.setDataSource(source);
    obj.setDataHandler(this);

    return obj;
  }

  public void saveInstance(MaxData instance) throws MaxDataException
  {
    throw new MaxDataException("Cannot save in .pat file format");
  }

  // Overwrite upper class method; we cannot save to a .pat file.

  public boolean canSaveTo(MaxDataSource source)
  {
    return false;
  }

  public boolean canSaveTo(MaxDataSource source, MaxData instance)
  {
    return false;
  }
}





