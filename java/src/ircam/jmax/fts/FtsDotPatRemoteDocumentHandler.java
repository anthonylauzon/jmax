package ircam.jmax.fts; 

import java.io.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/** An instance of this document handler can load MaxDocument from
 *  a bmax binary file.
 * Ask FTS to load the file, using the new
 * incremental Application Layer architecture.
 */

public class FtsDotPatRemoteDocumentHandler extends MaxDocumentHandler
{
  public FtsDotPatRemoteDocumentHandler()
  {
  }

  /** We can load from a file start with the "max v2" string*/

  public boolean canLoadFrom(File file)
  {
    if (super.canLoadFrom(file))
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
    else
      return false;
  }

  /** Make the real document */

  protected MaxDocument loadDocument(File file)
  {
    FtsServer server;
    FtsObject patcher;
    int id;

    // Load the environement file if needed

    MaxEnv.loadEnvFileFor(file);

    // Build an empty patcher son of root.

    server = Fts.getServer();
    id = server.getNewObjectId();

    // ask fts to load the file within this 
    // patcher, using a dedicated message

    server.loadPatcherDpat(server.getRootObject(), id, file.getAbsolutePath());
    server.sendDownloadObject(id);
    server.syncToFts();
    patcher = server.getObjectByFtsId(id);

    if (patcher != null)
      {
	FtsPatcherDocument obj = new FtsPatcherDocument();

	obj.setRootData((MaxData) patcher);
	obj.setDocumentFile(file);
	obj.setDocumentHandler(this);
	    
	return obj;
      }
    else
      return null;
  }

  public void saveDocument(MaxDocument document, File file) throws MaxDocumentException
  {
    throw new MaxDocumentException("Cannot save in .pat file format");
  }

  // Overwrite upper class method; we cannot save to a .pat file.

  public boolean canSaveTo(File file)
  {
    return false;
  }

  public boolean canSaveTo(MaxDocument document, File file)
  {
    return false;
  }
}





