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

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;

import ircam.jmax.toolkit.*;

public class PatcherSaveManager
{
  private static PatcherSaveManager saveManager = new PatcherSaveManager();

  /****************************************************************************/
  /*                                                                          */
  /*   ACTIONS                                                                */
  /*                                                                          */
  /****************************************************************************/
  
  static public boolean Save(EditorContainer container)
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
	return SaveFromSubPatcher(container, document);

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
      saved = SaveAs(container);
    return saved;
  }

  static public boolean SaveAs(EditorContainer container)
  {
    File file;

    ErmesSketchPad sketch = (ErmesSketchPad)container.getEditor();

    MaxDocument document = sketch.getDocument();
    FtsPatcherData patcherData = sketch.getFtsPatcherData();
    boolean saved = false;

    if (! document.isRootData(patcherData))
	return SaveAsFromSubPatcher(container , document);

    file = MaxFileChooser.chooseFileToSave( container.getFrame(), document.getDocumentFile(), "Save As");

    if (file == null)
      return false;
    else
      document.bindToDocumentFile( file);

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
  static boolean SaveAsFromSubPatcher(EditorContainer ec, MaxDocument document)
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
    else
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

  static boolean SaveFromSubPatcher(EditorContainer container, MaxDocument document)
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
	saved = SaveAsFromSubPatcher(container, document);
    return saved;
  }

  static public void SaveTemplate(EditorContainer container)
  {
    //nothing for now
  }

  static public void SaveTo(EditorContainer container)
  {
    File file;

    ErmesSketchPad sketch = (ErmesSketchPad)container.getEditor();

    MaxDocument document = sketch.getDocument();
    FtsPatcherData patcherData = sketch.getFtsPatcherData();

    file = MaxFileChooser.chooseFileToSave(container.getFrame(), document.getDocumentFile(), "Save To");

    if (file == null)
      return;

    try
      {
	if (document.isRootData(patcherData))
	  {
	    // Make a document save to
	    document.saveTo( file);
	  }
	else
	  {
	    // Make a subdocument save to
	    document.saveSubDocumentTo( patcherData, file);
	  }
      }
    catch ( MaxDocumentException e)
      {
	new ErrorDialog(container.getFrame(), e.getMessage());
      }
  }
  
  static public boolean SaveClosing(EditorContainer container, boolean doCancel)
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
	  toClose = Save(container);

	aDialog.dispose();
      }
    return toClose;
  }
}




