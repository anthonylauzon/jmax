package ircam.jmax.fts; 

import java.io.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/** An instance of this document handler can load MaxDocument from
 *  a tcl source obeyng the "jmax" command conventions.
 * This version actually ask FTS to load the file, using the new
 * incremental Application Layer architecture; the result
 * is a speed improvement of a factor of 40 (minimum).
 */

public class FtsBmaxRemoteDocumentHandler extends MaxDocumentHandler
{
  public FtsBmaxRemoteDocumentHandler()
  {
  }

  /** We can load from a file start with the "bmax" or the "mbxa" string (??) */

  public boolean canLoadFrom(MaxDocumentSource source)
  {
    if ((source instanceof MaxFileDocumentSource) && super.canLoadFrom(source))
      {
	File file = ((MaxFileDocumentSource) source).getFile();

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

  protected MaxDocument loadDocument(MaxDocumentSource source)
  {
    File file = ((MaxFileDocumentSource) source).getFile();
    FtsContainerObject patcher;

    // Build an empty patcher son of root.

    try
      {
	patcher = (FtsContainerObject) FtsObject.makeFtsObject(FtsServer.getServer().getRootObject(),
							       "patcher", "unnamed 0 0");

	// ask fts to load the file within this 
	// patcher, using a dedicated message

	FtsServer.getServer().loadPatcherBmax(patcher, file.getAbsolutePath());

	FtsPatcherDocument obj = new FtsPatcherDocument();

	obj.setRootData(patcher);
	obj.setDocumentSource(source);
	obj.setDocumentHandler(this);

	return obj;
      }
    catch (FtsException e)
      {
	return null;
      }
  }

  public void saveDocument(MaxDocument document, MaxDocumentSource source) throws MaxDocumentException
  {
    if ((document instanceof FtsPatcherDocument) && (source instanceof MaxFileDocumentSource))
      {
	File file = ((MaxFileDocumentSource) source).getFile();

	FtsServer.getServer().savePatcherBmax((FtsObject) document.getRootData(), file.getAbsolutePath());
      }
    else
      throw new MaxDocumentException("Cannot save a " + document.getDocumentType() + " as Bmax file");
  }

  public boolean canSaveTo(MaxDocumentSource source)
  {
    return (source instanceof MaxFileDocumentSource);
  }

  public boolean canSaveTo(MaxDocument document, MaxDocumentSource source)
  {
    return ((document instanceof FtsPatcherDocument) && (source instanceof MaxFileDocumentSource));
  }
}





