//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.mda;

import java.util.*;
import java.io.*;

import javax.swing.*;
import javax.swing.filechooser.*;

// Tmp packages

// import javax.swing.preview.*;
// import javax.swing.preview.filechooser.*;

import ircam.jmax.utils.*;

/** 
 * Access to MDA wide global public services.
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
     
  private static MaxVector editorFactoryTable = new MaxVector();

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

	    // In case of document less data 

	    if (data.getDocument() != null)
	      data.getDocument().addEditedData(data);

	    dataEditorTable.put(data, editor);

	    return editor;
	  }
      }

    throw new MaxDocumentException("No editor for " + data);
  }

  /** dispose a data. Signal Mda that the data should not be used
     anymore; for the moment, just close the associated editor.
     */

  public static void dispose(MaxData data)
  {
    MaxDataEditor editor;

    /* First, check if there is  an editor for the data */

    editor = (MaxDataEditor) dataEditorTable.get(data);

    if (editor != null)
      {
        data.getDocument().removeEditedData(data);
	editor.quitEdit();
	dataEditorTable.remove(data);
      }
  }

  /*
   * Document handlers registration service 
   */

  /** All the instantiated document handlers */

  private static MaxVector allHandlers = new MaxVector();

  /**  handle registration in the global
   * table. Note that Document Handlers cannot be deinstalled, once
   * created; handler have to be installed after the creation,
   * before using them.
   */

  public static void installDocumentHandler(MaxDocumentHandler handler)
  {
    allHandlers.addElement(handler);
  }

  /** Return true if we can load a document from a given file.*/

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

  /** Load a document from a given file */

  public static MaxDocument loadDocument(MaxContext context, File file) throws MaxDocumentException
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

	    newDocument = documentHandler.loadDocument(context, file);
	    newDocument.setSaved(true);

	    return newDocument;
	  }
      }

    throw new MaxDocumentException("Internal error: cannot load from " + file);
  }

  /** Static method to find a Document File description.
   *  We take the description from the document handler, and not from
   * the document type, because this is the description of the document;
   * a document file can produce diffent document types (for example,
   * a scheme/tcl file).
   */

  public static String getFileDescription(File file)
  {
    for (int i = 0; i < allHandlers.size() ; i++)
      {
	MaxDocumentHandler documentHandler;

	documentHandler = (MaxDocumentHandler) allHandlers.elementAt(i);

	if (documentHandler.canLoadFrom(file))
	  return documentHandler.getDescription();
      }

    return null;
  }


  /** Static method to find a Document file icon.
   */

  public static Icon getFileIcon(File file)
  {
    for (int i = 0; i < allHandlers.size() ; i++)
      {
	MaxDocumentHandler documentHandler;

	documentHandler = (MaxDocumentHandler) allHandlers.elementAt(i);

	if (documentHandler.canLoadFrom(file))
	  return documentHandler.getIcon();
      }

    return null;
  }



  /** Static method to find a Document Handler for a given Document file/document pair.
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

  /** Get a list of existing document types */

  public static DefaultListModel getDocumentTypes()
  {
    return typeList;
  }

  /** Install a new document type */

  public static void installDocumentType(MaxDocumentType type)
  {
    typeList.addElement(type);
  }

  /** Get a document type by name between the installed ones */

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


  /** Instantiate a new MaxDocument for a given data instance */

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

  /** Produce a FileFilter suitable for a File Box, for all the
   * jMax document
   */

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


  static class MaxDocumentFileView extends FileView
  {
    public String getName(File f)
    {
      return null; // let the L&F FileView figure this out
    }
    
    public String getDescription(File f)
    {
      return null; // let the L&F FileView figure this out
    }
    
    public String getTypeDescription(File f)
    {
      return Mda.getFileDescription(f);
    }
    
    public Icon getIcon(File f)
    {
      return Mda.getFileIcon(f);
    }
    
    public Boolean isTraversable(File f)
    {
      return null; // let the L&F FileView figure this out
    }
    
    // Get the extension of this file. Code is factored out
    // because we use this in both getIcon and getTypeDescription

    private String getExtension(File f)
    {
      String ext = null;
      String s = f.getName();

      int i = s.lastIndexOf('.');

      if (i > 0 &&  i < s.length() - 1) 
	ext = s.substring(i+1).toLowerCase();

      return ext;
    }
  }

  /** Generate a FileView based on Mda suitable for a JFileChooser dialog,
    so that each file type get its Icon and description */

  public static FileView getFileView()
  {
    return new  MaxDocumentFileView();
  }
}











