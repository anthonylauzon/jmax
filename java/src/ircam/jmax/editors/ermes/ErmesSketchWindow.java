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
import com.sun.java.swing.*;


//
// The window that contains the sketchpad. It knows the ftspatcher it is editing.
// It handles all the sketch menus, it knows how to load from a ftspatcher.
//
public class ErmesSketchWindow extends MaxEditor implements FtsPropertyHandler, ComponentListener {

  protected KeyEventClient keyEventClient;

  public void propertyChanged( FtsObject object, String name, Object value)
  {
    if (name == "ins")
      {
	// A patcher has been redefined
	
	itsSketchPad.RedefineInChoice();
      }
    else if (name == "outs")
      {
	// A patcher has been redefined
	
	itsSketchPad.RedefineOutChoice();
      }
    else if (name == "newObject") 
      {
	ftsObjectsPasted.addElement( value);
      }
    else if (name == "newConnection")
      {
	ftsConnectionsPasted.addElement( value);
      }
    else if (name == "deletedObject") 
      {
	itsSketchPad.DeleteGraphicObject(itsSketchPad.getErmesObjectFor((FtsObject)value), false);
      }
    else if (name == "deletedConnection") 
      {
	itsSketchPad.DeleteGraphicConnection(itsSketchPad.getErmesConnectionFor((FtsConnection)value), false);
      }

    if (!pasting)
      itsSketchPad.paintDirtyList();
  }

  public void componentResized( ComponentEvent e) 
  {
    if (itsPatcher != null) 
      {
	itsPatcher.put( "ww", getSize().width - horizontalOffset());
	itsPatcher.put( "wh", getSize().height - verticalOffset());
      }
  }

  public void componentMoved( ComponentEvent e) 
  {
    if (itsPatcher == null)
      {
	itsPatcher.put( "wx", getLocation().x);
	itsPatcher.put( "wy", getLocation().y);
      }
  }

  public void componentShown( ComponentEvent e) 
  {
  }

  public void componentHidden( ComponentEvent e)
  {
  }

  static FtsClipboard ftsClipboard;

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


  MaxVector ftsObjectsPasted = new MaxVector();
  MaxVector ftsConnectionsPasted = new MaxVector();
  boolean pasting = false;

  public static int preferredWidth = 490;
  public static int preferredHeight = 450;
  Dimension preferredsize = new Dimension( preferredWidth,preferredHeight);
  public ErmesSketchPad itsSketchPad;
  ScrollPane itsScrollerView;
  ErmesSwToolbar itsToolBar;
  static String[] itsFontList = Toolkit.getDefaultToolkit().getFontList();
  public FtsContainerObject itsPatcher;

  private Menu itsAlignObjectMenu;
  private Menu itsSizesMenu;	
  private Menu itsFontsMenu;
  private Menu itsExecutionMenu;

  CheckboxMenuItem itsSelectedSizeMenu;   //the Selected objects size MenuItem
  CheckboxMenuItem itsSketchSizeMenu;     //the SketchPad size MenuItem
  CheckboxMenuItem itsSketchFontMenu;     //the SketchPad font MenuItem
  CheckboxMenuItem itsSelectedFontMenu;   //the Selected objects font MenuItem

  MenuItem itsRunModeMenuItem;
  MenuItem itsSelectAllMenuItem;

  boolean itsChangingRunEditMode = false;
  public MaxDocument itsDocument;

  public void showObject( Object obj)
  {
    itsSketchPad.showObject( obj);
  }

  //
  // Constructor 
  //

  public ErmesSketchWindow( FtsContainerObject patcher) 
  {
    super( Mda.getDocumentTypeByName( "patcher"));

    // (fd)
    keyEventClient = null;

    itsDocument = patcher.getDocument();
    itsPatcher = patcher;

    // (mdc) deleteConnection watcher always active; 
    // connections can be deleted as side effect of 
    // editing on other windows; other watches should be istalled here also,

    itsPatcher.watch( "deletedConnection", this); 

    if (itsDocument.isRootData(itsPatcher))
      setTitle( itsDocument.getName());
    else
      setTitle(chooseWindowName( patcher));

    itsSketchPad = new ErmesSketchPad( this, itsPatcher);
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

    setVisible( true);

    itsPatcher.watch( "ins", this);
    itsPatcher.watch( "outs", this);
    addComponentListener( this); 
    
    // To set the initial state: set to edit mode only if the
    // initialMode property of a patcher is set and it is set
    // to something different than "run" (usually, "edit" :)

    FtsContainerObject p = itsPatcher;
    Object mode;

    mode = p.get("initialMode");

    p = p.getParent();

    while ((mode instanceof FtsVoid) && (p != null))
      {
	mode = p.get("editMode");
	p = p.getParent();
      }

    if ((mode instanceof FtsVoid) || mode.equals("run"))
      setRunMode(true);

    // Finally, activate the updates

    itsPatcher.startUpdates();
  }

