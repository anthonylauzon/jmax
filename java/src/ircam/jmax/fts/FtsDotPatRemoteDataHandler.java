package ircam.jmax.fts; 

import java.io.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/** An instance of this data handler can load MaxData from
 *  a tcl source obeyng the "jmax" command conventions.
 * This version actually ask FTS to load the file, using the new
 * incremental Application Layer architecture; the result
 * is a speed improvement of a factor of 40 (minimum).
 */

public class FtsDotPatRemoteDataHandler extends MaxDataHandler
{
  public FtsDotPatRemoteDataHandler()
  {
  }

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

  protected MaxData loadInstance(MaxDataSource source)
  {
    File file = ((MaxFileDataSource) source).getFile();
    FtsContainerObject patcher;

    // Build an empty patcher son of root.

    patcher = new FtsPatcherObject(FtsServer.getServer().getRootObject(), false);

    // ask fts to load the file within this 
    // patcher, using a dedicated message

    FtsServer.getServer().loadPatcherDpat(patcher, file.getAbsolutePath());

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





