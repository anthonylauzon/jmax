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
import java.awt.datatransfer.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import ircam.jmax.editors.patcher.objects.*;

//
// The window that contains the sketchpad. It knows the ftspatcher it is editing.
// It handles all the sketch menus, it knows how to load from a ftspatcher.
 //

// ^^^^ Problem with the clipboard: currently the clipboard is a static,
// ^^^^ But should move, where i don't know; copy/paste between different
// ^^^^ servers can be tricky (very very tricky), but actually just need
// ^^^^ to share the file; but, the implementation need to change, because the
// ^^^^ clipboard is represented by an fts object, that so reside in a single
// ^^^^ server.

public class PatcherClipboardManager implements ClipboardOwner
{
  private static PatcherClipboardManager clipboardManager;

  private FtsClipboard ftsClipboard;
  private String       textClipboard = "";
  
  private MaxVector ftsObjectsPasted = new MaxVector();
  private MaxVector ftsConnectionsPasted = new MaxVector();
  
  private boolean pasting = false;

  /****************************************************************************/
  /*                                                                          */
  /*           CONSTRUCTOR                                                      */
  /*                                                                          */
  /****************************************************************************/

  public PatcherClipboardManager() 
  {
     try 
      {
	Fts fts = MaxApplication.getFts();
	ftsClipboard = (FtsClipboard) fts.makeFtsObject( fts.getRootObject(), "__clipboard");
      }
    catch (FtsException e) 
      {
      }
  }
  
  public static void createManager() {
    if (clipboardManager == null)
      clipboardManager = new PatcherClipboardManager();
  }

  /**
   * returns the manager
   */
  public static PatcherClipboardManager getManager(){
    return clipboardManager;
  }

  /****************************************************************************/
  /*                                                                          */
  /*   ACTIONS                                                                */
  /*                                                                          */
  /****************************************************************************/

  public void Cut(ErmesSketchWindow sketchWindow)
  {
    ErmesSketchPad sketch = sketchWindow.getSketchPad();
    if (sketch.canCopyText())
      {
	sketch.cutText();
      }
    else if (ErmesSelection.patcherSelection.ownedBy(sketch)&&!ErmesSelection.patcherSelection.isEmpty())
      {
	Cursor temp = sketchWindow.getCursor();
	sketchWindow.setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));
	
	MaxApplication.systemClipboard.setContents(ErmesSelection.patcherSelection, this);
	ftsClipboard.copy(sketch.getFts().getSelection());
	sketch.setLastCopyCount(ftsClipboard.getCopyCount());
	sketch.resetPaste(-1);
	ErmesSelection.patcherSelection.redraw();
	ErmesSelection.patcherSelection.deleteAll();

