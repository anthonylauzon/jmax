package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;
import java.util.*;
import java.io.*;
import java.text.*;//DateFormat...

import tcl.lang.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import ircam.jmax.dialogs.*;
import com.sun.java.swing.*;
//import com.sun.java.swing.jlf.*;

/**
 * The window that contains the sketchpad. It knows the document
 * it is showing, and the fospatcher to which it is associated.
 * It handles all the sketch menus, it knows how to load from a fospatcher.
 */

public class ErmesSketchWindow extends MaxEditor implements MaxDataEditor, FtsPropertyHandler {

  public void propertyChanged(FtsObject object, String name, Object value) {
    //    System.err.println("cambiata la proprieta' "+name);
    if (name.equals("newObject"))
      ftsObjectsPasted.addElement(value);
    else if (name.equals("newConnection"))
      ftsConnectionsPasted.addElement(value);
    else if (name.equals("deletedObject")) {
      // just an hack: remove the watch temporarly, add it just after
      // to avoid recursion
      itsPatcher.removeWatch(this, "deletedObject");
      itsSketchPad.itsHelper.DeleteGraphicObject((ErmesObject)(((FtsObject)value).getRepresentation()));
      itsPatcher.watch("deletedObject", this);
    }
    else if (name.equals("deleteConnection")) {
      itsPatcher.removeWatch(this,"deleteConnection" );
      ErmesObject objFrom = (ErmesObject) (((FtsConnection)value).getFrom()).getRepresentation();
      int outletFrom = ((FtsConnection)value).getFromOutlet();
      ErmesObject objTo = (ErmesObject) (((FtsConnection)value).getTo()).getRepresentation();
      int inletTo = ((FtsConnection)value).getToInlet();
      itsSketchPad.itsHelper.DeleteConnectionByInOut(objFrom, outletFrom, objTo, inletTo);
      itsPatcher.watch("deleteConnection", this);
    }
  }

  
  FtsSelection itsSelection;
  Vector ftsObjectsPasted = new Vector();
  Vector ftsConnectionsPasted = new Vector();
  public static ErmesClipboardProvider itsClipboardProvider = new ErmesClipboardProvider();
  public boolean isSubPatcher = false;
  //  public ErmesObject itsOwner;//in case this is a subpatcher
  public boolean isAbstraction = false;
  final String FILEDIALOGMENUITEM = "File dialog...";
  public static int preferredWidth = 490;
  public static int preferredHeight = 450;
  Dimension preferredsize = new Dimension(preferredWidth,preferredHeight);
  public ErmesSketchPad itsSketchPad = new ErmesSketchPad(this);
  public ErmesPatcherDoc itsDocument;
  ErmesScrollerView itsScrollerView = new ErmesScrollerView(this, itsSketchPad);
  ErmesSwToolbar itsToolBar = new ErmesSwToolbar(itsSketchPad);
  ErmesSketchWindow itsTopWindow = null;
  static String[] itsFontList = Toolkit.getDefaultToolkit().getFontList();
  ErmesSwVarEdit itsVarEdit;//created when we need a variable editor (abstractions)!
  public FtsContainerObject itsPatcher;
  private Menu itsJustificationMenu;
  private Menu itsResizeObjectMenu;
  private Menu itsAlignObjectMenu;
  private Menu itsTextMenu;	
  private Menu itsSizesMenu;	
  private Menu itsFontsMenu;
  private Menu itsSubWindowsMenu;
  private Menu itsExecutionMenu;
  private Menu itsGraphicsMenu;
  CheckboxMenuItem itsSelectedSizeMenu;//the Selected objects size MenuItem
  CheckboxMenuItem itsSketchSizeMenu;//the SketchPad size MenuItem
  CheckboxMenuItem itsSketchFontMenu;//the SketchPad font MenuItem
  CheckboxMenuItem itsSelectedFontMenu;//the Selected objects font MenuItem
  CheckboxMenuItem itsSelectedJustificationMenu;
  CheckboxMenuItem itsSketchJustificationMenu;
  CheckboxMenuItem itsCurrentResizeMenu;
  CheckboxMenuItem itsAutoroutingMenu;
  MenuItem itsRunModeMenuItem;
  MenuItem itsSelectAllMenuItem;
  boolean itsClosing = false;
  boolean itsChangingRunEditMode = false;
  public Vector itsSubWindowList;
  Vector itsWindowMenuList;
  private boolean alreadySaved =true;
  boolean neverSaved =true;
  //public String itsTitle;
  public MaxData itsData;
  static int untitledCounter = 1;

  // the MaxDataEditor interface

  public void quitEdit() {}
  /** Tell the editor to syncronize, i.e. to store in the
   * data all the information possibly cached in the editor
   * and still not passed to the data instance; this usually
   * happen before saving an instance.
   */
  public void syncData(){}
  /** Tell the editor the data has changed; it pass a sigle Java
   * Object that may code what is changed and where; if the argument
   * is null, means usually that all the data is changed
   */
  public void dataChanged(Object reason){}

  //end of the MaxDataEditor interface


  //----------alternative contructors:
  /**
   * constructor from a MaxData-only (to be used for top-level patchers)
   */
  public ErmesSketchWindow(MaxData theData) {
    super();
    if (theData.getName()==null) setTitle(GetNewUntitledName());
    else {
      setTitle(theData.getDataSource().toString()); 
    }
    itsData = theData;
    itsPatcher = (FtsContainerObject)(theData.getContent());
    CommonInitializations();
    isSubPatcher = false;
    alreadySaved = true;
  }
  /**
   * constructor from a MaxData AND a ftsContainer AND a father window (subpatchers editors)
   * added better window titles for patchers.
   */


  static String chooseWindowName(FtsContainerObject theFtsPatcher)
  {
    if (theFtsPatcher instanceof FtsPatcherObject)
      return "patcher " + theFtsPatcher.getObjectName();
    else
      return theFtsPatcher.getClassName();
  }

