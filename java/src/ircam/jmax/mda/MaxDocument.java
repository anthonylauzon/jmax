package ircam.jmax.mda;

import java.util.*;
import com.sun.java.swing.*;

/**
 * Superclass for all the Max Document
 * provide basic services, like tracing 
 * the document source and the document handler,
 * registering and disposing
 *
 * TO ADD: very important: a generic naming scheme for data within documents
 */

abstract public class MaxDocument
{
  protected MaxData  rootData; // the "prefferred" or top level data to edit
  protected MaxDocumentHandler handler = null;
  protected MaxDocumentSource  source  = null;
  protected MaxDocumentType    type    = null;
  private   DefaultListModel editors = new DefaultListModel();
  protected String name = null; // name of the document, for UI purposes
  protected String info = null; // comment field; store and get back, but don't use for semantic purpose

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

  protected void addEditor(MaxDataEditor editor)
  {
    editors.addElement(editor);
  }

  /** Removing the editor */

  protected void removeEditor(MaxDataEditor editor)
  {
    editors.removeElement(editor);
  }


  /** Getting the list of editors, as a ListModel/DefaultListModel */

  public DefaultListModel getEditors()
  {
    return editors;
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

  /** Getting the document source */

  public MaxDocumentSource getDocumentSource()
  {
    return source;
  }

  /**
   * Bind this document to a new document the source; implicitly get a new Document Handler
   * it is the public method to call to set a document source.
   */

  public void bindToDocumentSource(MaxDocumentSource source)
  {
    setDocumentSource(source);
    setDocumentHandler(Mda.findDocumentHandlerFor(source, this));
  }

  /** To set both the handler and the source at the same time;
    used in initialization
    */

  public void setDocumentSource(MaxDocumentSource source)
  {
    this.source = source;
    this.name = source.getName();
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

  /** Getting the info */

  public String getInfo()
  {
    return info;
  }

  /** Setting the info */

  public void setInfo(String info)
  {
    this.info = info;
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
    return Mda.edit(rootData);
  }

  /** return true if the document can be saved to its current
   * document source
   */

  public boolean canSave()
  {
    if (source == null)
      return false;
    else if (handler == null)
      return false;
    else
      return handler.canSaveTo(this, source);
  }
    
  /** Save the document to its document source */

  public void save() throws MaxDocumentException
  {
    if (! canSave())
      throw new MaxDocumentException("Cannot save to " + source);

    for (int i = 0; i < editors.size() ; i++)
      {
	MaxDataEditor editor;

	editor = (MaxDataEditor) editors.elementAt(i);

	editor.syncData();
      }

    if (handler == null)
      throw new MaxDocumentException("No document handler for " + source);

    handler.saveDocument(this, source);
  }


  /** Save the document to a given source, without changing the
   *  original binding of the document 
   */

  public void saveTo(MaxDocumentSource source) throws MaxDocumentException
  {
    MaxDocumentHandler handler = Mda.findDocumentHandlerFor(source, this);

    if (handler == null)
      throw new MaxDocumentException("Cannot save to " + source);

    for (int i = 0; i < editors.size() ; i++)
      {
	MaxDataEditor editor;

	editor = (MaxDataEditor) editors.elementAt(i);

	editor.syncData();
      }

    handler.saveDocument(this, source);
  }

  /** Disposing this document */

  public void dispose()
  {
    for (int i = 0; i < editors.size() ; i++)
      {
	MaxDataEditor editor;

	editor = (MaxDataEditor) editors.elementAt(i);

	editor.quitEdit();
      }

    type.disposeDocument(this);
    handler = null;
    type = null;
    source = null;
  }
}



