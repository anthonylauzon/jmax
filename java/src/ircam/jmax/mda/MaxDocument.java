package ircam.jmax.mda;

import java.util.*;
import java.io.*;
import com.sun.java.swing.*;

/**
 * Superclass for all the Max Document
 * provide basic services, like tracing 
 * the document file and the document handler,
 * registering and disposing
 *
 * TO ADD: very important: a generic naming scheme for data within documents
 */

abstract public class MaxDocument
{
  protected MaxData  rootData; // the "prefferred" or top level data to edit
  protected MaxDocumentHandler handler = null;
  protected File file                  = null;
  protected MaxDocumentType    type    = null;
  private   Vector editedData          = new Vector();
  protected String name   = null; // name of the document, for UI purposes
  protected boolean saved = false;   // saved flag

  /** A constructor that get only the type */

  public MaxDocument(MaxDocumentType type)
  {
    type.registerDocument(this);
    this.type = type;
  }

  /** A constructor that get the type and the document name */

  public MaxDocument(MaxDocumentType type, String name)
  {
    type.registerDocument(this);
    this.type = type;
    this.name = name;
  }

  /**
   * Adding an editor; this method 
   * do not bind the editor on the document, and it is private
   */

  protected void addEditedData(MaxData data)
  {
    editedData.addElement(data);
  }

  /** Removing the editor */

  protected void removeEditedData(MaxData data)
  {
    editedData.removeElement(data);
  }

  /** Getting the handler */

  public MaxDocumentHandler getDocumentHandler()
  {
    return handler;
  }

  /** Setting the handler */

  public  void setDocumentHandler(MaxDocumentHandler handler)
  {
    this.handler = handler;
  }

  /** Getting the document file  */

  public File getDocumentFile()
  {
    return file;
  }

  /**
   * Bind this document to a new document file; implicitly get a new Document Handler
   * it is the public method to call to set a document file.
   */

  public void bindToDocumentFile(File file)
  {
    setDocumentFile(file);
    setDocumentHandler(Mda.findDocumentHandlerFor(file, this));
    setSaved(false);
  }

  /** To set both the handler and the file at the same time;
    used in initialization
    */

  public void setDocumentFile(File file)
  {
    this.file = file;
    this.name = file.getName();
  }

  /** Getting the type */

  public MaxDocumentType getDocumentType()
  {
    return type;
  }

  /** Setting the type */

  public void setDocumentType(MaxDocumentType type)
  {
    this.type = type;
  }


  /** Getting the name */

  public String getName()
  {
    return name;
  }

  /** Setting the name */

  public void setName(String name)
  {
    this.name = name;
  }

  /** Get the root data of the document */

  public MaxData getRootData()
  {
    return rootData;
  }

  /** Set the root data; for the document implementations */

  public void setRootData(MaxData data)
  {
    rootData = data;
  }

  /** edit: start an document of the default editor for this document,
   *  on its root data; Just a convenience function.
   *  it is not the only permitted way to start
   * an editor on a document , but it may be convenient.
   */

  public MaxDataEditor edit() throws MaxDocumentException
  {
    if (rootData != null)
      return Mda.edit(rootData);
    else
      throw new MaxDocumentException("No data to edit");
  }

  /** return true if the document can be saved to its current
   * document file
   */

  public boolean canSave()
  {
    if (file == null)
      return false;
    else if (handler == null)
      return false;
    else
      return handler.canSaveTo(this, file);
  }
    
  /** Save the document to its document file */

  public void save() throws MaxDocumentException
  {
    if (! canSave())
      throw new MaxDocumentException("Cannot save to " + file);

    if (handler == null)
      throw new MaxDocumentException("No document handler for " + file);

    handler.saveDocument(this, file);
    setSaved(true);
  }


  /** Save the document to a given file, without changing the
   *  original binding of the document 
   */

  public void saveTo(File file) throws MaxDocumentException
  {
    MaxDocumentHandler handler = Mda.findDocumentHandlerFor(file, this);

    if (handler == null)
      throw new MaxDocumentException("Cannot save to " + file);

    handler.saveDocument(this, file);
  }

  /** Save a subdocument to a given file, without changing the
   *  original binding of the document 
   *
   *  A sub document is a document corresponding to a part of the document.
   *  the save semantic is to create a *new* document file which content is a part
   *  of the current document; not all document types allow subdocuments; the canMakeSubDocumentFile
   *  method of the corresponding document type be redefined to return true in order to allow sub document
   *  for a document type (should this method be in documenttype ?? 
   * 
   *  The actual subdocument saving is implemented in the document handler.
   */

  public void saveSubDocumentTo(MaxData data, File file) throws MaxDocumentException
  {
    if (type.canMakeSubDocumentFile(data))
      {
	MaxDocumentHandler handler = Mda.findDocumentHandlerFor(file, this);

	if (handler == null)
	  throw new MaxDocumentException("Cannot save to " + file);

	handler.saveSubDocument(this, data, file);
      }
  }

  /** Saved flag query: true if the document have been saved to its current
      file.
      */

  public boolean isSaved()
  {
    return saved;
  }

  /** Saved flag setting; it is the responsability of the MaxData 
   * composing the document to update this flags correctly.
   */

  public void setSaved(boolean saved)
  {
    this.saved = saved;
  }

  /** Disposing this document;
   * Note that we don't close the editors here; the editors
   * are closed by the Mda.dispose(MaxData data) method, that
   * all the MaxData need to call when they are destroyed.
   */

  public void dispose()
  {
    Vector toDispose;

    toDispose = (Vector) editedData.clone();

    for (int i = 0; i < toDispose.size(); i++)
      Mda.dispose((MaxData) toDispose.elementAt(i));

    type.disposeDocument(this);
    handler = null;
    type = null;
    file = null;
  }
}



