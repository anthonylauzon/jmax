package ircam.jmax.mda; 

import java.util.*;

/** A Document Handler is an object able to load a Max Document 
 *  from a document source; the class also handle the document base of all
 *  the existing MaxDocumentHandler, and provide a static function
 *  to find the document handler can reconize an address, and another
 *  to directly load the document.
 */

abstract public class MaxDocumentHandler
{
  /** Return true if this Document Handler can load a new instance
    from the given address; by default return true if the source
    exists and it is readable */

  public boolean canLoadFrom(MaxDocumentSource source)
  {
    return source.exists() && source.canRead();
  }


  /** Return true if this document handler can save 
    to the given address; by default return true if the source
    exists, if it writable, and if we can load from it.
    */

  public boolean canSaveTo(MaxDocumentSource source)
  {
    if (source.exists() && source.canRead())
      return source.canWrite() && canLoadFrom(source);
    else
      return source.canWrite();
  }

  /** Return true if this document handler can save a given instance
    to the given address; by default return false */

  public boolean canSaveTo(MaxDocument document, MaxDocumentSource source)
  {
    if (source.exists())
      return source.canWrite() && canLoadFrom(source);
    else
      return source.canWrite();
  }

  /** Load an document from a document source.
    If you want to call this, probabily you really want to call
    the static loadDocument.
    */

  abstract protected MaxDocument loadDocument(MaxDocumentSource source) throws MaxDocumentException;

  abstract public void saveDocument(MaxDocument document, MaxDocumentSource source) throws MaxDocumentException;
}







