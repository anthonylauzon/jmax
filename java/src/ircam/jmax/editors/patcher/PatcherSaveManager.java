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

import java.io.*;
// import javax.swing.*;
import javax.swing.JOptionPane;

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
    ErmesSketchPad sketch = (ErmesSketchPad)container.getEditor();

    FtsPatcherObject patcherObj = sketch.getFtsPatcher();
    boolean saved = false;
    
    if (! patcherObj.isARootPatcher())
      return saveFromSubPatcher(container, patcherObj.getRootPatcher());
      
    if (patcherObj.canSave())
      {
	patcherObj.save();      
	saved = true;
      }
    else
      saved = saveAs(container);

    return saved;
  }

  static public boolean saveAs(EditorContainer container)
  {
    ErmesSketchPad sketch = (ErmesSketchPad)container.getEditor();
    FtsPatcherObject patcherObj = sketch.getFtsPatcher();

    if (! patcherObj.isARootPatcher())
      return saveAsFromSubPatcher( container, patcherObj.getRootPatcher());

    File file = MaxFileChooser.chooseFileToSave( container.getFrame(), null, 
						 "Save As", MaxFileChooser.JMAX_FILE_TYPE);
    
    if (file == null)
      return false;
    
    String path = file.getAbsolutePath();
    
    if((MaxFileChooser.getSaveType() == MaxFileChooser.JMAX_FILE_TYPE) && ! path.endsWith(".jmax"))
      file = new File( path+".jmax");
    else
      if((MaxFileChooser.getSaveType() == MaxFileChooser.PAT_FILE_TYPE) && ! path.endsWith(".pat"))
	file = new File( path+".pat");

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

    patcherObj.save( MaxFileChooser.getSaveType(), file.getAbsolutePath());

    return true;
  }

  static boolean saveAsFromSubPatcher(EditorContainer ec, FtsPatcherObject patcherObj)
  {
    File file = MaxFileChooser.chooseFileToSave( ec.getFrame(), null, 
						 "Save As", MaxFileChooser.JMAX_FILE_TYPE);    
    if (file == null)
      return false;
      
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

    patcherObj.save( MaxFileChooser.getSaveType(), file.getAbsolutePath());
    return true;
  }


  static boolean saveFromSubPatcher(EditorContainer container, FtsPatcherObject patcherObj)
  {
    boolean saved = false;
      
    if (patcherObj.canSave())
      {
	patcherObj.save();
	saved = true;
      }
    else
      saved = saveAsFromSubPatcher(container, patcherObj);
  
    return saved;
  }

  static public void saveTemplate(EditorContainer container)
  {
    //nothing for now
  }

  static public void saveTo(EditorContainer container)
  {
    /*File file;
      
      ErmesSketchPad sketch = (ErmesSketchPad)container.getEditor();
      FtsPatcherObject patcherObj = (FtsPatcherObject)sketch.getFtsPatcher();
      
      file = MaxFileChooser.chooseFileToSave( container.getFrame(), 
      document.getDocumentFile(), 
      "Save To",
      MaxFileChooser.JMAX_FILE_TYPE);
      
      if (file == null)
      return;
      
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

      if ( MaxFileChooser.getSaveType() == MaxFileChooser.PAT_FILE_TYPE)
      documentHandler = FtsDotPatRemoteDocumentHandler.getInstance();
      else
      documentHandler = FtsBmaxRemoteDocumentHandler.getInstance();

      try
      {
      if (patcherObj.isARootPatcher(patcherData))
      {
      //document.saveTo( file, documentHandler);
      }
      else
      {
      //document.saveSubDocumentTo( patcherObj, file, documentHandler);
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
    ErmesSketchPad sketch = (ErmesSketchPad)container.getEditor();
    FtsPatcherObject patcherObj = sketch.getFtsPatcher();
    boolean toClose = true;
      
    if(patcherObj.isARootPatcher() && ( patcherObj.isDirty()))
      {
	String message;
	if(patcherObj.getName() != null)
	  message = "File " + patcherObj.getName() + " is not saved.\n Do you want to save it now?";
	else {
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
      
      return toClose;
  }
}








