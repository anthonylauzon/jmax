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

public class FtsBmaxRemoteDataHandler extends MaxDataHandler
{
  public FtsBmaxRemoteDataHandler()
  {
  }

  /** We can load from a file start with the "bmax" or the "mbxa" string (??) */

  public boolean canLoadFrom(MaxDataSource source)
  {
    if ((source instanceof MaxFileDataSource) && super.canLoadFrom(source))
      {
	File file = ((MaxFileDataSource) source).getFile();

	try
	  {
	    FileReader fr = new FileReader(file);
	    char buf[] = new char[4];
	    String code;


	    

	    fr.read(buf);
	    fr.close();
	    
	    code = new String(buf);

	    if (code.equals("bMax") || code.equals("Mbxa"))
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

    try
      {
	patcher = (FtsContainerObject) FtsObject.makeFtsObject(FtsServer.getServer().getRootObject(),
							       "patcher", "unnamed 0 0");

	// ask fts to load the file within this 
	// patcher, using a dedicated message

	FtsServer.getServer().loadPatcherBmax(patcher, file.getAbsolutePath());

	FtsPatchData obj = new FtsPatchData();

	obj.setPatcher(patcher);
	obj.setDataSource(source);
	obj.setDataHandler(this);

	return obj;
      }
    catch (FtsException e)
      {
	return null;
      }
  }

  public void saveInstance(MaxData instance) throws MaxDataException
  {
    if ((instance instanceof FtsPatchData) && (instance.getDataSource() instanceof MaxFileDataSource))
      {
	File file = ((MaxFileDataSource) instance.getDataSource()).getFile();

	FtsServer.getServer().savePatcherBmax((FtsObject) instance.getContent(), file.getAbsolutePath());
      }
    else
      throw new MaxDataException("Cannot save a " + instance.getDataType() + " as Bmax file");
  }

  public boolean canSaveTo(MaxDataSource source)
  {
    return (source instanceof MaxFileDataSource);
  }

  public boolean canSaveTo(MaxDataSource source, MaxData instance)
  {
    return ((instance instanceof FtsPatchData) && (source instanceof MaxFileDataSource));
  }
}





