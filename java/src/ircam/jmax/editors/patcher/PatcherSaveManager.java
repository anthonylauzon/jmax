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
package ircam.jmax.editors.patcher;

import java.awt.*;
import java.io.*;
import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;

import ircam.jmax.toolkit.*;

public class PatcherSaveManager
{
  /****************************************************************************/
  /*                                                                          */
  /*   ACTIONS                                                                */
  /*                                                                          */
  /****************************************************************************/
  
  static public boolean save( EditorContainer container)
  {
    // first, tentative implementation:
    // the FILE is constructed now, and the ErmesSketchPad SaveTo method is invoked.
    // we should RECEIVE this FILE, or contruct it when we load this document
		
    // The "canSave" method of a data tell if it can be saved
    // i.e. if it have a document , and if we can write to its document file

    // Change in semantic: now Save() is active *only* on root level patchers 
    // SHOULD BECOME Gray in the others

    ErmesSketchPad sketch = (ErmesSketchPad)container.getEditor();

    MaxDocument document = sketch.getDocument();
    FtsPatcherData patcherData = sketch.getFtsPatcherData();
    boolean saved = false;
    
    if (! document.isRootData(patcherData))
	return saveFromSubPatcher(container, document);

    if (document.canSave())
      {
	try
	  {
	    document.save();

	    saved = true;
	  }
	catch ( MaxDocumentException e)
	  {
	    new ErrorDialog( container.getFrame(), e.getMessage());
	  }
      }
    else
      saved = saveAs(container);

    return saved;
  }

  static public boolean saveAs(EditorContainer container)
  {
    File file;

    ErmesSketchPad sketch = (ErmesSketchPad)container.getEditor();

    MaxDocument document = sketch.getDocument();
    FtsPatcherData patcherData = sketch.getFtsPatcherData();
    boolean saved = false;
    int saveType;
 
    if ( document.getDocumentHandler() instanceof FtsDotPatRemoteDocumentHandler)
      saveType = MaxFileChooser.SAVE_PAT_TYPE;
    else
      saveType = MaxFileChooser.SAVE_JMAX_TYPE;

    if (! document.isRootData(patcherData))
	return saveAsFromSubPatcher(container , document);

    file = MaxFileChooser.chooseFileToSave( container.getFrame(), 
					    document.getDocumentFile(), 
					    "Save As",
   					    saveType);

    if (file == null)
      return false;

    // Test if file exists already
    if (file.exists())
      {
	int result = JOptionPane.showConfirmDialog( container.getFrame(),
						    "File \"" + file.getName() + "\" exists.\nOK to overwrite ?",
						    "Warning",
						    JOptionPane.YES_NO_OPTION,
						    JOptionPane.WARNING_MESSAGE);

	if ( result != JOptionPane.OK_OPTION)
	  return false;
      }

    //document.bindToDocumentFile( file);
    // (fd) This does not work... It always binds the document to the *first* document
    // handler that can handle it. So it there are several document handler, it's the mess.
    // Conclusion about mda ?

    document.setDocumentFile( file );

    MaxDocumentHandler documentHandler = null;

    if ( MaxFileChooser.getSaveType() == MaxFileChooser.SAVE_PAT_TYPE)
      documentHandler = FtsDotPatRemoteDocumentHandler.getInstance();
    else
      documentHandler = FtsBmaxRemoteDocumentHandler.getInstance();

    document.setDocumentHandler( documentHandler);

    document.setSaved( false );

    container.getFrame().setTitle( file.toString()); 

    try
      {
	document.save();
	saved = true;
      }
    catch ( MaxDocumentException e)
      {
	new ErrorDialog( container.getFrame(), e.getMessage());
      }
    return saved;
  }

  static ErmesSketchWindow window = null;
  static ErmesSketchPad sketch = null;