  public ErmesSketchWindow (MaxData theData, FtsContainerObject theFtsPatcher, ErmesSketchWindow theTopWindow) {
    //super(theData.getName());
    super(MaxApplication.GetWholeWinName(chooseWindowName(theFtsPatcher)));
    itsPatcher = theFtsPatcher;
    itsData = theData;
    CommonInitializations();
    isSubPatcher = true;
    itsTopWindow = theTopWindow;
    alreadySaved = true;
  }
   
  /**
   *utility function, used to avoid to replicate code in the two contructors
   */
  void CommonInitializations() {
    
    itsSketchPad.setFont(new Font(ircam.jmax.utils.Platform.FONT_NAME, Font.PLAIN, ircam.jmax.utils.Platform.FONT_SIZE));						// communicate with
    Init(); //MaxEditor base class init (standard menu handling)
    //isSubPatcher = false;
    isAbstraction = false;
    itsSketchPad.SetToolBar(itsToolBar);	// inform the Sketch of the ToolBar to 
    itsSubWindowList = new Vector();
    itsWindowMenuList = new Vector();

    InitSketchWin();
    validate();
    itsPatcher.open();
    MaxApplication.itsWindow = this;
    InitFromContainer(itsPatcher);
    setVisible(true);
  }

  public static String GetNewUntitledName() {
    return "untitled"+(untitledCounter++);
  }

  public void ToSave(){
    alreadySaved = false;
  }

    //--------------------------------------------------------
    //	CONSTRUCTOR
    //
    //--------------------------------------------------------    
  public ErmesSketchWindow(boolean theIsSubPatcher, ErmesSketchWindow theTopWindow, boolean theIsAbstraction)
  {
    super();
    isSubPatcher = theIsSubPatcher;
    isAbstraction = theIsAbstraction;
    itsTopWindow = theTopWindow;
    itsSketchPad.SetToolBar(itsToolBar);	// inform the Sketch of the ToolBar to 
    itsSubWindowList = new Vector();
    itsWindowMenuList = new Vector();
    itsSketchPad.setFont(new Font(ircam.jmax.utils.Platform.FONT_NAME, Font.PLAIN, ircam.jmax.utils.Platform.FONT_SIZE));						// communicate with
      
    itsDocument = new ErmesPatcherDoc(this);
    InitSketchWin();
    validate();
    alreadySaved = true;
  }

  int horizontalOffset() {
    //sketchPad is not there yet, and we have no time for the release.
    //    return 20+itsSketchPad.getLocation().x; //the size of the sketch
    return 40;
  }
  
  int verticalOffset() {
    //sketchPad is not there yet, and there's no time for the release.
    //    return itsSketchPad.getLocation().y;//the size of the toolbar + menus
    return 130;
  }
  
  public void InitFromContainer(FtsContainerObject patcher) {
    
    Object aObject;
    int x=0;
    int y=0;
    int width=500;
    int height=480;
    Integer x1, y1, width1, height1;
    String autorouting;
    //double check the existence of the window properties. If there aren't, use defaults
      
      x1 = (Integer) patcher.get("wx");
      if (x1 == null) patcher.put("wx", new Integer(x));
      else  x = x1.intValue();
      y1 = (Integer) patcher.get("wy");
      if (y1 == null) patcher.put("wy", new Integer(y));
      else  y = y1.intValue();
      width1  = (Integer) patcher.get("ww");
      if (width1 == null) patcher.put("ww", new Integer (width));
      else  width = width1.intValue();
      height1 = (Integer) patcher.get("wh");
      if (height1 == null) patcher.put("wh", new Integer(height));
      else  height = height1.intValue();
      
      autorouting = (String) patcher.get("autorouting");
      if (autorouting == null) patcher.put("autorouting", "on");
      //get the window dimension use it for: reshape to the right dimensions

      setBounds(x, y, width+horizontalOffset(), height+verticalOffset());

      //assigning the autorouting mode.

      if (((String)(patcher.get("autorouting"))).equals("on"))
	SetAutorouting(true);
      else SetAutorouting(false);
      //      if((!isSubPatcher)&&(! MaxApplication.doAutorouting)) SetAutorouting(false);//???
      validate();
      itsSketchPad.InitFromFtsContainer(patcher);
      itsSketchPad.repaint();//force a repaint to build an offGraphics context
      validate();
    }

  //--------------------------------------------------------
  // InitSketchWin
  //--------------------------------------------------------
  protected void InitSketchWin(){ 
    
    getContentPane().setLayout(new ErmesToolBarLayout(ErmesToolBarLayout.VERTICAL));
    
    itsSelection = itsPatcher.getSelection();
    itsSelection.clean();
    setSize(new Dimension(600, 300));
    itsToolBar.setSize(600, 30);    
    getContentPane().add(itsToolBar);
    if (isAbstraction ) {
      ErmesSwVarEdit itsVarEdit = new ErmesSwVarEdit(itsSketchPad, 1);///***era 3
      getContentPane().add(itsVarEdit, "stick_both");
    }
    getContentPane().add(itsScrollerView, "fill");
    validate();
  }
  