  static String chooseWindowName( FtsContainerObject theFtsPatcher)
  {
    if (theFtsPatcher instanceof FtsPatcherObject)
      return "patcher " + theFtsPatcher.getObjectName();
    else
      return theFtsPatcher.getClassName();
  }

  int horizontalOffset()
  {
    return 40;
  }
  
  int verticalOffset() 
  {
    return 130;
  }
  
  private void InitFromContainer( FtsContainerObject patcher)
  {
    Object aObject;
    int x = 0;
    int y = 0;
    int width = 500;
    int height = 480;
    Object value;

    //Double check the existence of the window properties. If there aren't, use defaults
      
    value = patcher.get( "wx");

    if (value instanceof FtsVoid) 
      patcher.put( "wx", new Integer(x));
    else  
      x = ((Integer) value).intValue();

    value = patcher.get( "wy");

    if (value instanceof FtsVoid) 
      patcher.put( "wy", new Integer(y));
    else  
      y = ((Integer) value).intValue();


    value = patcher.get( "ww");

    if (value instanceof FtsVoid) 
      patcher.put( "ww", new Integer (width));
    else 
      width = ((Integer) value).intValue();


    value = patcher.get( "wh");

    if (value instanceof FtsVoid) 
      patcher.put( "wh", new Integer(height));
    else
      height = ((Integer) value).intValue();
      
    setBounds( x, y, width + horizontalOffset(), height + verticalOffset());
  }

  public void SetupMenu()
  {
    MenuBar menuBar = getMenuBar();

    Menu editMenu = GetEditMenu();

    editMenu.add( new MenuItem( "-"));

    itsSelectAllMenuItem = new MenuItem( "Select All  Ctrl+A");
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

    MenuItem inspectMenuItem = new MenuItem("Inspect Ctrl+I");
    editMenu.add(new MenuItem("-"));
    editMenu.add(inspectMenuItem);

    inspectMenuItem.addActionListener( new MaxActionListener(inspectMenuItem) {
      public void actionPerformed(ActionEvent e) 
	{
	  if (ErmesSketchPad.currentSelection.isEmpty())
	    ErmesPatcherInspector.inspect(itsPatcher);
	  else
	    ErmesSketchPad.inspectSelection();
	}
    });

    GetCutMenu().setEnabled( true);
    GetCopyMenu().setEnabled( true);
    GetPasteMenu().setEnabled( true);
    //GetClearMenu().setEnabled(false);
    
    // Add the Graphics menu
    Menu itsGraphicsMenu = new Menu( "Graphics");
    menuBar.add( itsGraphicsMenu);
    FillGraphicsMenu( itsGraphicsMenu);

    // Add the Text menu
    Menu itsTextMenu = new Menu( "Text");
    menuBar.add( itsTextMenu);
    FillTextMenu( itsTextMenu);

    CheckDefaultSizeFontMenuItem(); 
    CheckDefaultFontItem(); 

    // Add the Execution menu
    itsExecutionMenu = new Menu( "Execution");
    menuBar.add( itsExecutionMenu);
    FillExecutionMenu( itsExecutionMenu);
  }

  public void inspectAction()
  {
    if ( itsSketchPad.currentSelection.isEmpty() )
      ErmesPatcherInspector.inspect( itsPatcher);
    else
      itsSketchPad.inspectSelection();
  }

  protected void Cut()
  {
    Copy();
    itsSketchPad.DeleteSelected();
  }

