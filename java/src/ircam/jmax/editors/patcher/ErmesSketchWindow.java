//
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 
package ircam.jmax.editors.patcher;

import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;
import java.util.*;
import java.io.*;
import java.text.*;
import javax.swing.*;

import tcl.lang.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.menus.*;

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

public class ErmesSketchWindow extends JFrame implements ComponentListener, WindowListener, ClipboardOwner
{
  static private FtsClipboard ftsClipboard;
  static private String       textClipboard = "";

  static {
    try 
      {
	Fts fts = MaxApplication.getFts();
	ftsClipboard = (FtsClipboard) fts.makeFtsObject( fts.getRootObject(), "__clipboard");
      }
    catch (FtsException e) 
      {
      }
  }


  private MaxVector ftsObjectsPasted = new MaxVector();
  private MaxVector ftsConnectionsPasted = new MaxVector();

  public ErmesSketchPad itsSketchPad;
  JScrollPane  itsScrollerView;
  ErmesToolBar itsToolBar;
  JLabel       itsMessageLabel;
  public FtsObject itsPatcher;
  public FtsPatcherData itsPatcherData;

  private FileMenu itsFileMenu;
  private EditMenu itsEditMenu;	
  private JMenu itsWindowsMenu;
  private JMenu itsToolsMenu;
  private TextMenu itsTextMenu;
  private JMenu itsHelpMenu;

  private Fts fts;

  public MaxDocument itsDocument;

  public void showObject( Object obj)
  {
    itsSketchPad.showObject( obj);
  }

  /****************************************************************************/
  /*                                                                          */
  /*           CONSTRUCTOR                                                      */
  /*                                                                          */
  /****************************************************************************/

  public ErmesSketchWindow( FtsPatcherData patcherData) 
  {
    super("");

    fts = patcherData.getFts();

    MaxWindowManager.getWindowManager().addWindow(this);

    // Initialize state

    itsDocument = patcherData.getDocument();
    itsPatcher = patcherData.getContainerObject();
    itsPatcherData = patcherData;

    itsPatcherData.setPatcherListener(new ErmesPatcherListener(this));

    // Make the title

    makeTitle();

    // Make the content

    itsSketchPad = new ErmesSketchPad(fts, this, itsPatcherData);
    itsToolBar = new ErmesToolBar( itsSketchPad);

    itsScrollerView = new JScrollPane();
    itsScrollerView.setViewportView( itsSketchPad); 
    itsScrollerView.getHorizontalScrollBar().setUnitIncrement( 10);
    itsScrollerView.getVerticalScrollBar().setUnitIncrement( 10);
    itsScrollerView.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
    itsScrollerView.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);

    // Build The Menus and Menu Bar

    JMenuBar mb = new JMenuBar();

    // Build the file menu

    itsFileMenu = new FileMenu();
    mb.add(itsFileMenu); 

    // Build the edit menu

    itsEditMenu = new EditMenu(this); 
    mb.add(itsEditMenu); 

    // Build the text menu

    itsTextMenu = new TextMenu(this);
    mb.add( itsTextMenu);

    // New Tool menu 

    itsToolsMenu = new ircam.jmax.toolkit.MaxToolsJMenu("Tools"); 
    mb.add(itsToolsMenu);

    // New Window Manager based Menu

    itsWindowsMenu = new ircam.jmax.toolkit.MaxWindowJMenu("Windows", this); 
    mb.add(itsWindowsMenu);

    // Add some separation between help and the others.

    mb.add(Box.createHorizontalGlue());

    // Build up the help Menu 

    itsHelpMenu = new HelpMenu(this);
    //mb.setHelpMenu( itsHelpMenu);
    mb.add( itsHelpMenu);

    setJMenuBar(mb);
    
    // inform the Sketch of the ToolBar to 

    itsSketchPad.setToolBar( itsToolBar);

    // getContentPane().setLayout( new BorderLayout()); // NOT NECESSARY
    
    // setSize( new Dimension( 600, 300)); // ???

    itsMessageLabel = new JLabel("   ");

    getContentPane().add( itsToolBar, BorderLayout.NORTH);
    getContentPane().add( itsScrollerView, BorderLayout.CENTER);
    getContentPane().add( itsMessageLabel, BorderLayout.SOUTH);

    // Compute its Initial Size

    InitFromContainer( itsPatcher); 

    validate();

    addComponentListener( this); 
    addWindowListener(this); 
    