  public void SetupMenu(){
    
    itsGraphicsMenu = new Menu("Graphics");
    getMenuBar().add(itsGraphicsMenu);
    FillGraphicsMenu(itsGraphicsMenu);
    
    itsTextMenu = new Menu("Text");
    getMenuBar().add(itsTextMenu);
    FillTextMenu(itsTextMenu);
    CheckDefaultSizeFontMenuItem();
    CheckDefaultFontItem();
    
    itsSubWindowsMenu = new Menu("SubWindows");

    itsSelectAllMenuItem = new MenuItem("Select All  Ctrl+A");
    GetEditMenu().add(itsSelectAllMenuItem);
    itsSelectAllMenuItem.addActionListener(new ActionListener() {
    public  void actionPerformed(ActionEvent e)
      { GetSketchPad().SelectAll();}});

    GetEditMenu().add(new MenuItem("-"));

    itsResizeObjectMenu =  new Menu("Resize Object");
    GetEditMenu().add(itsResizeObjectMenu);
    FillResizeObjectMenu(itsResizeObjectMenu);

    GetEditMenu().add(new MenuItem("-"));

    itsAlignObjectMenu =  new Menu("Align Objects");
    GetEditMenu().add(itsAlignObjectMenu);
    FillAlignObjectsMenu(itsAlignObjectMenu);
    
    itsExecutionMenu = new Menu("Execution");
    getMenuBar().add(itsExecutionMenu);
    FillExecutionMenu(itsExecutionMenu);

    GetCutMenu().setEnabled(true);
    GetCopyMenu().setEnabled(true);
    GetPasteMenu().setEnabled(true);
    GetClearMenu().setEnabled(false);
  }

  protected void Cut(){
    Copy();
    itsSketchPad.itsHelper.DeleteSelected();
  }

  // clipboard handling
  protected void Copy() {
    CreateFtsGraphics(this);
    itsSelection.clean();
    //fill the Fts selection

    for (Enumeration e = itsSketchPad.itsSelectedList.elements(); e.hasMoreElements();) {
      ErmesObject aObj = (ErmesObject)e.nextElement();
      itsSelection.addObject(aObj.itsFtsObject);
    }

    for (Enumeration e = itsSketchPad.itsSelectedConnections.elements(); e.hasMoreElements();) {
      ErmesConnection aConnection = (ErmesConnection)e.nextElement();
      itsSelection.addConnection(aConnection.itsFtsConnection);
    }

    itsClipboardProvider.addSelection(itsSelection);

    MaxApplication.systemClipboard.setContents(itsClipboardProvider, itsClipboardProvider);
  }



  protected void Paste() {
    String tclScriptToExecute = null;
    
    // take the objects list from the clipboard, if any. Only tclGroups for now
    Transferable aTransferable = MaxApplication.systemClipboard.getContents(this);
    if ((aTransferable == null) || !aTransferable.isDataFlavorSupported(ErmesClipboardProvider.tclGroupFlavor))
      return;

    try {
      tclScriptToExecute = (String) aTransferable.getTransferData(ErmesClipboardProvider.tclGroupFlavor);
    } catch (Exception e) {
      System.err.println("Error in clipboard handling (paste)" + e.toString());
    }

    ftsObjectsPasted.removeAllElements();
    ftsConnectionsPasted.removeAllElements();
    //evaluate the script
    itsPatcher.watch("newObject", this);
    itsPatcher.watch("newConnection", this);

    try {
      itsPatcher.eval(MaxApplication.getTclInterp(), tclScriptToExecute);
    } catch (TclException e) {
      System.out.println("bad format, cannot paste:" + e.toString());
    }
    
    //ftsObjectPasted vector contains the needed new, pasted objects
    //ftsConnectionPasted vector contains the needed new, pasted objects

    itsPatcher.removeWatch(this);    

    // make the sketch do the graphic job
    itsSketchPad.PasteObjects(ftsObjectsPasted, ftsConnectionsPasted);
    ErmesSketchPad.RequestOffScreen(itsSketchPad);
    itsSketchPad.repaint();
  }

  
  private void FillGraphicsMenu(Menu theGraphicsMenu){
    itsAutoroutingMenu = new CheckboxMenuItem("Autorouting", true);
    theGraphicsMenu.add(itsAutoroutingMenu);
    itsAutoroutingMenu.addItemListener(new ItemListener() {
      public  void itemStateChanged(ItemEvent e)
	{ SetAutorouting(itsAutoroutingMenu.getState());}});
  }


  class ResizeMenuAdapter implements ItemListener
  {
    CheckboxMenuItem item;
    String resize;

    ResizeMenuAdapter(CheckboxMenuItem item,  String resize)
    {
      this.item = item;
      this.resize = resize;
    }

    public  void itemStateChanged(ItemEvent e)
    {
      ResizeObjectMenuAction(item, resize);
    }
  }

  private void FillResizeObjectMenu(Menu theResizeObjectMenu)
  {
    CheckboxMenuItem aCheckItem;

    aCheckItem = new CheckboxMenuItem("Both");
    theResizeObjectMenu.add(aCheckItem);
    aCheckItem.setState(true);
    aCheckItem.addItemListener(new ResizeMenuAdapter(aCheckItem, "Both"));
    itsCurrentResizeMenu = aCheckItem;

    aCheckItem = new CheckboxMenuItem("Horizontal");
    theResizeObjectMenu.add(aCheckItem);
    aCheckItem.addItemListener(new ResizeMenuAdapter(aCheckItem, "Both"));

    aCheckItem = new CheckboxMenuItem("Vertical");
    theResizeObjectMenu.add(aCheckItem);
    aCheckItem.addItemListener(new ResizeMenuAdapter(aCheckItem, "Both"));
  }
  
  class AlignMenuAdapter implements ActionListener
  {
    String align;

    AlignMenuAdapter(String align)
    {
      this.align = align;
    }

    public  void actionPerformed(ActionEvent e)
    {
      itsSketchPad.AlignSelectedObjects(align);
    }
  }

  private void FillAlignObjectsMenu(Menu theAlignObjectMenu)
  {
    MenuItem aMenuItem;

    aMenuItem = new MenuItem("Align Top");
    theAlignObjectMenu.add(aMenuItem);
    aMenuItem.addActionListener(new AlignMenuAdapter("Top"));

    aMenuItem = new MenuItem("Align Left");
    theAlignObjectMenu.add(aMenuItem);
    aMenuItem.addActionListener(new AlignMenuAdapter("Left"));

    aMenuItem = new MenuItem("Align Bottom");
    theAlignObjectMenu.add(aMenuItem);
    aMenuItem.addActionListener(new AlignMenuAdapter("Bottom"));

    aMenuItem = new MenuItem("Align Right");
    theAlignObjectMenu.add(aMenuItem);
    aMenuItem.addActionListener(new AlignMenuAdapter("Right"));
  }

