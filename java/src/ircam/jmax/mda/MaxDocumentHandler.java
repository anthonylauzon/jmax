package ircam.jmax.mda; 



import java.util.*;
import java.io.*;
import java.awt.*;

import javax.swing.*;

/** A Document Handler is an object able to load a Max Document 
 *  from a document file; the class also handle the document base of all
 *  the existing MaxDocumentHandler, and provide a static function
 *  to find the document handler can reconize an address, and another
 *  to directly load the document.
 */

abstract public class MaxDocumentHandler
{
  /** Return true if this Document Handler can load a new instance
    from the given address; by default return true if the file
    exists and it is readable */

  public boolean canLoadFrom(File file)
  {
    return file.exists() && file.canRead();
  }


  /** Return true if this document handler can save 
    to the given address; by default return true if the file
    exists, if it writable, and if we can load from it.
    */

  public boolean canSaveTo(File file)
  {
    if (file.exists() && file.canRead())
      return file.canWrite() && canLoadFrom(file);
    else
      return file.canWrite();
  }

  /** Return true if this document handler can save a given instance
    to the given address; by default return false */

  public boolean canSaveTo(MaxDocument document, File file)
  {
    if (file.exists())
      return file.canWrite() && canLoadFrom(file);
    else
      return file.canWrite();
  }

  /** Load an document from a document file.
    If you want to call this, probabily you really want to call
    the static loadDocument.
    */

  abstract protected MaxDocument loadDocument(File file) throws MaxDocumentException;

  abstract public void saveDocument(MaxDocument document, File file) throws MaxDocumentException;

  protected void saveSubDocument(MaxDocument document, MaxData data, File file) throws MaxDocumentException
  {
    throw new MaxDocumentException("SYSTEM ERROR: Cannot save subdocuments !");
  }

  /** Return a string describing the kind of files this handler can load 
    It is used for the file chooser dialog
    */

  abstract public String getDescription();

  public Icon getIcon()
  {
    return null;
  }
}







