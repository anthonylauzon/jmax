package ircam.jmax.mda;

import java.util.*;
import com.sun.java.swing.*;

/** This class provide central registration services
 * and methods that are not tied to specific classes 
 * All MDA wide global public services are accessed thru this class.
 */

public class Mda
{
  /*
   * Editor factory handling
   */

  /**
   * Store all the registered factory; the type checking is done
   * by the factory itself, using the method "canEdit"
   */
     
  private static Vector editorFactoryTable = new Vector();

  /**
   * Store all the pair data/editor; used for house keeping:
   * avoiding double editors, closing editors when the data
   * is destroyed and so on.
   */

  private static Hashtable dataEditorTable = new Hashtable();
  
  /** Register an editor factory for a given class */
 
  public static void installEditorFactory(MaxDataEditorFactory factory)
  {
    editorFactoryTable.addElement(factory);
  }

  /** Unregister an editor factory for a given class */

  public static void uninstallEditorFactory(MaxDataEditorFactory factory)
  {
    editorFactoryTable.removeElement(factory);
  }

  /** Start an editor for a data */

  public static MaxDataEditor edit(MaxData data) throws MaxDocumentException
  {
    MaxDataEditor editor;

    /* First, check if there is already an editor for the data */

    editor = (MaxDataEditor) dataEditorTable.get(data);

    if (editor != null)
      {
	editor.reEdit();

	return editor;
      }
    

    /* Not found, create a new one */

    for (int i = 0; i < editorFactoryTable.size() ; i++)
      {
	MaxDataEditorFactory factory;

	factory = (MaxDataEditorFactory) editorFactoryTable.elementAt(i);
	
	if (factory.canEdit(data))
	  {
	    editor = factory.newEditor(data);

	    data.getDocument().addEditor(editor);
	    dataEditorTable.put(data, editor);

	    return editor;
	  }
      }

    throw new MaxDocumentException("No editor for " + data);
  }

  /* dispose a data: signal Mda that the data should not be used
     anymore; for the moment, just close the associated editor.
     */

  public static void dispose(MaxData data)
  {
    MaxDataEditor editor;

    /* First, check if there is  an editor for the data */

    editor = (MaxDataEditor) dataEditorTable.get(data);

    if (editor != null)
      {
	editor.quitEdit();

	dataEditorTable.remove(data);
      }
  }

  /*
   * Document handlers registration service 
   */

  /** All the instantiated document handlers */

  private static Vector allHandlers = new Vector();

  /**  handle registration in the global
   * table; note that Document Handlers cannot be deinstalled, once
   * created; handler have to be installed after the creation,
   * before using them.
   */

  public static void installDocumentHandler(MaxDocumentHandler handler)
  {
    allHandlers.addElement(handler);
  }

  /** Load an document from a given source) */

  public static MaxDocument loadDocument(MaxDocumentSource source) throws MaxDocumentException
  {
    MaxDocument newDocument;
    
    for (int i = 0; i < allHandlers.size() ; i++)
      {
	MaxDocumentHandler documentHandler;

	documentHandler = (MaxDocumentHandler) allHandlers.elementAt(i);

	if (documentHandler.canLoadFrom(source))
	  {
	    // It is the responsability of the document Handler
	    // to properly set the source and handler in the document

	    newDocument = documentHandler.loadDocument(source);
	    newDocument.setSaved(true);

	    return newDocument;
	  }
      }

    throw new MaxDocumentException("Internal error: cannot load from " + source);
  }

  /** Static method to find a Document Handler for a given Document Source/document pair;
   */

  public static MaxDocumentHandler findDocumentHandlerFor(MaxDocumentSource source, MaxDocument document)
  {
    for (int i = 0; i < allHandlers.size() ; i++)
      {
	MaxDocumentHandler documentHandler;

	documentHandler = (MaxDocumentHandler) allHandlers.elementAt(i);

	if (documentHandler.canSaveTo(document, source))
	  return documentHandler;
      }

    return null;
  }

  /*
   * Document Type registration and handling 
   */

  private static DefaultListModel typeList = new DefaultListModel();

  public static DefaultListModel getDocumentTypes()
  {
    return typeList;
  }

  public static void installDocumentType(MaxDocumentType type)
  {
    typeList.addElement(type);
  }

  public static MaxDocumentType getDocumentTypeByName(String name)
  {
    for (int i = 0; i < typeList.size() ; i++)
      {
	MaxDocumentType type;

	type = (MaxDocumentType) typeList.elementAt(i);

	if (type.getName().equals(name))
	  return type;
      }
    
    return null;
  }

  public static MaxDocument newDocument(MaxData data) throws MaxDocumentException
  {
    for (int i = 0; i < typeList.size() ; i++)
      {
	MaxDocumentType type;

	type = (MaxDocumentType) typeList.elementAt(i);
	
	if (type.canMakeNewDocument(data))
	  return type.newDocument(data);
      }

    throw new MaxDocumentException("Cannot create document for " + data);
  }
}

