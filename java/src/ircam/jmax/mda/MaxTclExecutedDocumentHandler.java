package ircam.jmax.mda; 

import java.io.*;
import tcl.lang.*;

import ircam.jmax.*;


/** An instance of this document handler can load MaxDocument from
 *  a tcl file obeyng the "jmax" command conventions
 * Now support only files.
 * 
 * A TCL Document file is a file that start with the jmax command
 * at the *beginning* of the first line.
 */

public class MaxTclExecutedDocumentHandler extends MaxDocumentHandler
{
  public MaxTclExecutedDocumentHandler()
  {
    super();
  }

  /** We can load from a file whose name end with .tcl */

  public boolean canLoadFrom(MaxDocumentSource source)
  {
    if ((source instanceof MaxFileDocumentSource) && super.canLoadFrom(source))
      {
	File file = ((MaxFileDocumentSource) source).getFile();

	return file.getName().endsWith(".tcl");
      }
    else
      return false;
  }

  /** Make the real document */

  protected MaxDocument loadDocument(MaxDocumentSource source) throws MaxDocumentException
  {
    File file = ((MaxFileDocumentSource) source).getFile();
    Interp interp = MaxApplication.getTclInterp();

    try
      {
	interp.eval("sourceFile " + file.getPath());
      }
    catch (TclException e)
      {
	throw new MaxDocumentException("Tcl error: " + interp.getResult());
      }

    MaxDocument document = (MaxDocument) new MaxTclExecutedDocument();

    document.setDocumentSource(source);
    document.setDocumentHandler(this);

    return document;
  }

  public void saveDocument(MaxDocument document, MaxDocumentSource source) throws MaxDocumentException
  {
    throw new MaxDocumentException("Cannot save a MaxTclExecutedDocument");
  }

  /** Return true if this Document Handler can save a given document
    to the given source.
    */

  public boolean canSaveTo(MaxDocument document, MaxDocumentSource source)
  {
    return false;
  }
}



