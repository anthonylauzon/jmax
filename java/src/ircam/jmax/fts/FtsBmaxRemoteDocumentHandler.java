package ircam.jmax.fts; 

import java.io.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/** An instance of this document handler can load MaxDocument from
 *  a  remote binary file.
 */

public class FtsBmaxRemoteDocumentHandler extends MaxDocumentHandler
{
  public FtsBmaxRemoteDocumentHandler()
  {
  }

  /** We can load from a file start with the "bmax" or the "mbxa" string (??) */

  public boolean canLoadFrom(File file)
  {
    if (super.canLoadFrom(file))
      {
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

  /** Make the real document */

  protected MaxDocument loadDocument(File file)
  {
    FtsServer server;
    FtsObject patcher;
    int id;
    
    // Load the environment file if needed 

    MaxEnv.loadEnvFileFor(file);

    server = Fts.getServer();
    id = server.getNewObjectId();

    // ask fts to load the file within this 
    // patcher, using a dedicated message

    server.loadPatcherBmax(server.getRootObject(), id, file.getAbsolutePath());
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
    if (document instanceof FtsPatcherDocument) 
      {
	Fts.getServer().savePatcherBmax((FtsObject) document.getRootData(), file.getAbsolutePath());
      }
    else
      throw new MaxDocumentException("Cannot save a " + document.getDocumentType() + " as Bmax file");
  }

  protected void saveSubDocument(MaxDocument document, MaxData data, File file) throws MaxDocumentException
  {
    if ((document instanceof FtsPatcherDocument) && (data instanceof FtsObject))
      {
	Fts.getServer().savePatcherBmax((FtsObject) data, file.getAbsolutePath());
      }
    else
      throw new MaxDocumentException("Cannot save a " + document.getDocumentType() + " as Bmax file");

  }

  public boolean canSaveTo(File file)
  {
    return true;
  }

  public boolean canSaveTo(MaxDocument document, File file)
  {
    return (document instanceof FtsPatcherDocument);
  }
}





