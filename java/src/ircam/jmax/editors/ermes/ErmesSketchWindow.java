package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;
import java.util.*;
import java.io.*;
import java.text.*;

import tcl.lang.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import ircam.jmax.dialogs.*;
import javax.swing.*;


//
// The window that contains the sketchpad. It knows the ftspatcher it is editing.
// It handles all the sketch menus, it knows how to load from a ftspatcher.
//
public class ErmesSketchWindow extends MaxEditor implements ComponentListener {

  protected KeyEventClient keyEventClient;

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

  public static int preferredWidth = 490;
  public static int preferredHeight = 450;
  Dimension preferredsize = new Dimension( preferredWidth,preferredHeight);
  public ErmesSketchPad itsSketchPad;
  ScrollPane itsScrollerView;
  ErmesSwToolbar itsToolBar;
  static String[] itsFontList = Toolkit.getDefaultToolkit().getFontList();
  public FtsObject itsPatcher;
  public FtsPatcherData itsPatcherData;

  private Menu itsObjectsMenu;
  private Menu itsAlignMenu;
  private Menu itsSizesMenu;
  private Menu itsFontsMenu;
  private Menu itsHelpMenu;

  CheckboxMenuItem itsSelectedSizeMenu;   //the Selected objects size MenuItem
  CheckboxMenuItem itsSketchSizeMenu;     //the SketchPad size MenuItem
  CheckboxMenuItem itsSketchFontMenu;     //the SketchPad font MenuItem
  CheckboxMenuItem itsSelectedFontMenu;   //the Selected objects font MenuItem

  MenuItem itsSelectAllMenuItem;
  MenuItem itsLockMenuItem;
  public MaxDocument itsDocument;

  public void showObject( Object obj)
  {
    itsSketchPad.showObject( obj);
  }

  //
  // Constructor 
  //

  public ErmesSketchWindow( FtsPatcherData patcherData) 
  {
    super( Mda.getDocumentTypeByName( "patcher"));

    // (fd)
    keyEventClient = null;

    itsDocument = patcherData.getDocument();
    itsPatcher = patcherData.getContainerObject();
    itsPatcherData = patcherData;

    itsPatcherData.setPatcherListener(new ErmesPatcherListener(this));

    makeTitle();

    itsSketchPad = new ErmesSketchPad( this, itsPatcherData);
    itsToolBar = new ErmesSwToolbar( itsSketchPad);

    itsScrollerView = new ScrollPane();
    itsScrollerView.add( itsSketchPad, 0); 
    itsScrollerView.getHAdjustable().setUnitIncrement( 10);
    itsScrollerView.getVAdjustable().setUnitIncrement( 10);
    itsScrollerView.getHAdjustable().addAdjustmentListener( itsSketchPad); 
    itsScrollerView.getVAdjustable().addAdjustmentListener( itsSketchPad); 
    
    Init(); // MaxEditor base class init (standard menu handling)

    itsSketchPad.SetToolBar( itsToolBar);	// inform the Sketch of the ToolBar to 

    getContentPane().setLayout( new BorderLayout());
    
    Fts.getSelection().clean();

    setSize( new Dimension( 600, 300));
    getContentPane().add( "North", itsToolBar);
    getContentPane().add( "Center", itsScrollerView);

    InitFromContainer( itsPatcher); 

    validate();

    addComponentListener( this); 
    
    // To set the initial state: set to edit mode only if the
    // initialMode property of a patcher is set and it is set
    // to something different than "run" (usually, "edit" :)

    if (itsPatcherData.getRecursiveEditMode() == FtsPatcherData.EDIT_MODE)
      setLocked( false);
    else
      setLocked( true);

    // Finally, activate the updates

    itsPatcherData.startUpdates();

    // Make it visible, at the end

    setVisible( true);
  }