  private void FillExecutionMenu(Menu theExecutionMenu){
    itsRunModeMenuItem = new MenuItem("Run Mode Ctrl+E");
    theExecutionMenu.add(itsRunModeMenuItem);

    itsRunModeMenuItem.addActionListener(new ActionListener() {
      public  void actionPerformed(ActionEvent e)
	{
	  setRunMode(! itsSketchPad.itsRunMode);
	}
    });
  }

  private void FillTextMenu(Menu theTextMenu) {
    String aString;
    CheckboxMenuItem aCheckItem;

    //-- fonts
    itsFontsMenu =  new Menu("Fonts");
    FillFontMenu(itsFontsMenu);
    theTextMenu.add(itsFontsMenu);

    theTextMenu.add(new MenuItem("-"));

    //-- sizes
    itsSizesMenu = new Menu("Sizes");
    FillSizesMenu(itsSizesMenu);
    theTextMenu.add(itsSizesMenu);

    theTextMenu.add(new MenuItem("-"));

    //-- justification
    itsJustificationMenu = new Menu("Justification");
    FillJustificationMenu(itsJustificationMenu);
    theTextMenu.add(itsJustificationMenu);
  }

  class SizesMenuAdapter implements ItemListener
  {
    CheckboxMenuItem item;
    int size;

    SizesMenuAdapter(CheckboxMenuItem item, int size)
    {
      this.item = item;
      this.size = size;
    }

    public  void itemStateChanged(ItemEvent e)
    {
      SizesMenuAction(item, size);
    }
  }