	sketchWindow.setCursor( temp);
      }
  }

  public void Copy(ErmesSketchWindow sketchWindow)
  {
    ErmesSketchPad sketch = sketchWindow.getSketchPad();
    
    if (sketch.canCopyText())
      {
	sketch.copyText();
      }
    else 
      if (ErmesSelection.patcherSelection.ownedBy(sketch)&&!ErmesSelection.patcherSelection.isEmpty())
	{
	  Cursor temp = sketchWindow.getCursor();
	  sketchWindow.setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));
	  
	  MaxApplication.systemClipboard.setContents(ErmesSelection.patcherSelection, this);
	  ftsClipboard.copy( sketch.getFts().getSelection());
	  sketch.setLastCopyCount(ftsClipboard.getCopyCount());
	  sketch.resetPaste(0);
	  
	  sketchWindow.setCursor( temp);
	}
  }

  /**
   * Paste the content of clipboard.
   * The patcher supports text and "patcherSelectionFlavor" DataFlavor.
   * A text can be pasted only if an editable field is currently opened,
   * and a patcher selection can be pasted if we're not currently editing
   * a text. */

  public void Paste(ErmesSketchWindow sketchWindow)
  {
    ErmesSketchPad sketch = sketchWindow.getSketchPad();

    if (sketchWindow.isLocked())
      sketchWindow.setLocked(false);
    
    Transferable clipboardContent = MaxApplication.systemClipboard.getContents(this);

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
    else if (clipboardContent.isDataFlavorSupported(ErmesSelection.patcherSelectionFlavor))
      {
	Cursor temp = sketchWindow.getCursor();
    
	sketchWindow.setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));

	// Should get the fts clipboard from the System.clipboard content !!!!!

	pasting = true;

	if (sketch.getLastCopyCount() != ftsClipboard.getCopyCount())
	  {
	    sketch.resetPaste(-1);
	    sketch.setLastCopyCount(ftsClipboard.getCopyCount());
	  }

	ftsClipboard.paste( sketch.getFtsPatcher());

	sketch.getFtsPatcherData().update();

	sketch.getFts().sync();

	pasting = false;
	
	// make the sketch do the graphic job

	if (!ftsObjectsPasted.isEmpty() || ! ftsConnectionsPasted.isEmpty())
	  {
	    PasteObjects(sketch);
	    sketch.fixSize();
	  }
	  
	ftsObjectsPasted.removeAllElements();
	ftsConnectionsPasted.removeAllElements();

	sketchWindow.setCursor( temp);
      }
  }

  void PasteObjects(ErmesSketchPad sketch) 
  {
    FtsObject	fo;
    FtsConnection fc;
    DisplayList displayList = sketch.getDisplayList();
    GraphicObject object;
    GraphicConnection connection;
    int  incrementalPasteOffsetX;
    int  incrementalPasteOffsetY;
    int pasteNum;

    pasteNum = sketch.getPasteNumber();

    if (sketch.isTextEditingObject())
      sketch.stopTextEditing();

    ErmesSelection.patcherSelection.setOwner(sketch); 

    if (! ErmesSelection.patcherSelection.isEmpty())
      {
	ErmesSelection.patcherSelection.redraw(); 
	ErmesSelection.patcherSelection.deselectAll();
      }

    fo = (FtsObject)ftsObjectsPasted.elementAt( 0);

    if (pasteNum == 0) 
	sketch.setIncrementalPasteOffsets(0, 0);
    else if (pasteNum == 1) 
      {
	sketch.setOldPastedObject(fo);
	sketch.setIncrementalPasteOffsets(20, 20);
      }
    else if (pasteNum == 2) 
      sketch.setIncrementalPasteOffsets(sketch.getOldPastedObject().getX() - fo.getX(),
					sketch.getOldPastedObject().getY() - fo.getY());

    for ( Enumeration e = ftsObjectsPasted.elements(); e.hasMoreElements();) 
      {
	fo = (FtsObject)e.nextElement();

	int newPosX = fo.getX() + pasteNum*sketch.getPasteOffsetX();
	int newPosY = fo.getY() + pasteNum*sketch.getPasteOffsetY();
	
	fo.setX( newPosX);
	fo.setY( newPosY);

	object = GraphicObject.makeGraphicObject( sketch, fo);
	displayList.add( object);
	ErmesSelection.patcherSelection.select( object);
	object.redraw();
      }

    GraphicObject fromObj, toObj;
    
    for ( Enumeration e2 = ftsConnectionsPasted.elements(); e2.hasMoreElements();) 
      {
	fc = (FtsConnection)e2.nextElement();

	connection = new GraphicConnection( sketch, 
					  displayList.getGraphicObjectFor(fc.getFrom()),
					  fc.getFromOutlet(), 
					  displayList.getGraphicObjectFor(fc.getTo()),
					  fc.getToInlet(),
					  fc);

	displayList.add( connection);

	ErmesSelection.patcherSelection.select( connection);
	connection.updateDimensions();
	connection.redraw();
      }

    displayList.reassignLayers();
    displayList.sortDisplayList();
  }


  public boolean ftsClipboardIsEmpty()
  {
    return ftsClipboard.isEmpty();
  }

  public void lostOwnership(Clipboard c, Transferable t) 
  {
  }

  public void Duplicate(ErmesSketchWindow sketchWindow)
  {
    ErmesSketchPad sketch = sketchWindow.getSketchPad();

    if((!ErmesSelection.patcherSelection.ownedBy(sketch))||(ErmesSelection.patcherSelection.isEmpty()))
      return;

    Copy(sketchWindow); 
    Paste(sketchWindow);


    // (fd) if only one object duplicated, then edit it with all text selected

    if (ErmesSelection.patcherSelection.isSingleton())
      {
	GraphicObject obj = (GraphicObject)ErmesSelection.patcherSelection.getSingleton();

	if (obj instanceof Editable)
	  {
	    ErmesSelection.patcherSelection.deselectAll();

	    sketch.textEditObject((Editable)obj);
	    sketch.getEditField().selectAll();
	  }
      }
  }

  public boolean isPasting()
  {
    return pasting;
  }
  
  void addPastedObject(FtsObject obj)
  {
    ftsObjectsPasted.addElement( obj);
  }

  void addPastedConnection(FtsConnection c)
  {
    ftsConnectionsPasted.addElement( c);
  }
}