  static boolean saveAsFromSubPatcher(EditorContainer ec, MaxDocument document)
  {
    File file;
    boolean saved = false;
    sketch = (ErmesSketchPad)ec.getEditor();

    FtsObject containerObj = ((FtsPatcherData)document.getRootData()).getContainerObject();

    sketch.waiting();
    containerObj.getFts().editPropertyValue( containerObj ,  
					    new MaxDataEditorReadyListener()
					    {
					      public void editorReady(MaxDataEditor editor)
						{
						  window = ((ErmesDataEditor)editor).getSketchWindow();
						  sketch.stopWaiting();
						}
					    });  

    file = MaxFileChooser.chooseFileToSave( window, document.getDocumentFile(), "Save As");

    if (file == null)
      return false;

    // Test if file exists already
    if (file.exists())
      {
	int result = JOptionPane.showConfirmDialog( ec.getFrame(),
						    "File \"" + file.getName() + "\" exists.\nOK to overwrite ?",
						    "Warning",
						    JOptionPane.YES_NO_OPTION,
						    JOptionPane.WARNING_MESSAGE);

	if ( result != JOptionPane.OK_OPTION)
	  return false;
      }

    document.bindToDocumentFile( file);

    window.setTitle( file.toString()); 

    try
      {
	document.save();
	saved = true;
      }
    catch ( MaxDocumentException e)
      {
	new ErrorDialog( window , e.getMessage());
      }
    return saved;
  }

  static boolean saveFromSubPatcher(EditorContainer container, MaxDocument document)
  {
    boolean saved = false;
    
    if (document.canSave())
      {
	try
	  {
	    document.save();
	    saved = true;
	  }
	catch ( MaxDocumentException e)
	  {
	    new ErrorDialog( container.getFrame(), e.getMessage());
	  }
      }
    else
	saved = saveAsFromSubPatcher(container, document);
    return saved;
  }

  static public void saveTemplate(EditorContainer container)
  {
    //nothing for now
  }

  static public void saveTo(EditorContainer container)
  {
    File file;

    ErmesSketchPad sketch = (ErmesSketchPad)container.getEditor();

    MaxDocument document = sketch.getDocument();
    FtsPatcherData patcherData = sketch.getFtsPatcherData();
    int saveType;
 
    if ( document.getDocumentHandler() instanceof FtsDotPatRemoteDocumentHandler)
      saveType = MaxFileChooser.SAVE_PAT_TYPE;
    else
      saveType = MaxFileChooser.SAVE_JMAX_TYPE;

    file = MaxFileChooser.chooseFileToSave( container.getFrame(), 
					    document.getDocumentFile(), 
					    "Save To",
					    saveType);

    if (file == null)
      return;

    // Test if file exists already
    if (file.exists())
      {
	int result = JOptionPane.showConfirmDialog( container.getFrame(),
						    "File \"" + file.getName() + "\" exists.\nOK to overwrite ?",
						    "Warning",
						    JOptionPane.YES_NO_OPTION,
						    JOptionPane.WARNING_MESSAGE);

	if ( result != JOptionPane.OK_OPTION)
	  return;
      }

    MaxDocumentHandler documentHandler = null;

    if ( MaxFileChooser.getSaveType() == MaxFileChooser.SAVE_PAT_TYPE)
      documentHandler = FtsDotPatRemoteDocumentHandler.getInstance();
    else
      documentHandler = FtsBmaxRemoteDocumentHandler.getInstance();

    try
      {
	if (document.isRootData(patcherData))
	  {
	    // Make a document save to
	    document.saveTo( file, documentHandler);
	  }
	else
	  {
	    // Make a subdocument save to
	    document.saveSubDocumentTo( patcherData, file, documentHandler);
	  }
      }
    catch ( MaxDocumentException e)
      {
	new ErrorDialog(container.getFrame(), e.getMessage());
      }
  }
  
  static public boolean saveClosing(EditorContainer container, boolean doCancel)
  {
    ErmesSketchPad sketch = (ErmesSketchPad)container.getEditor();

    MaxDocument document = sketch.getDocument();
    FtsPatcherData patcherData = sketch.getFtsPatcherData();
    boolean toClose=true;

    if(document.isRootData(patcherData) && (!document.isSaved()))
      {
	FileNotSavedDialog aDialog = new FileNotSavedDialog(container.getFrame(), document, doCancel);
	
	aDialog.setLocation( 300, 300);
	aDialog.setVisible( true);
	
	if (aDialog.getNothingToDoFlag())
	  toClose = false;
	
	if (aDialog.getToSaveFlag())
	  toClose = save(container);

	aDialog.dispose();
      }
    return toClose;
  }
}








