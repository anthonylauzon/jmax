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
import ircam.jmax.editors.project.*;
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
      // just an hack: remove the watch temporarly, add it just after.
      //we should avoid the propagations of changing-property to the originator
      //of the change itself
      itsPatcher.removeWatch(this);
      itsSketchPad.itsHelper.DeleteGraphicObject((ErmesObject)(((FtsObject)value).getRepresentation()));
      itsPatcher.watch("deletedObject", this);
      //another problem: allow the watch removing for single properties!
      itsPatcher.watch("deletedConnection", this);
    }
    else if (name.equals("deletedConnection")) {
      //see previous comment..
      System.err.println(value.getClass().getName());
      /*itsPatcher.removeWatch(this);
	itsSketchPad.itsHelper.DeleteConnectionByInOut(ErmesObject)(((FtsObject)value).getRepresentation()));
	itsPatcher.watch("deletedObject", this);
	itsPatcher.watch("deletedConnection", this);*/
    }
  }

  FtsSelection itsSelection;
  Vector ftsObjectsPasted = new Vector();
  Vector ftsConnectionsPasted = new Vector();
  public static ErmesClipboardProvider itsClipboardProvider = new ErmesClipboardProvider();
  public boolean inAnApplet = false;
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
  public boolean alreadySaved =true;
  boolean neverSaved =true;
  //public String itsTitle;
  public MaxData itsData;
  static int untitledCounter = 1;

  public boolean CustomMenuActionPerformed(MenuItem theMenuItem, String itemName){

    if(IsInCustomEditMenu(itemName)) 
      CustomEditMenuAction(theMenuItem, itemName);
    if(IsInAlignObjectsMenu(itemName)) 
      AlignObjectsMenuAction(theMenuItem, itemName);
    if(IsInExecutionMenu(itemName)) 
      ExecutionMenuAction(theMenuItem, itemName);
    
    return true;
  }

  
  
  public boolean CustomMenuItemStateChanged(CheckboxMenuItem theCheckItem, String itemName){
    if (IsInGraphicsMenu(itemName)) 
      GraphicsItemStateChanged(theCheckItem, itemName);
    if (IsInFontsMenu(itemName)) 
      FontsMenuAction(theCheckItem, itemName);
    if (IsInSizesMenu(itemName)) 
      SizesMenuAction(theCheckItem, itemName);
    if(IsInJustificationMenu(itemName)) 
      JustificationMenuAction(theCheckItem, itemName);
    if(IsInResizeObjectMenu(itemName)) 
      ResizeObjectMenuAction(theCheckItem, itemName);
    return true;
  }


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
    //else setTitle(theData.getName());
    itsData = theData;
    itsPatcher = (FtsContainerObject)(theData.getContent());
    CommonInitializations();
    isSubPatcher = false;
    //setTitle(theData.getName());
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
  }
   
  /**
   *utility function, used to avoid to replicate code in the two contructors
   */
  void CommonInitializations() {
    
    itsSketchPad.setFont(new Font(ircam.jmax.utils.Platform.FONT_NAME, Font.PLAIN, ircam.jmax.utils.Platform.FONT_SIZE));						// communicate with
    Init(); //MaxEditor base class init (standard menu handling)
    //isSubPatcher = false;
    isAbstraction = false;
    //itsTopWindow = null;
    itsSketchPad.SetToolBar(itsToolBar);	// inform the Sketch of the ToolBar to 
    itsSubWindowList = new Vector();
    itsWindowMenuList = new Vector();

      
    //itsDocument = new ErmesPatcherDoc(this);
    InitSketchWin();
    validate();
    //*-
    //    itsSketchWindow = new ErmesSketchWindow(false, itsSketchWindow, false);
    //itsSketchWindow.Init();
    itsPatcher.open();
    //repaint();
    MaxApplication.itsWindow = this;
    InitFromContainer(itsPatcher);
    inAnApplet = false;
    //aPatcherDoc.SetWindow(itsSketchWindow);
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
    
    //SetupMenu();
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

      setBounds(x, y, width, height);

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
    
    itsGraphicsMenu = AddMenu("Graphics");
    FillGraphicsMenu(itsGraphicsMenu);
    
    itsTextMenu = AddMenu("Text");
    FillTextMenu(itsTextMenu);
    CheckDefaultSizeFontMenuItem();
    CheckDefaultFontItem();
    
    itsSubWindowsMenu = new Menu("SubWindows");
    itsSelectAllMenuItem = AddMenuItem(GetEditMenu(),"Select All  Ctrl+A");
    AddMenuSeparator(GetEditMenu());
    itsResizeObjectMenu =  AddSubMenu(GetEditMenu(),"Resize Object");
    FillResizeObjectMenu(itsResizeObjectMenu);
    AddMenuSeparator(GetEditMenu());
    itsAlignObjectMenu =  AddSubMenu(GetEditMenu(),"Align Objects");
    FillAlignObjectsMenu(itsAlignObjectMenu);
    
    itsExecutionMenu = AddMenu("Execution");
    FillExecutionMenu(itsExecutionMenu);

    GetCutMenu().setEnabled(true);
    GetCopyMenu().setEnabled(true);
    GetPasteMenu().setEnabled(true);
    GetClearMenu().setEnabled(false);
  }

  protected boolean Cut(){
    boolean temp = Copy();
    itsSketchPad.itsHelper.DeleteSelected();
    return temp;
  }

  // clipboard handling
  protected boolean Copy() {
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
    return true;
  }



  protected boolean Paste() {
    String tclScriptToExecute = null;
    
    // take the objects list from the clipboard, if any. Only tclGroups for now
    Transferable aTransferable = MaxApplication.systemClipboard.getContents(this);
    if ((aTransferable == null) || !aTransferable.isDataFlavorSupported(ErmesClipboardProvider.tclGroupFlavor))  return false;

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
    return true;
  }

  private boolean IsInSizesMenu(String theName) {
    return(theName.equals("8")|| theName.equals("9")||theName.equals("10")||theName.equals("12")||theName.equals("14")||theName.equals("18")||theName.equals("24")||theName.equals("36")||theName.equals("48"));
  }

  private boolean IsInCustomEditMenu(String theName) {
    return(theName.equals("Select All  Ctrl+A"));
  }

  private boolean IsInJustificationMenu(String theName) {
    return(theName.equals("Left")|| theName.equals("Center")||theName.equals("Right"));
  }
  
  private boolean IsInGraphicsMenu(String theName) {
    return(theName.equals("Autorouting"));
  }
  
  private void FillGraphicsMenu(Menu theGraphicsMenu){
    CheckboxMenuItem aCheckItem;
    theGraphicsMenu.add(itsAutoroutingMenu = new CheckboxMenuItem("Autorouting", true));
    itsAutoroutingMenu.addItemListener(this); 

  }


  private void FillResizeObjectMenu(Menu theResizeObjectMenu){
    CheckboxMenuItem aCheckItem;
    theResizeObjectMenu.add(aCheckItem = new CheckboxMenuItem("Both"));
    aCheckItem.addItemListener(this);	   
    aCheckItem.setState(true);
    itsCurrentResizeMenu = aCheckItem;
    theResizeObjectMenu.add(aCheckItem = new CheckboxMenuItem("Horizontal"));
    aCheckItem.addItemListener(this);	
    theResizeObjectMenu.add(aCheckItem = new CheckboxMenuItem("Vertical"));
    aCheckItem.addItemListener(this);   
  }
  
  private void FillAlignObjectsMenu(Menu theAlignObjectMenu){
    MenuItem aMenuItem;
    theAlignObjectMenu.add(aMenuItem = new MenuItem("Align Top"));
    aMenuItem.addActionListener(this);
    theAlignObjectMenu.add(aMenuItem = new MenuItem("Align Left"));
    aMenuItem.addActionListener(this);
    theAlignObjectMenu.add(aMenuItem = new MenuItem("Align Bottom"));
    aMenuItem.addActionListener(this);
    theAlignObjectMenu.add(aMenuItem = new MenuItem("Align Right"));
    aMenuItem.addActionListener(this);

  }

  private void FillExecutionMenu(Menu theExecutionMenu){
 
    theExecutionMenu.add(itsRunModeMenuItem = new MenuItem("Run Mode Ctrl+E"));
    itsRunModeMenuItem.addActionListener(this);
  }

  private boolean IsInResizeObjectMenu(String theName) {
    return(theName.equals("Both") || theName.equals("Horizontal") || theName.equals("Vertical"));
  }
  
  private boolean IsInAlignObjectsMenu(String theName) {
    return(theName.equals("Align Top") || theName.equals("Align Left") || 
	   theName.equals("Align Right")||theName.equals("Align Bottom"));
  }

  private boolean IsInExecutionMenu(String theName) {
    return(theName.equals("Edit Mode Ctrl+E") || theName.equals("Run Mode Ctrl+E"));
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

  private void FillSizesMenu(Menu theSizesMenu) {
    CheckboxMenuItem aCheckItem;

    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("8"));
    aCheckItem.addItemListener(this);
    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("9"));
    aCheckItem.addItemListener(this);
    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("10"));
    aCheckItem.addItemListener(this);
    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("12"));
    aCheckItem.addItemListener(this);
    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("14"));
    aCheckItem.addItemListener(this);
    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("18"));
    aCheckItem.addItemListener(this);
    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("24"));
    aCheckItem.addItemListener(this);
    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("36"));
    aCheckItem.addItemListener(this);
    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("48"));
    aCheckItem.addItemListener(this);
  }

  private void FillFontMenu(Menu theFontMenu) {
    CheckboxMenuItem aCheckItem;
    String aString;

    for(int i = 0;i<itsFontList.length;i++){
      aString = (String) itsFontList[i];
      theFontMenu.add(aCheckItem = new CheckboxMenuItem(aString));
      aCheckItem.addItemListener(this);
    }
  }

  private void FillJustificationMenu(Menu theJustificationMenu) {
    CheckboxMenuItem aCheckItem;

    theJustificationMenu.add(aCheckItem = new CheckboxMenuItem("Left"));
    aCheckItem.addItemListener(this);	     
    theJustificationMenu.add(aCheckItem = new CheckboxMenuItem("Center"));
    aCheckItem.addItemListener(this);	
    aCheckItem.setState(true);
    itsSelectedJustificationMenu = aCheckItem;
    itsSketchJustificationMenu = itsSelectedJustificationMenu;
    theJustificationMenu.add(aCheckItem = new CheckboxMenuItem("Right"));
    aCheckItem.addItemListener(this);	   
  }

  private boolean IsInFontsMenu(String theName) {
    for (int i = 0;i<itsFontList.length;i++) {
      if (theName.equals(itsFontList[i])) return true;
    }
    return false;
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

  public void keyPressed(KeyEvent e){
    int aInt = e.getKeyCode();
    if(e.isControlDown()){
      if(aInt == 74) MaxApplication.GetConsoleWindow().ToFront();//j
      else if(aInt == 65) itsSketchPad.SelectAll();//a
      else if(aInt == 67) Copy();//c
      else if(aInt == 78) MaxApplication.GetConsoleWindow().New();//n
      else if(aInt == 79) MaxApplication.GetConsoleWindow().Open();//o
      else if(aInt == 80) MaxApplication.ObeyCommand(MaxApplication.PRINT_WINDOW);//p
      else if(aInt == 81) MaxApplication.Quit(); //q
      else if(aInt == 83)Save();//s
      else if(aInt == 86) Paste();//v
      else if(aInt == 88) Cut();//x
      else if(aInt == 87) {//w
	if (isSubPatcher){
	  setVisible(false);
	  itsTopWindow.RemoveFromSubWindowList(this);
	}
	else {
	  Close();
	}
      }       
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
    }
  }
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// keyListener --fine
  public boolean Close(){
    if(isSubPatcher) return Close(false);
    else return Close(true);
  }

  public boolean Close(boolean deleteOnFts){
    itsClosing = true;

    if (deleteOnFts) itsPatcher.close();
    if(!(alreadySaved || isSubPatcher)){
      FileNotSavedDialog aDialog = new FileNotSavedDialog(this);
      aDialog.setLocation(300, 300);
      aDialog.setVisible(true);
      if(aDialog.GetNothingToDoFlag()) return false;
      if(aDialog.GetToSaveFlag()){
	Save();
      }
      aDialog.dispose();
    }
    ErmesObject aObject;
    FtsObject aFtsObject;
    for (Enumeration e = itsSketchPad.itsElements.elements(); e.hasMoreElements();){
      aObject = (ErmesObject)e.nextElement();
      if(aObject.NeedPropertyHandler()){
	aFtsObject = aObject.GetFtsObject();
	if(aFtsObject!=null) aFtsObject.removeWatch(aObject);
      }
    }
    CloseAllSubWindows();//?????

    MaxApplication.RemoveThisWindowFromMenus(this);
    MaxApplication.itsSketchWindowList.removeElement(this);
    if (deleteOnFts) itsPatcher.delete();
    itsClosing = false;
    setVisible(false);
    if (deleteOnFts) dispose();
    return true;
  }

  public boolean ShouldSave() {
    return !alreadySaved;
  }

  private boolean SaveBody(){
    // This line will not work for patches loaded from
    // sources that are not files !!!
      
    //e.m.1103setTitle(((MaxFileDataSource)itsData.getDataSource()).getFile().getName()); 
    setTitle(itsData.getDataSource().toString()); 
    CreateFtsGraphics(this);

    // This code is temporary, just to test the MDA
    // save architecture; real code will substitute
    // the whole thing.

    try
      {
	itsData.setInfo("Saved " + DateFormat.getDateInstance(DateFormat.FULL).format(new Date()));
	itsData.save();
      }
    catch (MaxDataException e)
      {
	System.out.println("ERROR " + e + " while saving " + itsData.getDataSource());
	e.printStackTrace(); // temporary, MDC
	return false;
      }

    alreadySaved = true;
    neverSaved = false;
    return true;
  }

  public boolean Save() {
    // first, tentative implementation:
    // the FILE is constructed now, and the ErmesSketchPad SaveTo method is invoked.
    // we should RECEIVE this FILE, or contruct it when we load this document
		
    // The "canSave" method of a data tell if it can be saved
    // i.e. if it have a data source, and if we can write to its data source

    if (itsData.canSave())
      {
	if(!SaveBody()) return false;
	alreadySaved = true;
	return true;
      }
    else
      return SaveAs();
  }

  public boolean SaveAs() {
    String oldTitle = getTitle();
    MaxDataSource source;

    source  = MaxFileChooser.chooseFileToSave(this, "Save As", itsData.getDataSource());

    if (source == null)
      return false;
    else
      itsData.setDataSource(source);
    
    if(!SaveBody()) return false;
    MaxApplication.ChangeWinNameMenus(oldTitle, getTitle());
    return true;
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
	    aExternal.itsSubWindow.dispose();
	    aExternal.itsSubWindow = null;
	  }
	}
      }
      else if(aObject instanceof ErmesObjPatcher){
	aPatcher = (ErmesObjPatcher)aObject;
	if(aPatcher.itsSubWindow!=null){
	  aPatcher.itsSubWindow.CloseAllSubWindows();
	  aPatcher.itsSubWindow.setVisible(false);
	  aPatcher.itsSubWindow.dispose();
	  aPatcher.itsSubWindow = null;
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



  private void CustomEditMenuAction(MenuItem theMenuItem, String theString) {

    if (theString.equals("Select All  Ctrl+A")) GetSketchPad().SelectAll();
  }


  private void GraphicsItemStateChanged(MenuItem theMenuItem, String theString){
    if (theString.equals("Autorouting")) {
      SetAutorouting(((CheckboxMenuItem) theMenuItem).getState());
    }
  }
  

  private void AlignObjectsMenuAction(MenuItem theMenuItem, String theString){
    if (theString.equals("Align Top")) itsSketchPad.AlignSelectedObjects("Top");
    else if (theString.equals("Align Left")) itsSketchPad.AlignSelectedObjects("Left");
    else if (theString.equals("Align Right")) itsSketchPad.AlignSelectedObjects("Right");
    else if (theString.equals("Align Bottom")) itsSketchPad.AlignSelectedObjects("Bottom");
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
  
  private boolean SizesMenuAction(MenuItem theMenuItem, String theString) {
    //if we are here, a font size have been choosen from the FONT menu
    if(itsSelectedSizeMenu!=null) itsSelectedSizeMenu.setState(false);

    int fontSize = itsSketchPad.sketchFontSize;
    try {
      fontSize = Integer.parseInt(theString);
    } catch (NumberFormatException e) {}
    
    if(itsSketchPad.GetSelectedList().size()==0) itsSketchSizeMenu = (CheckboxMenuItem)theMenuItem;
    itsSelectedSizeMenu = (CheckboxMenuItem)theMenuItem;
    
    itsSelectedSizeMenu.setState(true);
    
    if(itsSketchPad.GetSelectedList().size()==0) {
      itsSketchPad.sketchFontSize = fontSize;
      itsSketchPad.ChangeFont(new Font(itsSketchPad.sketchFont.getName(), itsSketchPad.sketchFont.getStyle(), fontSize));
    }
    else itsSketchPad.ChangeSizeFont(fontSize);
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
    //itsSketchPad.itsFirstClick = true;
    ErmesSketchPad.RequestOffScreen(itsSketchPad);
    itsSketchPad.itsScrolled = false;
  }

  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////focusListener --fine
  
  ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////WindowListener --inizio  
  public void windowClosing(WindowEvent e){
    if (inAnApplet) {
      dispose();
    } else {
      if (isSubPatcher) setVisible(false);
      else Close();
    }
  }
  public void windowOpened(WindowEvent e){}
  public void windowClosed(WindowEvent e){}
  public void windowIconified(WindowEvent e){}       
  public void windowDeiconified(WindowEvent e){}
  public void windowActivated(WindowEvent e){
    requestFocus();
    if(!itsClosing){
      MaxApplication.SetCurrentWindow(this);
      ErmesSketchPad.RequestOffScreen(itsSketchPad);
      if(itsSketchPad.getGraphics()!= null)
	itsSketchPad.update(itsSketchPad.getGraphics());
    }
  }
  public void windowDeactivated(WindowEvent e){}  

  ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////WindowListener --fine

  //--------------------------------------------------------
  //	paint
  //--------------------------------------------------------
  public void paint(Graphics g) {
    super.paint(g);
    getContentPane().paintComponents(g);
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
    if (theRunMode)
      aRunEditItem.setLabel("Edit Mode Ctrl+E");
    else aRunEditItem.setLabel("Run Mode Ctrl+E");
    aSelectAllItem.setEnabled(!theRunMode);
    requestFocus();
  }

  public void ToFront(){
    toFront();
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
    String ermesInfo = new String();
    

    itsPatcher.put("wx", aRect.x);
    itsPatcher.put("wy", aRect.y);
    itsPatcher.put("ww", aRect.width);
    itsPatcher.put("wh", aRect.height);

    if (itsSketchPad.doAutorouting) itsPatcher.put("autorouting", "on");
    else itsPatcher.put("autorouting", "off");

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

      if (! aErmesObject.itsFont.getName().equals(aErmesObject.itsSketchPad.sketchFont.getName()))
	aFObject.put("font", aErmesObject.itsFont.getName());

      if (aErmesObject.itsFont.getSize() != aErmesObject.itsSketchPad.sketchFont.getSize())
	aFObject.put("fs", aErmesObject.itsFont.getSize());

      if (aErmesObject instanceof ircam.jmax.editors.ermes.ErmesObjExternal &&
	  ((ErmesObjExternal)aErmesObject).itsSubWindow != null)
	CreateFtsGraphics(((ErmesObjExternal)aErmesObject).itsSubWindow); //recursive call
    }
  }
}




