//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.editors.patcher;

import java.awt.*;
import java.io.*;
import javax.swing.*;

import ircam.fts.client.*;
import ircam.jmax.*;
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
      /* WARNING!!!!!!!!!!!!!!!!!!!!!*/

    // first, tentative implementation:
    // the FILE is constructed now, and the ErmesSketchPad SaveTo method is invoked.
    // we should RECEIVE this FILE, or contruct it when we load this document
		
    // The "canSave" method of a data tell if it can be saved
    // i.e. if it have a document , and if we can write to its document file

    // Change in semantic: now Save() is active *only* on root level patchers 
    // SHOULD BECOME Gray in the others

      /*ErmesSketchPad sketch = (ErmesSketchPad)container.getEditor();

	MaxDocument document = sketch.getDocument();
	FtsPatcherObject patcherData = (FtsPatcherObject)sketch.getFtsPatcher();
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
	JOptionPane.showMessageDialog(container.getFrame(), e.getMessage(), 
	"Error", JOptionPane.ERROR_MESSAGE); 
	}
	}
	else
	saved = saveAs(container);

	return saved;*/
      return false;
  }

  static public boolean saveAs(EditorContainer container)
  {
      /* WARNING!!!!!!!!!!!!!!!!!!!!!*/

      /*File file;

	ErmesSketchPad sketch = (ErmesSketchPad)container.getEditor();

	MaxDocument document = sketch.getDocument();
	FtsPatcherObject patcherData = (FtsPatcherObject)sketch.getFtsPatcher();

	boolean saved = false;
	int saveType;
	
	if (! document.isRootData(patcherData))
	return saveAsFromSubPatcher(container , document);

	if ( document.getDocumentHandler() instanceof FtsDotPatRemoteDocumentHandler)
	saveType = MaxFileChooser.SAVE_PAT_TYPE;
	else
	saveType = MaxFileChooser.SAVE_JMAX_TYPE;

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
	MaxWindowManager.getWindowManager().windowChanged(container.getFrame());

	try
	{
	document.save();
	saved = true;
	}
	catch ( MaxDocumentException e)
	{
	JOptionPane.showMessageDialog(container.getFrame(), e.getMessage(), 
	"Error", JOptionPane.ERROR_MESSAGE); 
	}
	return saved;*/
      return false;
  }

  static ErmesSketchWindow window = null;
  static ErmesSketchPad sketch = null;

    /* WARNING!!!!!!!!!!!!!!!!!!!!*/
    /*static boolean saveAsFromSubPatcher(EditorContainer ec, MaxDocument document)
      {
      File file;
      boolean saved = false;
      sketch = (ErmesSketchPad)ec.getEditor();
      
      FtsPatcherObject containerObj = (FtsPatcherObject)document.getRootData();
      window = (ErmesSketchWindow)(containerObj.getEditorFrame();
      
      int saveType;
      
      if ( document.getDocumentHandler() instanceof FtsDotPatRemoteDocumentHandler)
      saveType = MaxFileChooser.SAVE_PAT_TYPE;
      else
      saveType = MaxFileChooser.SAVE_JMAX_TYPE;

      file = MaxFileChooser.chooseFileToSave( window, document.getDocumentFile(), 
      "Save As", saveType);
      
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
      
      /////////////////////////////
      document.setDocumentFile( file );
      
      MaxDocumentHandler documentHandler = null;
      
      if ( MaxFileChooser.getSaveType() == MaxFileChooser.SAVE_PAT_TYPE)
      documentHandler = FtsDotPatRemoteDocumentHandler.getInstance();
      else
      documentHandler = FtsBmaxRemoteDocumentHandler.getInstance();
      
      document.setDocumentHandler( documentHandler);
      
      document.setSaved( false );
      /////////////////////////////
      
      window.setTitle( file.toString()); 
      MaxWindowManager.getWindowManager().windowChanged(window);
      
      try
      {
      document.save();
      saved = true;
      }
      catch ( MaxDocumentException e)
      {
      JOptionPane.showMessageDialog( window, e.getMessage(), 
      "Error", JOptionPane.ERROR_MESSAGE); 
      }
      return saved;
      }*/


    /*WARNING!!!!!!!!!!!!!!!!!!!11*/
    /*static boolean saveFromSubPatcher(EditorContainer container, MaxDocument document)
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
      JOptionPane.showMessageDialog(container.getFrame(), e.getMessage(), 
      "Error", JOptionPane.ERROR_MESSAGE); 
      }
      }
      else
      saved = saveAsFromSubPatcher(container, document);
      }*/

  static public void saveTemplate(EditorContainer container)
  {
    //nothing for now
  }

  static public void saveTo(EditorContainer container)
  {
      /* WARNING !!!!!!!!!!!!!!!!!!!!!!*/

      /*File file;

	ErmesSketchPad sketch = (ErmesSketchPad)container.getEditor();
	
	MaxDocument document = sketch.getDocument();
	FtsPatcherObject patcherData = (FtsPatcherObject)sketch.getFtsPatcher();
    
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
	JOptionPane.showMessageDialog(container.getFrame(), e.getMessage(), 
	"Error", JOptionPane.ERROR_MESSAGE); 
	}*/
  }
  
  static public boolean saveClosing(EditorContainer container, boolean doCancel)
  {
      /* WARNING !!!!!!!!!!!!!!!!!!!!!!*/
      /*ErmesSketchPad sketch = (ErmesSketchPad)container.getEditor();

	MaxDocument document = sketch.getDocument();
	FtsPatcherObject patcherData = (FtsPatcherObject)sketch.getFtsPatcher();
	boolean toClose=true;
	
	if(document.isRootData(patcherData) && (!document.isSaved()))
	{
	String message;
	if(document.getDocumentFile() != null)
	message = "File " + document.getDocumentFile() + " is not saved.\n Do you want to save it now?";
	else {
	if(document.getName()!=null)   
	message = ("Patch " + document.getName() + " is not saved.\n Do you want to save it now?");
	else
	message = ("Patch " + container.getFrame().getTitle() + " is not saved.\n Do you want to save it now?");
	}
	
	Object[] options = { "Save", "Don't save", "Cancel" };
	int result = JOptionPane.showOptionDialog(null, message, "File Not Saved", 
	JOptionPane.YES_NO_CANCEL_OPTION,
	JOptionPane.QUESTION_MESSAGE,
	null, options, options[0]);
	
	if(result == JOptionPane.CANCEL_OPTION)
	toClose = false;
	if(result == JOptionPane.YES_OPTION)
	toClose = save(container);	  
	}
    
	return toClose;*/
      return false;
  }
}








