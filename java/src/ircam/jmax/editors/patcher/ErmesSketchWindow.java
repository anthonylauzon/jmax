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
public class ErmesSketchWindow extends JFrame implements ComponentListener, WindowListener, ClipboardOwner
{

  // Primitive and fast implementation of multi-type clipboard.
  // Enzo, forgive me ... it will be done as a clipboard provider
  // later ..
  
  static private FtsClipboard ftsClipboard;
  static private String       textClipboard = "";

  static {
    try 
      {
	ftsClipboard = (FtsClipboard) Fts.makeFtsObject( Fts.getServer().getRootObject(), "__clipboard");
      }
    catch (FtsException e) 
      {
	System.out.println("warning: failed to create an Fts clipboard");
      }
  }


  private MaxVector ftsObjectsPasted = new MaxVector();
  private MaxVector ftsConnectionsPasted = new MaxVector();

  public ErmesSketchPad itsSketchPad;
  JScrollPane itsScrollerView;
  ErmesToolBar itsToolBar;
  public FtsObject itsPatcher;
  public FtsPatcherData itsPatcherData;

  private FileMenu itsFileMenu;
  private EditMenu itsEditMenu;	
  private JMenu itsWindowsMenu;
  private JMenu itsToolsMenu;
  private TextMenu itsTextMenu;
  private JMenu itsHelpMenu;

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

    MaxWindowManager.getWindowManager().addWindow(this);

    // Initialize state

    itsDocument = patcherData.getDocument();
    itsPatcher = patcherData.getContainerObject();
    itsPatcherData = patcherData;

    itsPatcherData.setPatcherListener(new ErmesPatcherListener(this));

    // Make the title

    makeTitle();

    // Make the content

    itsSketchPad = new ErmesSketchPad( this, itsPatcherData);
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

    itsEditMenu = new EditMenu(); 
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
    
    Fts.getSelection().clean();

    setSize( new Dimension( 600, 300));
    getContentPane().add( itsToolBar, BorderLayout.NORTH);
    getContentPane().add( itsScrollerView, BorderLayout.CENTER);

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
	if (itsPatcher.getObjectName() != null)
	  name = "patcher " + itsPatcher.getObjectName();
	else
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


  /****************************************************************************/
  /*                                                                          */
  /*           SELECTION CHANGED UPDATER                                      */
  /*                                                                          */
  /****************************************************************************/

  /**
   * Called when the selection change; the method have to find up 
   * what is the proper action to do for each Menu 
   */

  void selectionChanged()
  {
    itsEditMenu.selectionChanged();
    itsTextMenu.selectionChanged();
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
	textClipboard = itsSketchPad.getSelectedText(); 
	itsSketchPad.deleteSelectedText();
      }
    else if (ErmesSelection.patcherSelection.ownedBy(itsSketchPad))
      {
	Cursor temp = getCursor();
	setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));
	ftsClipboard.copy( Fts.getSelection());
	lastCopyCount = ftsClipboard.getCopyCount();
	itsSketchPad.resetPaste(0);
	ErmesSelection.patcherSelection.deleteAll();

	setCursor( temp);
      }
  }

  private int lastCopyCount;

  public void Copy()
  {
    if (itsSketchPad.canCopyText())
      {
	textClipboard = itsSketchPad.getSelectedText(); 
	MaxApplication.systemClipboard.setContents(new StringSelection(textClipboard), this);
      }
    else if (ErmesSelection.patcherSelection.ownedBy(itsSketchPad))
      {
	MaxApplication.systemClipboard.setContents(ErmesSelection.patcherSelection, this);
	Cursor temp = getCursor();
	setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));

	ftsClipboard.copy( Fts.getSelection());
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

    if (clipboardContent.isDataFlavorSupported(DataFlavor.stringFlavor))
      {
	if (itsSketchPad.canPasteText())
	  {
	    try
	      {
		textClipboard = (String) clipboardContent.getTransferData(DataFlavor.stringFlavor);
		itsSketchPad.pasteText(textClipboard);
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

	// @@@@: Should get the fts clipboard from the System.clipboard content !!!!!

	pasting = true;

	if (lastCopyCount != ftsClipboard.getCopyCount())
	  {
	    itsSketchPad.resetPaste(-1);
	    lastCopyCount = ftsClipboard.getCopyCount();
	  }

	ftsClipboard.paste( itsPatcher);

	itsPatcherData.update();
	Fts.sync();

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
	ErmesObject obj = (ErmesObject)ErmesSelection.patcherSelection.getSingleton();

	if (obj instanceof ErmesObjEditableObject)
	  {
	    ErmesSelection.patcherSelection.deselectAll();

	    itsSketchPad.textEditObject((ErmesObjEditableObject)obj);
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
	    itsSketchPad.printAll( aPrintGraphics);
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

    itsEditMenu.setLocked(locked);

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


}