    // To set the initial state: set to edit mode only if the
    // initialMode property of a patcher is set and it is set
    // to something different than "run" (usually, "edit" :)

    if (itsPatcherData.getRecursiveEditMode() == FtsPatcherData.EDIT_MODE)
      setLocked( false);
    else
      setLocked( true);

    // Fix the sketch size if needed ????

    itsSketchPad.fixSize();

    // Finally, activate the updates

    itsPatcherData.startUpdates();

    // Make it visible, at the end

    setVisible( true);
  }

  private final void makeTitle()
  {
    String name;

    if (itsDocument.isRootData(itsPatcherData))
      name = itsDocument.getName();
    else if (itsPatcher instanceof FtsPatcherObject)
      {
	name = "patcher " + itsPatcher.getDescription();
      }
    else
      name = "template " + itsPatcher.getClassName();

    setTitle(MaxWindowManager.getWindowManager().makeUniqueWindowTitle(name));

    MaxWindowManager.getWindowManager().windowChanged(this);
  }

  int horizontalOffset()
  {
    return 40;
  }
  
  int verticalOffset() 
  {
    return 130;
  }
  
  private void InitFromContainer( FtsObject patcher)
  {
    int x;
    int y;
    int width;
    int height;

    //Double check the existence of the window properties. If there aren't, use defaults
      
    x = itsPatcherData.getWindowX();
    y = itsPatcherData.getWindowY();
    width = itsPatcherData.getWindowWidth();
    height = itsPatcherData.getWindowHeight();

    if (width <= 0)
      width = 480;

    if (height <= 0)
      height = 500;

    setBounds( x, y, width + horizontalOffset(), height + verticalOffset());
  }


  public Fts getFts()
  {
    return fts;
  }

  /****************************************************************************/
  /*                                                                          */
  /*           MENU ACTIONS                                                   */
  /*                                                                          */
  /****************************************************************************/


  public void Cut()
  {
    if (itsSketchPad.canCopyText())
      {
	itsSketchPad.cutText();
      }
    else if (ErmesSelection.patcherSelection.ownedBy(itsSketchPad))
      {
	Cursor temp = getCursor();
	setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));
	ftsClipboard.copy( fts.getSelection());
	lastCopyCount = ftsClipboard.getCopyCount();
	itsSketchPad.resetPaste(0);
	ErmesSelection.patcherSelection.redraw();
	ErmesSelection.patcherSelection.deleteAll();

	setCursor( temp);
      }
  }

  private int lastCopyCount;

  public void Copy()
  {
    if (itsSketchPad.canCopyText())
      {
	itsSketchPad.copyText();
      }
    else if (ErmesSelection.patcherSelection.ownedBy(itsSketchPad))
      {
	MaxApplication.systemClipboard.setContents(ErmesSelection.patcherSelection, this);
	Cursor temp = getCursor();
	setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));

	ftsClipboard.copy( fts.getSelection());
	lastCopyCount = ftsClipboard.getCopyCount();
	itsSketchPad.resetPaste(0);

	setCursor( temp);
      }
  }

  private boolean pasting = false;

  /**
   * Paste the content of clipboard.
   * The patcher supports text and "patcherSelectionFlavor" DataFlavor.
   * A text can be pasted only if an editable field is currently opened,
   * and a patcher selection can be pasted if we're not currently editing
   * a text. */

  public void Paste()
  {
    if (isLocked())
      setLocked(false);

    Transferable clipboardContent = MaxApplication.systemClipboard.getContents(this);

    if (clipboardContent == null)
      return;  // Should never happen

    if (clipboardContent.getTransferDataFlavors() == null)
      return; // Should never happen, protection against system clipboard bug.

    if (clipboardContent.isDataFlavorSupported(DataFlavor.stringFlavor))
      {
	if (itsSketchPad.canPasteText())
	  {
	    try
	      {
		itsSketchPad.pasteText();
	      }
	    catch (Exception e)
	      {
		System.err.println("error while pasting text: " + e);
	      }
	  }
      }
    else if (clipboardContent.isDataFlavorSupported(ErmesSelection.patcherSelectionFlavor))
      {
	Cursor temp = getCursor();
    
	setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));

	// Should get the fts clipboard from the System.clipboard content !!!!!

	pasting = true;

	if (lastCopyCount != ftsClipboard.getCopyCount())
	  {
	    itsSketchPad.resetPaste(-1);
	    lastCopyCount = ftsClipboard.getCopyCount();
	  }

	ftsClipboard.paste( itsPatcher);

	itsPatcherData.update();
	fts.sync();

	pasting = false;

	// make the sketch do the graphic job

	if (!ftsObjectsPasted.isEmpty() || ! ftsConnectionsPasted.isEmpty())
	  {
	    itsSketchPad.PasteObjects( ftsObjectsPasted, ftsConnectionsPasted);
	    itsSketchPad.fixSize();
	  }

	ftsObjectsPasted.removeAllElements();
	ftsConnectionsPasted.removeAllElements();

	setCursor( temp);
      }
  }

  public static boolean ftsClipboardIsEmpty()
  {
    return ftsClipboard.isEmpty();
  }

  public void lostOwnership(Clipboard c, Transferable t) 
  {
  }

  public void Duplicate()
  {
    Copy(); 
    Paste();

    // (fd) if only one object duplicated, then edit it with all text selected

    if (ErmesSelection.patcherSelection.isSingleton())
      {
	GraphicObject obj = (GraphicObject)ErmesSelection.patcherSelection.getSingleton();

	if (obj instanceof Editable)
	  {
	    ErmesSelection.patcherSelection.deselectAll();

	    itsSketchPad.textEditObject((Editable)obj);
	    itsSketchPad.getEditField().selectAll();
	  }
      }
  }

  boolean isPasting()
  {
    return pasting;
  }
  
  // Pasting should not be done this way,
  // it should be done on the fly, without
  // accumulating results

  void addPastedObject(FtsObject obj)
  {
    ftsObjectsPasted.addElement( obj);
  }

  void addPastedConnection(FtsConnection c)
  {
    ftsConnectionsPasted.addElement( c);
  }

  public void Close(boolean doCancel)
  {
    if (! itsDocument.isRootData(itsPatcherData))
      {
	itsPatcherData.stopUpdates();
	Mda.dispose(itsPatcherData); // experimental
	// setVisible( false);
      }
    else 
      {
	if ( ShouldSave()) 
	  {
	    FileNotSavedDialog aDialog = new FileNotSavedDialog( this, itsDocument, doCancel);

	    aDialog.setLocation( 300, 300);
	    aDialog.setVisible( true);

	    if (aDialog.getNothingToDoFlag())
	      return;

	    if (aDialog.getToSaveFlag())
	      Save();

	    aDialog.dispose();
	  }

	// Just call dispose on the document
	// Mda will indirectly call Destroy, and will close all the other editors

	itsDocument.dispose();
      }
  }

  // Method to close the editor (do not touch the patcher)

  public void Destroy()
  {
    setVisible(false);

    itsPatcherData.resetPatcherListener();
    removeComponentListener( this);
    removeWindowListener(this);
    itsSketchPad.cleanAll();

    itsSketchPad = null;
    itsPatcher = null;
    itsPatcherData = null;
    itsDocument = null;

    itsDocument = null;
    ftsObjectsPasted = null;
    ftsConnectionsPasted = null;
    itsScrollerView = null;

    dispose();
  }

  public boolean ShouldSave()
  {
    return (itsDocument.isRootData(itsPatcherData) && (! itsDocument.isSaved()));
  }

  public void Save()
  {
    // first, tentative implementation:
    // the FILE is constructed now, and the ErmesSketchPad SaveTo method is invoked.
    // we should RECEIVE this FILE, or contruct it when we load this document
		
    // The "canSave" method of a data tell if it can be saved
    // i.e. if it have a document , and if we can write to its document file

    // Change in semantic: now Save() is active *only* on root level patchers 
    // SHOULD BECOME Gray in the others

    if (! itsDocument.isRootData(itsPatcherData))
      {
	new ErrorDialog( this, "Only root patchers can be saved");
	return;
      }

    if (itsDocument.canSave())
      {
	try
	  {
	    itsDocument.save();
	  }
	catch ( MaxDocumentException e)
	  {
	    new ErrorDialog( this, e.getMessage());
	  }
      }
    else
      SaveAs();
  }

  public void SaveAs()
  {
    File file;

    if (! itsDocument.isRootData(itsPatcherData))
      {
	new ErrorDialog( this, "Only root patchers can be saved");
	return;
      }

    file = MaxFileChooser.chooseFileToSave( this, itsDocument.getDocumentFile(), "Save As");

    if (file == null)
      return;
    else
      itsDocument.bindToDocumentFile( file);

    setTitle( file.toString()); 

    try
      {
	itsDocument.save();
      }
    catch ( MaxDocumentException e)
      {
	new ErrorDialog( this, e.getMessage());
      }
  }

  public void SaveTo()
  {
    File file;

    file = MaxFileChooser.chooseFileToSave( this, itsDocument.getDocumentFile(), "Save To");

    if (file == null)
      return;

    try
      {
	if (itsDocument.isRootData(itsPatcherData))
	  {
	    // Make a document save to
	    itsDocument.saveTo( file);
	  }
	else
	  {
	    // Make a subdocument save to
	    itsDocument.saveSubDocumentTo( itsPatcherData, file);
	  }
      }
    catch ( MaxDocumentException e)
      {
	new ErrorDialog( this, e.getMessage());
      }
  }

  public void Print()
  {
    RepaintManager.currentManager(itsSketchPad).setDoubleBufferingEnabled(false);

    PrintJob aPrintjob = getToolkit().getPrintJob( this, "Printing Patcher", MaxApplication.getProperties());

    if (aPrintjob != null)
      {
	Graphics aPrintGraphics = aPrintjob.getGraphics();

	if (aPrintGraphics != null)
	  {
	    itsSketchPad.print( aPrintGraphics);
	    aPrintGraphics.dispose();
	  }
	aPrintjob.end();
      }

    RepaintManager.currentManager(itsSketchPad).setDoubleBufferingEnabled(true);
  }

  /****************************************************************************/
  /*                                                                          */
  /*           LOCK MODE HANDLING                                             */
  /*                                                                          */
  /****************************************************************************/

  public void setLocked( boolean locked)
  {
    // Store the mode in a non persistent, property of 
    // the patch, so that subpatcher can use it as their initial mode

    if (locked)
      itsPatcherData.setEditMode(FtsPatcherData.RUN_MODE);
    else
      itsPatcherData.setEditMode(FtsPatcherData.EDIT_MODE);

    itsSketchPad.setLocked( locked); 

    itsToolBar.setLocked( locked);

    itsSketchPad.setKeyEventClient( null); //when changing mode, always remove key listeners

    itsSketchPad.requestFocus();
  }

  final public boolean isLocked()
  {
    return itsSketchPad.isLocked();
  }

  /****************************************************************************/
  /*                                                                          */
  /*           LISTENERS                                                      */
  /*                                                                          */
  /****************************************************************************/


  // Window Listener Interface 

  public void windowClosing(WindowEvent e)
  {
    Close(false);
  }

  public void windowOpened(WindowEvent e)
  {
  }

  public void windowClosed(WindowEvent e)
  {
  }

  public void windowIconified( WindowEvent e)
  {
    // Do the test because the awt can call this before itsPatcher is ready
    if (itsPatcherData != null)
      itsPatcherData.stopUpdates();
  }       

  public void windowDeiconified( WindowEvent e)
  {
    // Do the test because the awt can call this before itsPatcher is ready
    if (itsPatcherData != null)
      itsPatcherData.startUpdates();
  }       

  public void windowActivated(WindowEvent e)
  {
    itsSketchPad.requestFocus();
  }

  public void windowDeactivated(WindowEvent e)
  {
  }

  // Component Listener Interface

  public void componentResized( ComponentEvent e) 
  {
    if (itsPatcher != null) 
      {
	itsPatcherData.setWindowWidth(getSize().width - horizontalOffset());
	itsPatcherData.setWindowHeight(getSize().height - verticalOffset());
	itsSketchPad.fixSize();
      }
  }

  public void componentMoved( ComponentEvent e) 
  {
    if (itsPatcher != null)
      {
	itsPatcherData.setWindowX(getLocation().x);
	itsPatcherData.setWindowY(getLocation().y);
      }
  }

  public void componentShown( ComponentEvent e) 
  {
  }

  public void componentHidden( ComponentEvent e)
  {
  }

  boolean gotMessage = false;

  public void showMessage(String text)
  {
    itsMessageLabel.setText(text);
    gotMessage = true;
  }

  public void resetMessage()
  {
    if (gotMessage)
      {
	itsMessageLabel.setText("    ");
	gotMessage = false;
      }
  }

  public boolean isMessageReset()
  {
    return (! gotMessage);
  }
}







