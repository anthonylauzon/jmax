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
import java.awt.datatransfer.*;
import java.io.*;
import java.util.*;
import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.fts.client.*;
import ircam.jmax.editors.patcher.objects.*;

import ircam.jmax.toolkit.*;

//
// The window that contains the sketchpad. It knows the ftspatcher it is editing.
// It handles all the sketch menus, it knows how to load from a ftspatcher.
 //

// ^^^^ Problem with the clipboard: currently the clipboard is a  static,
// ^^^^ But should move, where i don't know; copy/paste between different
// ^^^^ servers can be tricky (very very tricky), but actually just need
// ^^^^ to share the file; but, the implementation need to change, because the
// ^^^^ clipboard is represented by an fts object, that so reside in a single
// ^^^^ server.

public class PatcherClipboardManager implements ClipboardOwner
{
  public static PatcherClipboardManager clipboardManager = null;
  
  private static FtsClipboard ftsClipboard;

  private EditorContainer currentContainer;
  /****************************************************************************/
  /*                                                                          */
  /*           CONSTRUCTOR                                                      */
  /*                                                                          */
  /****************************************************************************/

  public PatcherClipboardManager() 
  {
      try
	  {
	    ftsClipboard = new FtsClipboard();
	  }
      catch(IOException e)
	  {
	      System.err.println("[PatcherClipboardManager]: Error in FtsClipboard creation!");
	      e.printStackTrace();
	  }
  }
  
  public static void createManager() {
    if (clipboardManager == null)
      clipboardManager = new PatcherClipboardManager();
  }

  public static PatcherClipboardManager getManager()
  {
      if(clipboardManager == null)
	  createManager();
      return clipboardManager;
  }

  /****************************************************************************/
  /*                                                                          */
  /*   ACTIONS                                                                */
  /*                                                                          */
  /****************************************************************************/

  public void Cut(EditorContainer container)
  {
    ErmesSketchPad sketch = (ErmesSketchPad)container.getEditor();
    if (sketch.canCopyText())
      {
	sketch.cutText();
      }
    else if (ErmesSelection.patcherSelection.ownedBy(sketch)&&!ErmesSelection.patcherSelection.isEmpty())
      {
	Cursor temp = container.getFrame().getCursor();
	container.getFrame().setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));

	if(!ErmesSelection.patcherSelection.isObjectsEmpty())
	  {
	    JMaxApplication.getSystemClipboard().setContents(ErmesSelection.patcherSelection, this);

	    ftsClipboard.copy(ErmesSelection.getFtsSelection());
	    sketch.setLastCopyCount(ftsClipboard.getCopyCount());
	    sketch.resetPaste(-1);
	    GraphicObject obj = ErmesSelection.patcherSelection.getSingleton();
	    sketch.setStartPasteXY( obj.getX(), obj.getY());
	  }
	ErmesSelection.patcherSelection.redraw();
	ErmesSelection.patcherSelection.deleteAll();

	container.getFrame().setCursor( temp);
      }
  }

  public void Copy( EditorContainer container)
  {
    ErmesSketchPad sketch = (ErmesSketchPad)container.getEditor();
    if ( sketch.canCopyText())
      {
	sketch.copyText();
      }
    else 
      if ( ErmesSelection.patcherSelection.ownedBy( sketch)&&!ErmesSelection.patcherSelection.isObjectsEmpty())
	{
	  Cursor temp = container.getFrame().getCursor();
	  container.getFrame().setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));

	  JMaxApplication.getSystemClipboard().setContents( ErmesSelection.patcherSelection, this);
	  ftsClipboard.copy( ErmesSelection.getFtsSelection());
	  sketch.setLastCopyCount( ftsClipboard.getCopyCount());
	  sketch.resetPaste( 0);	  
	  GraphicObject obj = ErmesSelection.patcherSelection.getSingleton();
	  sketch.setStartPasteXY( obj.getX(), obj.getY());
	  container.getFrame().setCursor( temp);
	}
  }

  /**
   * Paste the content of clipboard.
   * The patcher supports text and "patcherSelectionFlavor" DataFlavor.
   * A text can be pasted only if an editable field is currently opened,
   * and a patcher selection can be pasted if we're not currently editing
   * a text. */

  public void Paste(EditorContainer container)
  {
    ErmesSketchPad sketch = (ErmesSketchPad)container.getEditor();
    currentContainer = container;

    if (sketch.isLocked())
      sketch.setLocked(false);

    Transferable clipboardContent = JMaxApplication.getSystemClipboard().getContents(this);

    if (clipboardContent == null)
      return;  // Should never happen

    if (clipboardContent.getTransferDataFlavors() == null)
      return; // Should never happen, protection against system clipboard bug.

    if (clipboardContent.isDataFlavorSupported(DataFlavor.stringFlavor))
      {
	if (sketch.canPasteText())
	  {
	    try
	      {
		sketch.pasteText();
	      }
	    catch (Exception e)
	      {
		System.err.println("error while pasting text: " + e);
	      }
	  }
      }
    else 
      {	
	container.getFrame().setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));
	
	if (sketch.getLastCopyCount() != ftsClipboard.getCopyCount())
	  {
	    sketch.resetPaste(-1);
	    sketch.setLastCopyCount(ftsClipboard.getCopyCount());
	    sketch.setOldPastedObject( null);
	    sketch.setStartPasteXY( -1, -1);
	  }
	
	newPaste(sketch);
	sketch.getFtsPatcher().requestPaste( ftsClipboard, sketch.getPasteDX(), sketch.getPasteDY());
	
	sketch.getFtsPatcher().requestStopWaiting(new FtsActionListener(){
	    public void ftsActionDone()
	    {
	      currentContainer.getFrame().setCursor(Cursor.getDefaultCursor());
	    }
	  });	
      }
  }

  void newPaste(ErmesSketchPad sketch)
  {
    int pasteNum = sketch.getPasteNumber();
    int  incrementalPasteOffsetX;
    int  incrementalPasteOffsetY;    

    if (sketch.isTextEditingObject())
      sketch.stopTextEditing();

    if (pasteNum == 0) 
      sketch.setIncrementalPasteOffsets(0, 0);
    else if (pasteNum == 1) 
      sketch.setIncrementalPasteOffsets(20, 20);
    else if (pasteNum == 2) 
      {
	sketch.setIncrementalPasteOffsets((int)(sketch.getOldPastedObject().getX() - sketch.getStartPasteX()),
					  (int)(sketch.getOldPastedObject().getY() - sketch.getStartPasteY()));    
      }
  }

  public boolean ftsClipboardIsEmpty()
  {
    return ftsClipboard.isEmpty();
  }

  public void lostOwnership(Clipboard c, Transferable t) 
  {
  }

  private ErmesSketchPad sketch;

  public void Duplicate(EditorContainer container)
  {
    sketch = (ErmesSketchPad)container.getEditor();

    if((!ErmesSelection.patcherSelection.ownedBy(sketch))||(ErmesSelection.patcherSelection.isObjectsEmpty()))
      return;

    Copy(container); 
    Paste(container);

    // (fd) if only one object duplicated, then edit it with all text selected

    if (ErmesSelection.patcherSelection.isSingleton())
      {
	GraphicObject obj = (GraphicObject)ErmesSelection.patcherSelection.getSingleton();

	if (obj instanceof Editable)
	  {
	    ErmesSelection.patcherSelection.deselectAll();

	    sketch.textEditObject((Editable)obj);
	    SwingUtilities.invokeLater(new Runnable(){
	      public void run(){
		sketch.getEditField().selectAll();
	      }
	    });
	  }
      }
  }
}







