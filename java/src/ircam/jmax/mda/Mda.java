package ircam.jmax.mda;

import java.util.*;
import java.io.*;
import com.sun.java.swing.*;

// Tmp packages
import com.sun.java.swing.preview.*;
import com.sun.java.swing.preview.filechooser.*;

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
    return edit(data, null);
  }

  /** Start an editor for a data, specifing an editor relative "focus" point 
   * for the editor; something to show, select highlight, center in the screen,
   * whatever; use the method "showObject" of the MaxDataEditor interface
   */
  public static MaxDataEditor edit(MaxData data, Object where) throws MaxDocumentException
  {
    MaxDataEditor editor;

    /* First, check if there is already an editor for the data */

    editor = (MaxDataEditor) dataEditorTable.get(data);

    if (editor != null)
      {
	editor.reEdit();

	if (where != null)
	  editor.showObject(where);

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

	    if (where != null)
	      editor.showObject(where);

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
        data.getDocument().removeEditor(editor);
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

  /** Return true if we can load a document from a given file) */

  public static boolean canLoadDocument(File file)
  {
    for (int i = 0; i < allHandlers.size() ; i++)
      {
	MaxDocumentHandler documentHandler;

	documentHandler = (MaxDocumentHandler) allHandlers.elementAt(i);

	if (documentHandler.canLoadFrom(file))
	  return true;
      }
    return false;
  }

  /** Load an document from a given file) */

  public static MaxDocument loadDocument(File file) throws MaxDocumentException
  {
    MaxDocument newDocument;
    
    for (int i = 0; i < allHandlers.size() ; i++)
      {
	MaxDocumentHandler documentHandler;

	documentHandler = (MaxDocumentHandler) allHandlers.elementAt(i);

	if (documentHandler.canLoadFrom(file))
	  {
	    // It is the responsability of the document Handler
	    // to properly set the file and handler in the document

	    newDocument = documentHandler.loadDocument(file);
	    newDocument.setSaved(true);

	    return newDocument;
	  }
      }

    throw new MaxDocumentException("Internal error: cannot load from " + file);
  }

  /** Static method to find a Document Handler for a given Document file/document pair;
   */

  public static MaxDocumentHandler findDocumentHandlerFor(File file, MaxDocument document)
  {
    for (int i = 0; i < allHandlers.size() ; i++)
      {
	MaxDocumentHandler documentHandler;

	documentHandler = (MaxDocumentHandler) allHandlers.elementAt(i);

	if (documentHandler.canSaveTo(document, file))
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


  /** Produce a FileFilter suitable for a File Box, for all the
   * jMax document
   */

  static private class MaxAllDocumentFileFilter extends FileFilter
  {
    public boolean accept(File f)
    {
      if (f.isDirectory()) 
	return true;
      else
	return Mda.canLoadDocument(f);
    }
    
    public String getDescription()
    {
	return "jMax Documents";
    }
  }

  static public FileFilter getAllDocumentsFileFilter()
  {
    return new MaxAllDocumentFileFilter();
  }


  static private class MaxDocumentFileFilter extends FileFilter
  {
    MaxDocumentHandler documentHandler;

    MaxDocumentFileFilter(MaxDocumentHandler documentHandler)
    {
      this.documentHandler = documentHandler;
    }

    public boolean accept(File f)
    {
      if (f.isDirectory()) 
	return true;
      else
	return documentHandler.canLoadFrom(f);
    }
    
    public String getDescription()
    {
      return documentHandler.getDescription();
    }
  }

  static class FileFilterEnumeration implements Enumeration
  {
    int i = 0;

    public boolean hasMoreElements()
    {
      return (i < allHandlers.size());
    }

    public Object nextElement()
    {
      return new MaxDocumentFileFilter((MaxDocumentHandler) allHandlers.elementAt(i++));
    }
  }

  static public Enumeration getDocumentFileFilters()
  {
    return new FileFilterEnumeration();
  }
}