  protected void Copy()
  {
    Cursor temp = getCursor();

    Point tempPoint = itsSketchPad.selectionUpperLeft();
    itsSketchPad.pasteDelta.setLocation( tempPoint.x - itsSketchPad.itsCurrentScrollingX, 
					 tempPoint.y - itsSketchPad.itsCurrentScrollingY);
    itsSketchPad.numberOfPaste = 0;

    setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));
    
    ftsClipboard.copy( Fts.getSelection());
    setCursor( temp);
  }

  protected void Paste()
  {
    if (itsSketchPad.itsRunMode)
      return;

    Cursor temp = getCursor();

    setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));

    ftsObjectsPasted.removeAllElements();
    ftsConnectionsPasted.removeAllElements();
    pasting = true;

    itsPatcher.watch( "newObject", this);
    itsPatcher.watch( "newConnection", this);

    ftsClipboard.paste( itsPatcher);

    itsPatcher.removeWatch( "newObject", this);    
    itsPatcher.removeWatch( "newConnection", this);    
    pasting = false;

    // make the sketch do the graphic job
    if (!ftsObjectsPasted.isEmpty() || ! ftsConnectionsPasted.isEmpty())
      {
	itsSketchPad.PasteObjects( ftsObjectsPasted, ftsConnectionsPasted);
	itsSketchPad.RequestOffScreen();
	itsSketchPad.repaint();
      }
    setCursor( temp);
  }

  private void FillGraphicsMenu( Menu graphicsMenu)
  {
    itsAlignObjectMenu =  new Menu( "Align Objects");
    graphicsMenu.add( itsAlignObjectMenu);
    FillAlignObjectsMenu( itsAlignObjectMenu);
  }

  class AlignMenuAdapter extends MaxActionListener
  {
    String align;

    AlignMenuAdapter( MenuItem item, String align)
    {
      super(item);
      this.align = align;
    }

    public  void actionPerformed( ActionEvent e)
    {
      itsSketchPad.AlignSelectedObjects( align);
    }
  }

  private void FillAlignObjectsMenu( Menu theAlignObjectMenu)
  {
    MenuItem aMenuItem;

    aMenuItem = new MenuItem( "Align Top");
    theAlignObjectMenu.add( aMenuItem);
    aMenuItem.addActionListener(new AlignMenuAdapter(aMenuItem,  "Top"));

    aMenuItem = new MenuItem( "Align Left");
    theAlignObjectMenu.add( aMenuItem);
    aMenuItem.addActionListener( new AlignMenuAdapter(aMenuItem,  "Left"));

    aMenuItem = new MenuItem( "Align Bottom");
    theAlignObjectMenu.add( aMenuItem);
    aMenuItem.addActionListener( new AlignMenuAdapter(aMenuItem,  "Bottom"));

    aMenuItem = new MenuItem( "Align Right");
    theAlignObjectMenu.add( aMenuItem);
    aMenuItem.addActionListener( new AlignMenuAdapter(aMenuItem,  "Right"));
  }

  private void FillExecutionMenu( Menu theExecutionMenu)
  {
    itsRunModeMenuItem = new MenuItem( "Run Mode Ctrl+E");
    theExecutionMenu.add( itsRunModeMenuItem);

    itsRunModeMenuItem.addActionListener( new MaxActionListener(itsRunModeMenuItem) {
      public  void actionPerformed( ActionEvent e)
	{
	  setRunMode( ! itsSketchPad.itsRunMode);
	}
    });
  }

  private void FillTextMenu( Menu theTextMenu)
  {
    String aString;
    CheckboxMenuItem aCheckItem;

    //-- fonts
    itsFontsMenu =  new Menu( "Fonts");
    FillFontMenu( itsFontsMenu);
    theTextMenu.add( itsFontsMenu);

    theTextMenu.add( new MenuItem( "-"));

    //-- sizes
    itsSizesMenu = new Menu( "Sizes");
    FillSizesMenu( itsSizesMenu);
    theTextMenu.add( itsSizesMenu);
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
	
  public void keyTyped( KeyEvent e)
  {
  }

  public void keyReleased( KeyEvent e)
  {
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
	if (aInt == 73)
	  {
	    // i

	    if (ErmesSketchPad.currentSelection.isEmpty())
	      ErmesPatcherInspector.inspect(itsPatcher);
	    else
	      ErmesSketchPad.inspectSelection();
	  }
	else if (aInt == 90)
	  itsSketchPad.showAnnotations( "errdesc"); // z ??
	else if (aInt == 65)
	  itsSketchPad.SelectAll();//a
	else if (aInt == 69)
	  {//e
	    if (itsSketchPad.GetRunMode()) 
	      setRunMode( false);
	    else 
	      setRunMode( true);
	  }
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
    else if (aInt == 47)
      {
	//ask help for the selected element...
	ErmesObject aObject = null;
      
	for (Enumeration en = ErmesSketchPad.currentSelection.itsObjects.elements(); en.hasMoreElements(); )
	  {
	    aObject = (ErmesObject) en.nextElement();
	
	    FtsHelpPatchTable.openHelpPatch( aObject.itsFtsObject);
	  }
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

  public static boolean isAnArrow( int code) 
  {
    return code == Platform.LEFT_KEY 
      || code == Platform.RIGHT_KEY
      || code == Platform.UP_KEY
      || code == Platform.DOWN_KEY;
  }

  public void Close()
  {
    if (! itsDocument.isRootData(itsPatcher))
      {
	itsPatcher.stopUpdates();
	Mda.dispose(itsPatcher); // experimental
	// setVisible( false);
      }
    else 
      {
	if ( ShouldSave()) 
	  {
	    FileNotSavedDialog aDialog = new FileNotSavedDialog( this, itsDocument);

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
    removeComponentListener( this);

    itsScrollerView.getHAdjustable().removeAdjustmentListener( itsSketchPad);
    itsScrollerView.getVAdjustable().removeAdjustmentListener( itsSketchPad);

    itsPatcher.removeWatch( this);
    itsSketchPad.cleanAll();

    itsSketchPad = null;
    itsPatcher = null;
    itsDocument = null;
    keyEventClient = null;
    ftsObjectsPasted = null;
    ftsConnectionsPasted = null;
    itsScrollerView = null;
    super.Destroy();
  }

  public boolean ShouldSave()
  {
    return (itsDocument.isRootData(itsPatcher) && (! itsDocument.isSaved()));
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

    if (! itsDocument.isRootData(itsPatcher))
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

    if (! itsDocument.isRootData(itsPatcher))
      {
	new ErrorDialog( this, "Only root patchers can be saved");
	return;
      }

    file = MaxFileChooser.chooseFileToSave( this, itsDocument.getDocumentFile());

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

    file = MaxFileChooser.chooseFileToSave( this, itsDocument.getDocumentFile());

    if (file == null)
      return;

    try
      {
	if (itsDocument.isRootData(itsPatcher))
	  {
	    // Make a document save to
	    itsDocument.saveTo( file);
	  }
	else
	  {
	    // Make a subdocument save to
	    itsDocument.saveSubDocumentTo( itsPatcher, file);
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
    itsSketchPad.itsScrolled = false;
  }

  public void windowActivated( WindowEvent e)
  {
    requestFocus();
  }


  private MenuItem getRunModeMenuItem() 
  {
    return itsRunModeMenuItem;
  }

  private MenuItem getSelectAllMenuItem() 
  {
    return itsSelectAllMenuItem;
  }

  public void setRunMode( boolean theRunMode)
  {
    ErmesObject aObject;

    // Store the mode in a non persistent, property of 
    // the patch, so that subpatcher can use it as their initial mode
    
    itsPatcher.put( "editMode", (theRunMode ? "run" : "edit"));

    itsChangingRunEditMode = true;

    MenuItem aRunEditItem = getRunModeMenuItem();
    MenuItem aSelectAllItem = getSelectAllMenuItem();

    if (theRunMode)  
      setBackground( Color.white);
    else
      setBackground( ErmesSketchPad.sketchColor);
    
    itsSketchPad.SetRunMode( theRunMode);
    
    itsToolBar.setRunMode( theRunMode);
    aSelectAllItem.setEnabled( !theRunMode);

    itsRunModeMenuItem.setLabel( theRunMode ? "Edit Mode Ctrl+E" : "Run Mode Ctrl+E");
    setKeyEventClient( null); //when changing mode, always remove key listeners
    requestFocus();
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
    if (itsPatcher != null)
      itsPatcher.stopUpdates();
  }       

  public void windowDeiconified( WindowEvent e)
  {
    // Do the test because the awt can call this before itsPatcher is ready
    if (itsPatcher != null)
      itsPatcher.startUpdates();
  }       
}