  private void FillSizesMenu(Menu theSizesMenu)
  {
    CheckboxMenuItem aCheckItem;

    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("8"));
    aCheckItem.addItemListener(new SizesMenuAdapter(aCheckItem, 8));
    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("9"));
    aCheckItem.addItemListener(new SizesMenuAdapter(aCheckItem, 9));
    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("10"));
    aCheckItem.addItemListener(new SizesMenuAdapter(aCheckItem, 10));
    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("12"));
    aCheckItem.addItemListener(new SizesMenuAdapter(aCheckItem, 12));
    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("14"));
    aCheckItem.addItemListener(new SizesMenuAdapter(aCheckItem, 14));
    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("18"));
    aCheckItem.addItemListener(new SizesMenuAdapter(aCheckItem, 18));
    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("24"));
    aCheckItem.addItemListener(new SizesMenuAdapter(aCheckItem, 24));
    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("36"));
    aCheckItem.addItemListener(new SizesMenuAdapter(aCheckItem, 36));
    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("48"));
    aCheckItem.addItemListener(new SizesMenuAdapter(aCheckItem, 48));
  }

  class FontMenuAdapter implements ItemListener
  {
    CheckboxMenuItem item;
    String font;

    FontMenuAdapter(CheckboxMenuItem item, String font)
    {
      this.item = item;
      this.font = font;
    }

    public  void itemStateChanged(ItemEvent e)
    {
      FontsMenuAction(item, font);
    }
  }

  private void FillFontMenu(Menu theFontMenu)
  {
    CheckboxMenuItem aCheckItem;
    String aString;

    for(int i = 0;i<itsFontList.length;i++){
      aString = (String) itsFontList[i];
      theFontMenu.add(aCheckItem = new CheckboxMenuItem(aString));
      aCheckItem.addItemListener(new FontMenuAdapter(aCheckItem, aString));
    }
  }


  class JustificationMenuAdapter implements ItemListener
  {
    CheckboxMenuItem item;
    String justification;

    JustificationMenuAdapter(CheckboxMenuItem item,  String justification)
    {
      this.item = item;
      this.justification = justification;
    }

    public  void itemStateChanged(ItemEvent e)
    {
      JustificationMenuAction(item, justification);
    }
  }

  private void FillJustificationMenu(Menu theJustificationMenu)
  {
    CheckboxMenuItem aCheckItem;

    aCheckItem = new CheckboxMenuItem("Left");
    theJustificationMenu.add(aCheckItem);
    aCheckItem.addItemListener(new JustificationMenuAdapter(aCheckItem, "Left"));

    aCheckItem = new CheckboxMenuItem("Center");
    theJustificationMenu.add(aCheckItem);
    aCheckItem.addItemListener(new JustificationMenuAdapter(aCheckItem, "Center"));
    aCheckItem.setState(true);
    itsSelectedJustificationMenu = aCheckItem;
    itsSketchJustificationMenu = itsSelectedJustificationMenu;

    aCheckItem = new CheckboxMenuItem("Right");
    theJustificationMenu.add(aCheckItem);
    aCheckItem.addItemListener(new JustificationMenuAdapter(aCheckItem, "Right"));
  }

  private void CheckDefaultSizeFontMenuItem(){
    CheckboxMenuItem aCheckboxMenuItem;
    String aFontSize = String.valueOf(itsSketchPad.getSketchFontSize());
    for(int i=0; i<9;i++){
      aCheckboxMenuItem = (CheckboxMenuItem)itsSizesMenu.getItem(i);
      if(aCheckboxMenuItem.getLabel().compareTo(aFontSize) == 0){
	itsSketchSizeMenu = aCheckboxMenuItem;
	itsSelectedSizeMenu = itsSketchSizeMenu;
	itsSelectedSizeMenu.setState(true);
	return;
      }
    }
  }
	
  private void CheckDefaultFontItem(){
    CheckboxMenuItem aCheckboxMenuItem;
    String aFont = itsSketchPad.getFont().getName();
    for(int i=0; i<itsFontList.length; i++){
      aCheckboxMenuItem = (CheckboxMenuItem)itsFontsMenu.getItem(i);
      if(aCheckboxMenuItem.getLabel().toLowerCase().compareTo(aFont.toLowerCase()) == 0){
	itsSketchFontMenu = aCheckboxMenuItem;
	itsSelectedFontMenu = aCheckboxMenuItem;
	itsSketchFontMenu.setState(true);
	return;
      }
    }
  }
	
  public void AddToSubWindowList(ErmesSketchWindow theSketchWindow){
    MenuItem aMenuItem;
    if(isSubPatcher) itsTopWindow.AddToSubWindowList(theSketchWindow);
    else{
      boolean aFirstItem = false;
      if(itsSubWindowList.size()==0){
	itsSubWindowsMenu.add(aMenuItem = new MenuItem(theSketchWindow.getTitle()));
	aMenuItem.addActionListener(this);
	itsWindowsMenu.insert(itsSubWindowsMenu, 5);
	aFirstItem = true;
      }
      else{
	itsSubWindowsMenu.add(aMenuItem = new MenuItem(theSketchWindow.getTitle()));
	aMenuItem.addActionListener(this);
      }
      MaxApplication.AddToSubWindowsList(this, theSketchWindow, aFirstItem);
      itsSubWindowList.addElement(theSketchWindow);
    }
  }


  public void RemoveFromSubWindowList(ErmesSketchWindow theSubWindow){
    if(isSubPatcher)itsTopWindow.RemoveFromSubWindowList(theSubWindow);
    else{
      boolean aLastItem = true;
      MenuItem aItem;
      itsSubWindowList.removeElement(theSubWindow);
      if(itsSubWindowList.size()==0){
	itsSubWindowsMenu.removeAll();
	itsWindowsMenu.remove(itsSubWindowsMenu);
      }
      else{
	for(int i=0; i<itsSubWindowsMenu.getItemCount();i++){
	  aItem = itsSubWindowsMenu.getItem(i);
	  if(aItem.getLabel().equals(theSubWindow.getTitle())){
	    itsSubWindowsMenu.remove(aItem);
	    break;
	  }
	}
	aLastItem = false;
      }
      MaxApplication.RemoveFromSubWindowsList(this, theSubWindow, aLastItem);
    }
  }

  //--------------------------------------------------------
  //	GetFrame
  //--------------------------------------------------------
  public Frame GetFrame(){
    return this;
  }
  //--------------------------------------------------------
  //	GetSketchPad
  //	returns the associated ErmesSketchPad
  //--------------------------------------------------------
  public ErmesSketchPad GetSketchPad(){
    return itsSketchPad;
  }
  
  /////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////keyListener --inizio  
  public void keyTyped(KeyEvent e){}

  public void keyReleased(KeyEvent e){}

  // Modified to use inheritance and call the MaxEditor method
  // for all the standard key bindings
  public void keyPressed(KeyEvent e){
    int aInt = e.getKeyCode();
    if(e.isControlDown()){
      if(aInt == 65) itsSketchPad.SelectAll();//a
      else if(aInt == 69){//e
	if (itsSketchPad.GetRunMode()) setRunMode(false);
	else setRunMode(true);
	return;
      }
      else if (aInt == 47){//?
	//ask help for the reference Manual for the selected element...
	// open one url *only*, because we open only one browser.
      
	ErmesObject aObject;
	String urlToOpen;
	Interp interp  = MaxApplication.getTclInterp();
	
	if (itsSketchPad.itsSelectedList.size() > 0){
	  aObject = (ErmesObject) itsSketchPad.itsSelectedList.elementAt(0);

	  urlToOpen = FtsReferenceURLTable.getReferenceURL(aObject.itsFtsObject);
	  
	  if (urlToOpen != null){
	    try
	      {
		// Call the tcl browse function, with the URL as argument
		// By default, the tcl browse function do nothing.
		// if a user installed a browser package, this will
		// show the documentation.



		interp.eval("browse " + urlToOpen);
	      }
	    catch (tcl.lang.TclException et)
	      {
		System.out.println("TCL error executing browse " + urlToOpen + " : " + interp.getResult());
	      }
	  }
	}   
      }
      else super.keyPressed(e);
    } 
    else if((aInt==ircam.jmax.utils.Platform.DELETE_KEY)||(aInt==ircam.jmax.utils.Platform.BACKSPACE_KEY)){
      if(itsSketchPad.GetEditField()!=null){
	if(!itsSketchPad.GetEditField().HasFocus())
	  itsSketchPad.itsHelper.DeleteSelected();
      }
    }
    else if(aInt == 47){//this is a patch to trap the '?'
      //ask help for the selected element...
      ErmesObject aObject = null;
      File fileToOpen;
      for (Enumeration en = itsSketchPad.itsSelectedList.elements(); en.hasMoreElements();) {
	aObject = (ErmesObject) en.nextElement();
	
	fileToOpen = FtsHelpPatchTable.getHelpPatch(aObject.itsFtsObject);
	
	if (fileToOpen != null)
	  MaxApplication.OpenFile(MaxDataSource.makeDataSource(fileToOpen));
      }
    } else {
      // Finally, if we don't redefine the key, call the superclass method
      // that define the standard things.
      super.keyPressed(e);
    }
  }
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// keyListener --fine
  public void Close(){
    if (isSubPatcher){
      itsTopWindow.RemoveFromSubWindowList(this);
      setVisible(false);
    }
    else {
      Close(true);
    }
  }

  public void Close(boolean deleteOnFts){

    itsClosing = true;
    /*if (deleteOnFts)*/ itsPatcher.close();
    if (ShouldSave()) {
      FileNotSavedDialog aDialog = new FileNotSavedDialog(this, itsData);
      aDialog.setLocation(300, 300);
      aDialog.setVisible(true);
      if(aDialog.GetNothingToDoFlag()) return;
      if(aDialog.GetToSaveFlag()){
	Save();
      }
      aDialog.dispose();
    }
    //Bug#22
    //ErmesObject aObject;
    //FtsObject aFtsObject;
    //for (Enumeration e = itsSketchPad.itsElements.elements(); e.hasMoreElements();){
    //aObject = (ErmesObject)e.nextElement();
      //if(aObject.NeedPropertyHandler()){
      //aFtsObject = aObject.GetFtsObject();
      //if(aFtsObject!=null) aFtsObject.removeWatch(aObject);
      //}
    //}
    CloseAllSubWindows();//?????
    MaxApplication.RemoveThisWindowFromMenus(this);
    MaxApplication.itsSketchWindowList.removeElement(this);
    if (deleteOnFts) itsPatcher.delete();
    itsClosing = false;
    setVisible(false);
    if (deleteOnFts) dispose();
    return;
  }

  public boolean ShouldSave() {
    return (!(alreadySaved || isSubPatcher));
  }

  private boolean SaveBody(){
    setVisible(false);
    setTitle(itsData.getDataSource().toString()); 
    setVisible(true);
    CreateFtsGraphics(this);

    try
      {
	itsData.setInfo("Saved " + DateFormat.getDateInstance(DateFormat.FULL).format(new Date()));
	itsData.save();
      }
    catch (MaxDataException e)
      {
	ErrorDialog aErr = new ErrorDialog(this, e.getMessage());
	aErr.setLocation(100, 100);
	aErr.show();  
	return false;
      }

    alreadySaved = true;
    neverSaved = false;
    return true;
  }

  public void Save() {
    // first, tentative implementation:
    // the FILE is constructed now, and the ErmesSketchPad SaveTo method is invoked.
    // we should RECEIVE this FILE, or contruct it when we load this document
		
    // The "canSave" method of a data tell if it can be saved
    // i.e. if it have a data source, and if we can write to its data source

    if (itsData.canSave())
      SaveBody();
    else
      SaveAs();
  }

  public void SaveAs() {
    String oldTitle = getTitle();
    MaxDataSource source;

    source  = MaxFileChooser.chooseFileToSave(this, "Save As", itsData.getDataSource());

    if (source == null)
      return;
    else
      itsData.setDataSource(source);
    
    if(SaveBody())
      MaxApplication.ChangeWinNameMenus(oldTitle, getTitle());
  }


  private void  CloseAllSubWindows(){
    ErmesObject aObject;
    ErmesObjExternal aExternal;
    ErmesObjPatcher aPatcher;
    for (Enumeration e = itsSketchPad.itsElements.elements(); e.hasMoreElements();){
      aObject = (ErmesObject)e.nextElement();
      if(aObject instanceof ErmesObjExternal){
	aExternal = (ErmesObjExternal)aObject;
	if(aExternal.iAmPatcher){
	  if(aExternal.itsSubWindow!=null){
	    aExternal.itsSubWindow.CloseAllSubWindows();
	    aExternal.itsSubWindow.setVisible(false);
	    //bug11 aExternal.itsSubWindow.dispose();
	    //bug11 aExternal.itsSubWindow = null;
	  }
	}
      }
      else if(aObject instanceof ErmesObjPatcher){
	aPatcher = (ErmesObjPatcher)aObject;
	if(aPatcher.itsSubWindow!=null){
	  aPatcher.itsSubWindow.CloseAllSubWindows();
	  aPatcher.itsSubWindow.setVisible(false);
	  //bug11 aPatcher.itsSubWindow.dispose();
	  //bug11 aPatcher.itsSubWindow = null;
	}
      }
    }
  }

  public void Print(){
    PrintJob aPrintjob = getToolkit().getPrintJob(this, "Printing Patcher", MaxApplication.jmaxProperties);
    if(aPrintjob != null){
      Graphics aPrintGraphics = aPrintjob.getGraphics();
      if(aPrintGraphics != null){
	//aPrintGraphics.setClip(0, 0, 400, 400);
	itsSketchPad.printAll(aPrintGraphics);
	aPrintGraphics.dispose();
      }
      aPrintjob.end();
    }
  }

  private void FontsMenuAction(MenuItem theMenuItem, String theString) {
    
    if(itsSelectedFontMenu!=null) itsSelectedFontMenu.setState(false);
    
    if(itsSketchPad.GetSelectedList().size()==0) itsSketchFontMenu=(CheckboxMenuItem) theMenuItem;
    
    itsSelectedFontMenu = (CheckboxMenuItem) theMenuItem;
    
    if(itsSketchPad.GetSelectedList().size()==0)
      itsSketchPad.ChangeFont(new Font(theString, Font.PLAIN, itsSketchPad.sketchFontSize));
    else itsSketchPad.ChangeNameFont(theString);
    
    itsSelectedFontMenu.setState(true);
  }

  public void DeselectionUpdateMenu(){
    if(itsSelectedFontMenu!=null) itsSelectedFontMenu.setState(false);
    itsSelectedFontMenu = itsSketchFontMenu;
    itsSelectedFontMenu.setState(true);
    if(itsSelectedSizeMenu!=null) itsSelectedSizeMenu.setState(false);
    itsSelectedSizeMenu = itsSketchSizeMenu;
    itsSelectedSizeMenu.setState(true);
    if(itsSelectedJustificationMenu!=null) itsSelectedJustificationMenu.setState(false);
    itsSelectedJustificationMenu = itsSketchJustificationMenu;
    itsSelectedJustificationMenu.setState(true);
    itsAutoroutingMenu.setState(itsSketchPad.doAutorouting);
    //itsSketchPad.itsSelectionRouting = itsSketchPad.doAutorouting;
  }

  public void SelectionUpdateMenu(String theFont, Integer theSize, Integer theJustification){
    CheckboxMenuItem aCheckItem = null;
    int i;
    if(itsSelectedFontMenu!=null) itsSelectedFontMenu.setState(false);
    if(theFont!=null){
      for(i=0; i<itsFontsMenu.getItemCount(); i++){
	aCheckItem = (CheckboxMenuItem)itsFontsMenu.getItem(i);
	if(aCheckItem.getLabel().toLowerCase().equals(theFont)){
	  itsSelectedFontMenu = aCheckItem;
	  itsSelectedFontMenu.setState(true);
	  break;
	}
      }
    }
    else itsSelectedFontMenu = null;
    
    if(itsSelectedSizeMenu!=null) itsSelectedSizeMenu.setState(false);
    if(theSize!=null){
      for(i=0; i<itsSizesMenu.getItemCount(); i++){
	aCheckItem = (CheckboxMenuItem)itsSizesMenu.getItem(i);
	if(aCheckItem.getLabel().equals(theSize.toString())){
	  itsSelectedSizeMenu = aCheckItem;
	  itsSelectedSizeMenu.setState(true);
	  break;
	}
      }
    }
    else itsSelectedSizeMenu = null;

    if(itsSelectedJustificationMenu!=null) itsSelectedJustificationMenu.setState(false);
    if(theJustification!=null){
      int aJust = theJustification.intValue();
      if(aJust == ErmesSketchPad.CENTER_JUSTIFICATION){
	for(i=0; i<itsJustificationMenu.getItemCount(); i++){
	  aCheckItem = (CheckboxMenuItem)itsJustificationMenu.getItem(i);
	  if(aCheckItem.getLabel().equals("Center")){
	    itsSelectedJustificationMenu = aCheckItem;
	    itsSelectedJustificationMenu.setState(true);
	    break;
	  }
	}
      }
      else if(aJust == ErmesSketchPad.LEFT_JUSTIFICATION){
	for(i=0; i<itsJustificationMenu.getItemCount(); i++){
	  aCheckItem = (CheckboxMenuItem)itsJustificationMenu.getItem(i);
	  if(aCheckItem.getLabel().equals("Left")){
	    itsSelectedJustificationMenu = aCheckItem;
	    itsSelectedJustificationMenu.setState(true);
	    break;
	  }
	}
      }
      else if(aJust == ErmesSketchPad.RIGHT_JUSTIFICATION){
	for(i=0; i<itsJustificationMenu.getItemCount(); i++){
	  aCheckItem = (CheckboxMenuItem)itsJustificationMenu.getItem(i);
	  if(aCheckItem.getLabel().equals("Right")){
	    itsSelectedJustificationMenu = aCheckItem;
	    itsSelectedJustificationMenu.setState(true);
	    break;
	  }
	}
      }
    }
    else itsSelectedJustificationMenu = null;
    
    UpdateRoutingMenuWithSelection();
  }

  public void UpdateRoutingMenuWithSelection(){
    int temp = itsSketchPad.getSelectionRouting();
    if (temp == 1) itsAutoroutingMenu.setState(true);
    else if (temp ==0) itsAutoroutingMenu.setState(false);
    else itsAutoroutingMenu.setState(itsSketchPad.doAutorouting);
    /*    if(itsSketchPad.itsSelectedConnections.size()!=0){
	  boolean aAutorouting = ((ErmesConnection)itsSketchPad.itsSelectedConnections.elementAt(0)).GetAutorouted();
	  boolean aBothRoutingMode = false;
	  for(int i=1; i<itsSketchPad.itsSelectedConnections.size(); i++){
	  if(aAutorouting!=((ErmesConnection)itsSketchPad.itsSelectedConnections.elementAt(i)).GetAutorouted()){
	  aBothRoutingMode = true;
	  break;
	  }
	  }
	  if(aBothRoutingMode) itsAutoroutingMenu.setState(false);
	  else itsAutoroutingMenu.setState(aAutorouting);
	  }*/
  }

  private void ExecutionMenuAction(MenuItem theMenuItem, String theString) {
    ErmesObject aObject;
    if (theString.equals("Run Mode Ctrl+E")) {
      setRunMode(true);
    }
    else if (theString.equals("Edit Mode Ctrl+E")) {
      setRunMode(false);
    }
  }
  
  private boolean SizesMenuAction(CheckboxMenuItem theMenuItem, int theFontSize) {
    // UI action

    if(itsSelectedSizeMenu != null)
      itsSelectedSizeMenu.setState(false);

    itsSelectedSizeMenu = theMenuItem;
    itsSelectedSizeMenu.setState(true);

    //if we are here, a font size have been choosen from the FONT menu

    if(itsSketchPad.GetSelectedList().size()==0) itsSketchSizeMenu = (CheckboxMenuItem)theMenuItem;
    
    if(itsSketchPad.GetSelectedList().size()==0) {
      itsSketchPad.sketchFontSize = theFontSize;
      itsSketchPad.ChangeFont(new Font(itsSketchPad.sketchFont.getName(), itsSketchPad.sketchFont.getStyle(), theFontSize));
    }
    else itsSketchPad.ChangeSizeFont(theFontSize);
    return true;
  }

  private boolean JustificationMenuAction(MenuItem theMenuItem, String theString) {
    if(itsSelectedJustificationMenu!=null) itsSelectedJustificationMenu.setState(false);
    itsSketchPad.ChangeJustification(theString);
    itsSelectedJustificationMenu = (CheckboxMenuItem)theMenuItem;
    itsSelectedJustificationMenu.setState(true);
    if(itsSketchPad.itsSelectedList.size()==0) itsSketchJustificationMenu = itsSelectedJustificationMenu;
    return true;
  }


  private boolean ResizeObjectMenuAction(MenuItem theMenuItem, String  theString){
    itsCurrentResizeMenu.setState(false);
    itsSketchPad.ChangeResizeMode(theString);
    itsCurrentResizeMenu = (CheckboxMenuItem)theMenuItem;
    itsCurrentResizeMenu.setState(true);
    return true;
  }

	
  ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////focusListener --inizio
  public void focusGained(FocusEvent e){
    if(!itsClosing){
      MaxApplication.SetCurrentWindow(this);
      ErmesSketchPad.RequestOffScreen(itsSketchPad);
      if(itsSketchPad.getGraphics()!= null)
	itsSketchPad.update(itsSketchPad.getGraphics());
    }
  } 

  public void focusLost(FocusEvent e){
    
    //?ErmesSketchPad.RequestOffScreen(itsSketchPad);
    itsSketchPad.itsScrolled = false;
  }

  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////focusListener --fine
  
  ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////WindowListener --inizio  

  public void windowActivated(WindowEvent e){
    requestFocus();
    if(!itsClosing){
      MaxApplication.SetCurrentWindow(this);
      ErmesSketchPad.RequestOffScreen(itsSketchPad);
      if(itsSketchPad.getGraphics()!= null)
	itsSketchPad.update(itsSketchPad.getGraphics());
    }
  }

  ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////WindowListener --fine

  //--------------------------------------------------------
  //	paint
  //--------------------------------------------------------
  public void paint(Graphics g) {
    super.paint(g);
    //2203getContentPane().paintComponents(g);
  }

  //--------------------------------------------------------
  //	SetSnapToGrid
  //--------------------------------------------------------
  public void SetSnapToGrid(){
    itsSketchPad.SetSnapToGrid();
  }
  
  //--------------------------------------------------------
  //	SetAutorouting
  //--------------------------------------------------------
  public void SetAutorouting(boolean t){
    itsSketchPad.SetAutorouting(t);
    itsAutoroutingMenu.setState(t);
  }
  
  private MenuItem getRunModeMenuItem() {
    return itsRunModeMenuItem;
  }

  private MenuItem getSelectAllMenuItem() {
    return itsSelectAllMenuItem;
  }


  public void setRunMode(boolean theRunMode) {
    ErmesObject aObject;

    itsChangingRunEditMode = true;
    MenuItem aRunEditItem = getRunModeMenuItem();
    MenuItem aSelectAllItem = getSelectAllMenuItem();
    if(theRunMode)  
      setBackground(Color.white);
    else setBackground(ErmesSketchPad.sketchColor);
    
    itsSketchPad.SetRunMode(theRunMode);
    for(Enumeration en1 = itsSketchPad.itsElements.elements(); en1.hasMoreElements();) {
      aObject = (ErmesObject)en1.nextElement();
      aObject.RunModeSetted();
    }
    itsToolBar.setRunMode(theRunMode);
    aSelectAllItem.setEnabled(!theRunMode);

    itsRunModeMenuItem.setLabel(theRunMode ? "Edit Mode Ctrl+E" : "Run Mode Ctrl+E");
    requestFocus();
  }

  //--------------------------------------------------------
  // minimumSize()
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
      return getPreferredSize();//(depending on the layout manager).
    }
  
  //--------------------------------------------------------
  // preferredSize()
  //--------------------------------------------------------
  public Dimension getPreferredSize() {
      return preferredsize;
  }

  public void CreateFtsGraphics(ErmesSketchWindow theSketchWindow)
  {
    //create the graphic descriptions for the FtsObjects, before saving them
    ErmesObject aErmesObject = null;
    FtsObject aFObject = null;
    Rectangle aRect = theSketchWindow.getBounds();
    Rectangle aRect1 = theSketchWindow.getContentPane().getBounds();//e.m.1103
      //String ermesInfo = new String();
    
    /*System.err.println("sketchW: "+getBounds());
      System.err.println("sketchW.contentPane: "+getContentPane().getBounds());*/
    theSketchWindow.itsPatcher.put("wx", aRect.x);
    theSketchWindow.itsPatcher.put("wy", aRect.y);
    theSketchWindow.itsPatcher.put("ww", aRect.width-horizontalOffset());//e.m.1103
    theSketchWindow.itsPatcher.put("wh", aRect.height-verticalOffset());//e.m.1103

    if (theSketchWindow.itsSketchPad.doAutorouting) theSketchWindow.itsPatcher.put("autorouting", "on");
    else theSketchWindow.itsPatcher.put("autorouting", "off");

    for (Enumeration e=theSketchWindow.itsSketchPad.itsElements.elements(); e.hasMoreElements();) {
      aErmesObject = (ErmesObject) e.nextElement();
      aFObject = aErmesObject.itsFtsObject;
      if (aFObject == null) continue; //security check!           
      // Set geometrical properties
      
      aFObject.put("x", aErmesObject.itsX);
      aFObject.put("y", aErmesObject.itsY);
      aFObject.put("w", aErmesObject.currentRect.width);
      aFObject.put("h", aErmesObject.currentRect.height);

      // Set the font properties
      if (!aErmesObject.itsFont.getName().equals(theSketchWindow.itsSketchPad.sketchFont.getName()))
	aFObject.put("font", aErmesObject.itsFont.getName());

      if (aErmesObject.itsFont.getSize() != theSketchWindow.itsSketchPad.sketchFont.getSize())
	aFObject.put("fs", aErmesObject.itsFont.getSize());

      if(aErmesObject.itsResized) aFObject.put("resized", "on");
      // if (aErmesObject.itsJustification != itsSketchPad.itsJustificationMode)
      //aFObject.put("jsf", aErmesObject.itsJustification);
      //moved to putOtherProperties

      aErmesObject.putOtherProperties(aFObject);
      if (aErmesObject instanceof ircam.jmax.editors.ermes.ErmesObjExternal &&
	  ((ErmesObjExternal)aErmesObject).itsSubWindow != null)
	CreateFtsGraphics(((ErmesObjExternal)aErmesObject).itsSubWindow); //recursive call
      if (aErmesObject instanceof ircam.jmax.editors.ermes.ErmesObjPatcher &&
	  ((ErmesObjPatcher)aErmesObject).itsSubWindow != null)
	CreateFtsGraphics(((ErmesObjPatcher)aErmesObject).itsSubWindow);
    }
  }
}