  private final void makeTitle()
  {
    if (itsDocument.isRootData(itsPatcherData))
      setTitle(itsDocument.getName());
    else if (itsPatcher instanceof FtsPatcherObject)
      {
	if (itsPatcher.getObjectName() != null)
	  setTitle("patcher " + itsPatcher.getObjectName());
	else
	  setTitle("patcher " + itsPatcher.getDescription());
      }
    else
      setTitle("template " + itsPatcher.getClassName());
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

  public void SetupMenu()
  {
    MenuBar menuBar = getMenuBar();

    Menu editMenu = getEditMenu();

    editMenu.remove(getUndoMenu());
    editMenu.remove(getRedoMenu());

    editMenu.addSeparator();

    itsSelectAllMenuItem = new MenuItem( "Select All", new MenuShortcut(KeyEvent.VK_A));
    editMenu.add( itsSelectAllMenuItem);
    itsSelectAllMenuItem.addActionListener( new MaxActionListener(itsSelectAllMenuItem) {
      public  void actionPerformed( ActionEvent e)
	{
	  itsSketchPad.SelectAll();
	}
    });

    editMenu.add( new MenuItem( "-"));

    MenuItem findMenuItem = new MenuItem( "Find");
    editMenu.add( findMenuItem);
    findMenuItem.addActionListener( new MaxActionListener(findMenuItem) {
      public void actionPerformed( ActionEvent e)
 	{
	  FindPanel p;
	  p = FindPanel.open();

	  if (Fts.getSelection().getObjects().size() > 0)
	    {
	      p.findFriends((FtsObject) Fts.getSelection().getObjects().elementAt( 0));
	    }
 	}
    });

    MenuItem findErrorsMenuItem = new MenuItem( "Find Errors");
    editMenu.add( findErrorsMenuItem);
    findErrorsMenuItem.addActionListener( new MaxActionListener(findErrorsMenuItem) {
      public void actionPerformed( ActionEvent e)
 	{
	  FindPanel p;
	  p = FindPanel.open();
	  p.findErrors();
 	}
    });

    MenuItem inspectMenuItem = new MenuItem("Inspect", new MenuShortcut(KeyEvent.VK_I));
    editMenu.add(new MenuItem("-"));
    editMenu.add(inspectMenuItem);

    inspectMenuItem.addActionListener( new MaxActionListener(inspectMenuItem) {
      public void actionPerformed(ActionEvent e) 
	{
	  inspectAction();
	}
    });

    editMenu.add( new MenuItem( "-"));

    itsLockMenuItem = new MenuItem( "Lock", new MenuShortcut(KeyEvent.VK_E));
    editMenu.add( itsLockMenuItem);
    itsLockMenuItem.addActionListener( new MaxActionListener(itsLockMenuItem) {
      public void actionPerformed( ActionEvent e)
 	{
	  setLocked(! isLocked());
 	}
    });

    getCutMenu().setEnabled( true);
    getCopyMenu().setEnabled( true);
    getPasteMenu().setEnabled( true);
    getDuplicateMenu().setEnabled( true);
    
    // Add the Objects menu
    itsObjectsMenu = new Menu( "Objects");
    menuBar.add( itsObjectsMenu);

    itsSizesMenu = new Menu( "Sizes");
    FillSizesMenu( itsSizesMenu);
    itsObjectsMenu.add( itsSizesMenu);

    itsFontsMenu =  new Menu( "Fonts");
    FillFontMenu( itsFontsMenu);
    itsObjectsMenu.add( itsFontsMenu);

    itsAlignMenu =  new Menu( "Align");
    FillAlignMenu( itsAlignMenu);
    itsObjectsMenu.add( itsAlignMenu);

    CheckDefaultSizeFontMenuItem(); 
    CheckDefaultFontItem(); 

    /* Build up the edit Menu */

    itsHelpMenu = new Menu( "Help");
    menuBar.setHelpMenu( itsHelpMenu);
    FillHelpMenu(itsHelpMenu);
  }

  public void inspectAction()
  {
    if (ErmesSketchPad.currentSelection.getOwner() == itsSketchPad)
      ErmesSketchPad.inspectSelection();
  }

  protected void Cut()
  {
    if (itsSketchPad.canCopyText())
      {
	textClipboard = itsSketchPad.getSelectedText(); 
	itsSketchPad.deleteSelectedText();
      }
    else if (ErmesSketchPad.currentSelection.getOwner() == itsSketchPad)
      {
	Cursor temp = getCursor();
	setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));
	ftsClipboard.copy( Fts.getSelection());
	lastCopyCount = ftsClipboard.getCopyCount();
	itsSketchPad.resetPaste(0);
	itsSketchPad.DeleteSelected();

	setCursor( temp);
      }
  }

  private int lastCopyCount;

  protected void Copy()
  {
    if (itsSketchPad.canCopyText())
      {
	textClipboard = itsSketchPad.getSelectedText(); 
      }
    else if (ErmesSketchPad.currentSelection.getOwner() == itsSketchPad)
      {
	Cursor temp = getCursor();
	setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));

	ftsClipboard.copy( Fts.getSelection());
	lastCopyCount = ftsClipboard.getCopyCount();
	itsSketchPad.resetPaste(0);

	setCursor( temp);
      }
  }

  private boolean pasting = false;

  protected void Paste()
  {
    if (isLocked())
      setLocked(false);

    Cursor temp = getCursor();

    setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));

    if (itsSketchPad.canPasteText())
      {
	itsSketchPad.pasteText(textClipboard);
      }
    else 
      {
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
	    itsSketchPad.RequestOffScreen();
	    itsSketchPad.repaint();
	  }

	ftsObjectsPasted.removeAllElements();
	ftsConnectionsPasted.removeAllElements();
      }

    setCursor( temp);
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

  // Help support

  void FillHelpMenu(Menu menu)
  {
    MenuItem aMenuItem;

    aMenuItem = new MenuItem( ".. on object", new MenuShortcut(KeyEvent.VK_H));
    menu.add( aMenuItem);
    aMenuItem.addActionListener( new MaxActionListener(aMenuItem) {
      public  void actionPerformed( ActionEvent e)
	{
	  Help();
	}
    });

    // Adding the summaries 

    Enumeration en = FtsHelpPatchTable.getSummaries(); 

    while (en.hasMoreElements())
       {
	 final String str = (String) en.nextElement();

	aMenuItem = new MenuItem( str + " summary");
	menu.add( aMenuItem);
	aMenuItem.addActionListener( new MaxActionListener(aMenuItem) {
	  public  void actionPerformed( ActionEvent e)
	    {
	      FtsHelpPatchTable.openHelpSummary( str);
	    }
	});
       }
  }


  void Help()
  {
    if (ErmesSketchPad.currentSelection.getOwner() == itsSketchPad)
      {
	ErmesObject aObject;
      
	for (Enumeration en = ErmesSketchPad.currentSelection.itsObjects.elements(); en.hasMoreElements(); )
	  {
	    aObject = (ErmesObject) en.nextElement();
	
	    if (! FtsHelpPatchTable.openHelpPatch( aObject.itsFtsObject))
	      new ErrorDialog( this, "Sorry, no help for object " + aObject.itsFtsObject.getClassName());
	  }
      }
  }

  // 
  // Methods to handle changes in the patcher data.
  //
  
  void DeleteGraphicObject( FtsObject object)
  {
    itsSketchPad.DeleteGraphicObject(itsSketchPad.getErmesObjectFor(object));
    itsSketchPad.paintDirtyList(); // SHould be a repaint
  }

  void DeleteGraphicConnection( FtsConnection c)
  {
    ErmesConnection conn;

    conn = itsSketchPad.getErmesConnectionFor(c);

    // conn may be null if the connection has been delete by Ermes
    // first; a little hack, the whole deleting business should be cleaned up.
    
    if (conn != null)
      {
	itsSketchPad.DeleteGraphicConnection( itsSketchPad.getErmesConnectionFor(c));
	itsSketchPad.paintDirtyList(); // SHould be a repaint
      }
  }


  private void FillAlignMenu( Menu theAlignObjectMenu)
  {
    MenuItem aMenuItem;

    aMenuItem = new MenuItem( "Top");
    theAlignObjectMenu.add( aMenuItem);
    aMenuItem.addActionListener( new MaxActionListener(aMenuItem) {
      public  void actionPerformed( ActionEvent e)
	{
	  itsSketchPad.AlignSelectedObjects( ((MenuItem)e.getSource()).getLabel());
	}
    });

    aMenuItem = new MenuItem( "Left");
    theAlignObjectMenu.add( aMenuItem);
    aMenuItem.addActionListener( new MaxActionListener(aMenuItem) {
      public  void actionPerformed( ActionEvent e)
	{
	  itsSketchPad.AlignSelectedObjects( ((MenuItem)e.getSource()).getLabel());
	}
    });

    aMenuItem = new MenuItem( "Bottom");
    theAlignObjectMenu.add( aMenuItem);
    aMenuItem.addActionListener( new MaxActionListener(aMenuItem) {
      public  void actionPerformed( ActionEvent e)
	{
	  itsSketchPad.AlignSelectedObjects( ((MenuItem)e.getSource()).getLabel());
	}
    });

    aMenuItem = new MenuItem( "Right");
    theAlignObjectMenu.add( aMenuItem);
    aMenuItem.addActionListener( new MaxActionListener(aMenuItem) {
      public  void actionPerformed( ActionEvent e)
	{
	  itsSketchPad.AlignSelectedObjects( ((MenuItem)e.getSource()).getLabel());
	}
    });
  }

  class SizesMenuAdapter extends MaxItemListener
  {
    CheckboxMenuItem item;
    int size;

    SizesMenuAdapter( CheckboxMenuItem item, int size)
    {
      super(item);
      this.item = item;
      this.size = size;
    }

    public  void itemStateChanged( ItemEvent e)
    {
      SizesMenuAction( item, size);
    }
  }

  private void FillSizesMenu( Menu theSizesMenu)
  {
    CheckboxMenuItem aCheckItem;

    theSizesMenu.add( aCheckItem = new CheckboxMenuItem( "8"));
    aCheckItem.addItemListener( new SizesMenuAdapter( aCheckItem, 8));
    theSizesMenu.add( aCheckItem = new CheckboxMenuItem( "9"));
    aCheckItem.addItemListener( new SizesMenuAdapter( aCheckItem, 9));
    theSizesMenu.add( aCheckItem = new CheckboxMenuItem( "10"));
    aCheckItem.addItemListener( new SizesMenuAdapter( aCheckItem, 10));
    theSizesMenu.add( aCheckItem = new CheckboxMenuItem( "12"));
    aCheckItem.addItemListener( new SizesMenuAdapter( aCheckItem, 12));
    theSizesMenu.add( aCheckItem = new CheckboxMenuItem( "14"));
    aCheckItem.addItemListener( new SizesMenuAdapter( aCheckItem, 14));
    theSizesMenu.add( aCheckItem = new CheckboxMenuItem( "18"));
    aCheckItem.addItemListener( new SizesMenuAdapter( aCheckItem, 18));
    theSizesMenu.add( aCheckItem = new CheckboxMenuItem( "24"));
    aCheckItem.addItemListener( new SizesMenuAdapter( aCheckItem, 24));
    theSizesMenu.add( aCheckItem = new CheckboxMenuItem( "36"));
    aCheckItem.addItemListener( new SizesMenuAdapter( aCheckItem, 36));
    theSizesMenu.add( aCheckItem = new CheckboxMenuItem( "48"));
    aCheckItem.addItemListener( new SizesMenuAdapter( aCheckItem, 48));
  }

  class FontMenuAdapter extends MaxItemListener
  {
    CheckboxMenuItem item;
    String font;

    FontMenuAdapter( CheckboxMenuItem item, String font)
    {
      super(item);
      this.item = item;
      this.font = font;
    }

    public  void itemStateChanged( ItemEvent e)
    {
      FontsMenuAction( item, font);
    }
  }

  private void FillFontMenu( Menu theFontMenu)
  {
    CheckboxMenuItem aCheckItem;
    String aString;

    for ( int i = 0; i < itsFontList.length; i++)
      {
	aString = (String) itsFontList[i];
	theFontMenu.add( aCheckItem = new CheckboxMenuItem( aString));
	aCheckItem.addItemListener( new FontMenuAdapter( aCheckItem, aString));
      }
  }

  private void CheckDefaultSizeFontMenuItem()
  {
    CheckboxMenuItem aCheckboxMenuItem;
    String aFontSize = String.valueOf( itsSketchPad.getSketchFontSize());

    for ( int i = 0; i < 9; i++)
      {
	aCheckboxMenuItem = (CheckboxMenuItem)itsSizesMenu.getItem( i);

	if (aCheckboxMenuItem.getLabel().compareTo( aFontSize) == 0)
	  {
	    itsSketchSizeMenu = aCheckboxMenuItem;
	    itsSelectedSizeMenu = itsSketchSizeMenu;
	    itsSelectedSizeMenu.setState( true);
	    return;
	  }
      }
  }
	
  private void CheckDefaultFontItem()
  {
    CheckboxMenuItem aCheckboxMenuItem;
    String aFont = itsSketchPad.getFont().getName();

    for( int i = 0; i < itsFontList.length; i++)
      {
	aCheckboxMenuItem = (CheckboxMenuItem)itsFontsMenu.getItem( i);
	if ( aCheckboxMenuItem.getLabel().toLowerCase().compareTo( aFont.toLowerCase()) == 0)
	  {
	    itsSketchFontMenu = aCheckboxMenuItem;
	    itsSelectedFontMenu = aCheckboxMenuItem;
	    itsSketchFontMenu.setState( true);
	    return;
	  }
      }
  }
	
  // Modified to use inheritance and call the MaxEditor method
  // for all the standard key bindings

  public void keyPressed( KeyEvent e)
  {
    int aInt = e.getKeyCode();

    itsSketchPad.cleanAnnotations(); // MDC

    //arrows first:
    if ( isAnArrow( aInt)) 
      {
	if (e.isShiftDown()) 
	  {
	    if (e.isControlDown()) 
	      itsSketchPad.resizeSelection( 10, aInt);
	    else 
	      itsSketchPad.moveSelection( 10, aInt);
	  }
	else if (e.isControlDown()) 
	  {
	    if (e.isMetaDown()) 
	      itsSketchPad.alignSizeSelection( aInt);
	    else 
	      itsSketchPad.resizeSelection( 1, aInt);
	  }
	else if ( e.isMetaDown()) 
	  {
	    //align
	    String where;

	    if (aInt == Platform.LEFT_KEY) 
	      where = "Left";
	    else if (aInt == Platform.RIGHT_KEY) 
	      where = "Right";
	    else if (aInt == Platform.UP_KEY) 
	      where = "Top";
	    else 
	      where = "Bottom";

	    itsSketchPad.AlignSelectedObjects( where);
	  }
	else 
	  itsSketchPad.moveSelection( 1, aInt);
      }
    else if (e.isControlDown()) 
      {
	if (aInt == KeyEvent.VK_Z)
	  itsSketchPad.showErrorDescriptions();
	else if (aInt == 47)
	  {
	    //ask help for the reference Manual for the selected element...
	    // open one url *only*, because we open only one browser.
	    ErmesObject aObject;
	    String urlToOpen;
	    Interp interp  = MaxApplication.getTclInterp();
	  
	    if (ErmesSketchPad.currentSelection.itsObjects.size() > 0)
	      {
		aObject = (ErmesObject) ErmesSketchPad.currentSelection.itsObjects.elementAt( 0);
		urlToOpen = FtsReferenceURLTable.getReferenceURL( aObject.itsFtsObject);
	    
		if (urlToOpen != null)
		  {
		    try
		      {
			// Call the tcl browse function, with the URL as argument
			// By default, the tcl browse function do nothing.
			// if a user installed a browser package, this will show the documentation.
			interp.eval( "browse " + urlToOpen);
		      }
		    catch (tcl.lang.TclException et)
		      {
			System.out.println( "TCL error executing browse " + urlToOpen + " : " + interp.getResult());
		      }
		  }
	      }   
	  }
	else
	  super.keyPressed( e);
      }
    else if ( keyEventClient != null)
      {
	keyEventClient.keyPressed( e);
      }
    else if ( ( aInt == ircam.jmax.utils.Platform.DELETE_KEY)
	      || ( aInt==ircam.jmax.utils.Platform.BACKSPACE_KEY) )
      {
	if (! itsSketchPad.GetEditField().HasFocus())
	  itsSketchPad.DeleteSelected();
      }
    else
      {
	// Finally, if we don't redefine the key, call the superclass method that define the standard things.
	super.keyPressed( e);
      }
  }

  public void keyTyped(KeyEvent e)
  {
    if ( keyEventClient != null)
      keyEventClient.keyTyped( e);
  }

  public void keyReleased(KeyEvent e)
  {
    if ( keyEventClient != null)
      keyEventClient.keyReleased( e);
  }

  public static boolean isAnArrow( int code) 
  {
    return code == Platform.LEFT_KEY 
      || code == Platform.RIGHT_KEY
      || code == Platform.UP_KEY
      || code == Platform.DOWN_KEY;
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

	    if (aDialog.GetNothingToDoFlag())
	      return;

	    if (aDialog.GetToSaveFlag())
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

    itsObjectsMenu.remove(itsAlignMenu);
    itsObjectsMenu.remove(itsSizesMenu);
    itsObjectsMenu.remove(itsFontsMenu);
    getMenuBar().remove(itsObjectsMenu);

    itsScrollerView.getHAdjustable().removeAdjustmentListener( itsSketchPad);
    itsScrollerView.getVAdjustable().removeAdjustmentListener( itsSketchPad);

    itsSketchPad.cleanAll();

    itsSketchPad = null;
    itsPatcher = null;
    itsPatcherData = null;
    itsDocument = null;

    itsDocument = null;
    keyEventClient = null;
    ftsObjectsPasted = null;
    ftsConnectionsPasted = null;
    itsScrollerView = null;
    super.Destroy();

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
  }

  private void FontsMenuAction( MenuItem theMenuItem, String theString)
  {
    if (itsSelectedFontMenu!=null) 
      itsSelectedFontMenu.setState( false);
    
    if (ErmesSketchPad.currentSelection.itsObjects.size() == 0)
      itsSketchFontMenu=(CheckboxMenuItem) theMenuItem;
    
    itsSelectedFontMenu = (CheckboxMenuItem) theMenuItem;
    
    if (ErmesSketchPad.currentSelection.itsObjects.size()==0)
      itsSketchPad.ChangeFont( new Font( theString, Font.PLAIN, itsSketchPad.sketchFontSize));
    else
      itsSketchPad.ChangeNameFont( theString);
    
    itsSelectedFontMenu.setState( true);
  }

  public void DeselectionUpdateMenu()
  {
    if (itsSelectedFontMenu != null)
      itsSelectedFontMenu.setState( false);

    itsSelectedFontMenu = itsSketchFontMenu;
    itsSelectedFontMenu.setState( true);

    if ( itsSelectedSizeMenu != null)
      itsSelectedSizeMenu.setState( false);
    itsSelectedSizeMenu = itsSketchSizeMenu;
    itsSelectedSizeMenu.setState( true);
  }

  public void SelectionUpdateMenu( String theFont, Integer theSize)
  {
    CheckboxMenuItem aCheckItem = null;

    if (itsSelectedFontMenu != null)
      itsSelectedFontMenu.setState( false);

    if (theFont != null)
      {
	for( int i = 0; i < itsFontsMenu.getItemCount(); i++)
	  {
	    aCheckItem = (CheckboxMenuItem)itsFontsMenu.getItem( i);
	    if (aCheckItem.getLabel().toLowerCase().equals( theFont))
	      {
		itsSelectedFontMenu = aCheckItem;
		itsSelectedFontMenu.setState( true);
		break;
	      }
	  }
      }
    else
      itsSelectedFontMenu = null;
    
    if (itsSelectedSizeMenu!=null)
      itsSelectedSizeMenu.setState( false);

    if (theSize != null)
      {
	for ( int i = 0; i < itsSizesMenu.getItemCount(); i++)
	  {
	    aCheckItem = (CheckboxMenuItem)itsSizesMenu.getItem( i);
	    if (aCheckItem.getLabel().equals( theSize.toString()))
	      {
		itsSelectedSizeMenu = aCheckItem;
		itsSelectedSizeMenu.setState( true);
		break;
	      }
	  }
      }
    else
      itsSelectedSizeMenu = null;
  }


  private boolean SizesMenuAction( CheckboxMenuItem theMenuItem, int theFontSize) 
  {
    if (itsSelectedSizeMenu != null)
      itsSelectedSizeMenu.setState( false);

    itsSelectedSizeMenu = theMenuItem;
    itsSelectedSizeMenu.setState( true);

    //if we are here, a font size have been choosen from the FONT menu
    if (ErmesSketchPad.currentSelection.itsObjects.size() ==0 )
      itsSketchSizeMenu = (CheckboxMenuItem)theMenuItem;
    
    if  (ErmesSketchPad.currentSelection.itsObjects.size() == 0) 
      {
	itsSketchPad.sketchFontSize = theFontSize;
	itsSketchPad.ChangeFont( new Font( itsSketchPad.sketchFont.getName(), 
					   itsSketchPad.sketchFont.getStyle(), 
					   theFontSize));
      }
    else
      itsSketchPad.ChangeSizeFont( theFontSize);

    return true;
  }

  public void focusGained( FocusEvent e)
  {
    itsSketchPad.RequestOffScreen();

    Graphics g = getGraphics();

    if (g != null)
      {
	itsSketchPad.update( g);
	g.dispose();
      }
  } 

  public void focusLost( FocusEvent e)
  {
    itsSketchPad.duringScrolling = false;
  }

  public void windowActivated( WindowEvent e)
  {
    requestFocus();
  }

  private MenuItem getSelectAllMenuItem() 
  {
    return itsSelectAllMenuItem;
  }

  private MenuItem getLockMenuItem() 
  {
    return itsLockMenuItem;
  }

  protected void setLocked( boolean locked)
  {
    // Store the mode in a non persistent, property of 
    // the patch, so that subpatcher can use it as their initial mode

    if (locked)
      itsPatcherData.setEditMode(FtsPatcherData.RUN_MODE);
    else
      itsPatcherData.setEditMode(FtsPatcherData.EDIT_MODE);

    itsSketchPad.setLocked( locked);

    itsToolBar.setLocked( locked);

    getSelectAllMenuItem().setEnabled( !locked);
    getCutMenu().setEnabled( !locked);
    getCopyMenu().setEnabled( !locked);
    getDuplicateMenu().setEnabled( !locked);

    getLockMenuItem().setLabel(locked ? "Unlock" : "Lock");

    setKeyEventClient( null); //when changing mode, always remove key listeners

    requestFocus();
  }

  final boolean isLocked()
  {
    return itsSketchPad.isLocked();
  }

  public Dimension getMinimumSize() 
  {
    return getPreferredSize();
  }
  
  public Dimension getPreferredSize()
  {
    return preferredsize;
  }

  public void setKeyEventClient( KeyEventClient keyEventClient)
  {
    if ( this.keyEventClient != null && this.keyEventClient != keyEventClient)
      this.keyEventClient.keyInputLost();

    this.keyEventClient = keyEventClient;

    if (this.keyEventClient != null)
      this.keyEventClient.keyInputGained();
  }

  // More window listening primitives

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
}
