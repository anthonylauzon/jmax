package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;


import tcl.lang.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.editors.project.*;
import com.sun.java.swing.*;

/**
 * The window that contains the sketchpad. It knows the document
 * it is showing, and the fospatcher to which it is associated.
 * It handles all the sketch menus, it knows how to load from a fospatcher.
 */
public class ErmesSketchWindow extends MaxEditor {

  public boolean inAnApplet = false;
  public boolean isSubPatcher = false;
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

  private Menu itsJustificationMenu;
  private Menu itsResizeObjectMenu;
  private Menu itsAlignObjectMenu;
  private Menu itsTextMenu;	
  private Menu itsSizesMenu;	
  private Menu itsFontsMenu;
  private Menu itsSubWindowsMenu;
  private Menu itsExecutionMenu;
  private Menu itsGraphicsMenu;
  CheckboxMenuItem itsCurrentSizesMenu;
  CheckboxMenuItem itsCurrentFontMenu;
  CheckboxMenuItem itsCurrentJustificationMenu;
  CheckboxMenuItem itsCurrentResizeMenu;
  CheckboxMenuItem itsAutoroutingCheckbox;
  MenuItem itsRunModeMenuItem;
  MenuItem itsSelectAllMenuItem;
  boolean itsClosing = false;
  boolean itsChangingRunEditMode = false;
  public Vector itsSubWindowList;
  Vector itsWindowMenuList;

  //********************************************************** //
  //START of the transformation of SketchWindow in a MaxEditor //
  //********************************************************** //

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

  //**********************************************************//
  // END of the transormation                                 //
  //**********************************************************//

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


    //--------------------------------------------------------
    //	CONSTRUCTOR FROM DOCUMENT
    //
    //--------------------------------------------------------
    public void InitFromDocument(MaxDocument theDocument) {
		
      Object aObject;
      
      itsDocument = (ErmesPatcherDoc)theDocument;

      FtsWindowDescription aFtsWindow = (FtsWindowDescription) itsDocument.GetFtsPatcher().getWindowDescription();
      //get the FtsWindowDescription, and use it for: reshape to the right dimensions
      setBounds(aFtsWindow.x, aFtsWindow.y, aFtsWindow.width, aFtsWindow.height+80);
      //assigning the right name to the window.

      if((!isSubPatcher)&&(! MaxApplication.doAutorouting)) SetAutorouting();//???
      validate();
      itsSketchPad.InitFromDocument(itsDocument);
      itsSketchPad.repaint();//force a repaint to build an offGraphics context
      validate();
    }

  //--------------------------------------------------------
  // InitSketchWin
  //--------------------------------------------------------
  protected void InitSketchWin(){ 
    
    getContentPane().setLayout(new ErmesToolBarLayout(ErmesToolBarLayout.VERTICAL)); //provaSw: cancellare
    
    setSize(new Dimension(600, 300));//tintin...
    //itsToolBar= new ErmesSwToolbar(null);
    itsToolBar.setSize(600, 30);//provaSw: togliere
    
    getContentPane().add(itsToolBar);//che succedera'?
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
    //AddCheckboxMenuItem(GetEditMenu(),"Autorouting", true);
    itsResizeObjectMenu =  AddSubMenu(GetEditMenu(),"Resize Object");
    FillResizeObjectMenu(itsResizeObjectMenu);
    itsAlignObjectMenu =  AddSubMenu(GetEditMenu(),"Align Objects");
    FillAlignObjectsMenu(itsAlignObjectMenu);
    
    itsExecutionMenu = AddMenu("Execution");
    FillExecutionMenu(itsExecutionMenu);

    GetCutMenu().setEnabled(false);
    GetCopyMenu().setEnabled(false);
    GetPasteMenu().setEnabled(false);
    GetClearMenu().setEnabled(false);
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
    theGraphicsMenu.add(itsAutoroutingCheckbox = new CheckboxMenuItem("Autorouting", true));
    itsAutoroutingCheckbox.addItemListener(this); 
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
 
    theExecutionMenu.add(itsRunModeMenuItem = new MenuItem("Edit Mode Ctrl+E"));
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
    itsCurrentJustificationMenu = aCheckItem;
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
	itsCurrentSizesMenu = aCheckboxMenuItem;
	itsCurrentSizesMenu.setState(true);
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
	itsCurrentFontMenu = aCheckboxMenuItem;
	itsCurrentFontMenu.setState(true);
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
  //	GetDocument
  //	returns the associated ErmesPatcherDoc
  //--------------------------------------------------------
  public MaxDocument GetDocument(){
    return itsDocument;
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
      //#else if(aInt == 77) MaxApplication.GetProjectWindow().toFront();//m
      else if(aInt == 78) MaxApplication.GetConsoleWindow().New();//n
      else if(aInt == 79) MaxApplication.GetConsoleWindow()/*itsProjectWindow*/.Open();//o
      else if(aInt == 80) MaxApplication.ObeyCommand(MaxApplication.PRINT_WINDOW);//p
      else if(aInt == 81) MaxApplication.Quit(); //q
      else if(aInt == 83)itsDocument.Save();//s
      else if(aInt == 87) {//w
	if (isSubPatcher){
	  setVisible(false);
	  itsTopWindow.RemoveFromSubWindowList(this);
	}
	else {
	  //MaxApplication.ObeyCommand(MaxApplication.CLOSE_WINDOW);
	  Close();
	  //dispose();
	}
      }       
      else if(aInt == 69){//e
	if (itsSketchPad.GetRunMode()) setRunMode(false);
	else setRunMode(true);
	return;
	/*ErmesObject aObject;
	  itsChangingRunEditMode = true;
	  MenuItem aRunEditItem = itsWindowsMenu.getItem(0);
	  MenuItem aSelectAllItem = itsEditMenu.getItem(5);
	  if(!itsSketchPad.GetRunMode()){  
	  setBackground(Color.white);
	  itsSketchPad.SetRunMode(true);
	  for(Enumeration en1 = itsSketchPad.itsElements.elements(); en1.hasMoreElements();) {
	  aObject = (ErmesObject)en1.nextElement();
	  aObject.RunModeSetted();
	  }
	  itsToolBar.setRunMode(true);
	  aRunEditItem.setLabel("Edit mode Ctrl+E");
	  aSelectAllItem.setEnabled(false);
	  }
	  else {
	  itsChangingRunEditMode = true;
	  setBackground(ErmesSketchPad.sketchColor);
	  itsSketchPad.SetRunMode(false);
	  itsToolBar.setRunMode(false);
	  aRunEditItem.setLabel("Run mode Ctrl+E");
	  aSelectAllItem.setEnabled(true);
	  }
	  requestFocus();*/
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
	  MaxApplication./*itsProjectWindow.*/GetConsoleWindow().OpenFile(fileToOpen);
      }
    }
  }
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// keyListener --fine

  public boolean Close(){
    itsClosing = true;

    if (!isSubPatcher) itsDocument.itsPatcher.close();
    if(!GetDocument().GetSaveFlag()){
      FileNotSavedDialog aDialog = new FileNotSavedDialog(this);
      aDialog.setLocation(300, 300);
      aDialog.setVisible(true);
      if(aDialog.GetNothingToDoFlag()) return false;
      if(aDialog.GetToSaveFlag()){
	if(!GetDocument().Save()) return false;
      }
      aDialog.dispose();
    }
    ErmesObject aObject;
    FtsObject aFtsObject;
    for (Enumeration e = itsSketchPad.itsElements.elements(); e.hasMoreElements();){
      aObject = (ErmesObject)e.nextElement();
      if(aObject.NeedPropertyHandler()){
	aFtsObject = aObject.GetFtsObject();
	if(aFtsObject!=null) aFtsObject.removePropertyHandler("value", aObject);
      }
    }
    CloseAllSubWindows();//?????

    MaxApplication.RemoveThisWindowFromMenus(this);
    MaxApplication.itsSketchWindowList.removeElement(this);
    itsDocument.itsPatcher.delete();
    itsDocument.DelWindow();
    itsClosing = false;
    setVisible(false);
    dispose();
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

  /*private void CustomEditItemStateChanged(MenuItem theMenuItem, String theString) {    CheckboxMenuItem aCheckItem;

    //if (theString.equals("Snap to Grid")) {
      //MaxApplication.ObeyCommand(MaxApplication.SNAP_TO_GRID);
      
      //aCheckItem = (CheckboxMenuItem)theMenuItem;
      //if(aCheckItem.getState()) aCheckItem.setState(false);
     // else aCheckItem.setState(true);
     // }
    if (theString.equals("Autorouting")) {
      SetAutorouting();
      aCheckItem = (CheckboxMenuItem)theMenuItem;
      if(aCheckItem.getState())aCheckItem.setState(false);
      else aCheckItem.setState(true);
    }
  }*/

  private void GraphicsItemStateChanged(MenuItem theMenuItem, String theString){
    CheckboxMenuItem aCheckItem;
    if (theString.equals("Autorouting")) {
      SetAutorouting();
      //aCheckItem = (CheckboxMenuItem)theMenuItem;
      //if(aCheckItem.getState()) aCheckItem.setState(false);
      //else aCheckItem.setState(true);
    }
  }


  private void AlignObjectsMenuAction(MenuItem theMenuItem, String theString){
    if (theString.equals("Align Top")) itsSketchPad.AlignSelectedObjects("Top");
    else if (theString.equals("Align Left")) itsSketchPad.AlignSelectedObjects("Left");
    else if (theString.equals("Align Right")) itsSketchPad.AlignSelectedObjects("Right");
    else if (theString.equals("Align Bottom")) itsSketchPad.AlignSelectedObjects("Bottom");
  }


  private void FontsMenuAction(MenuItem theMenuItem, String theString) {
    //if we are here, a font name have been choosen from the menu
    itsCurrentFontMenu.setState(false);
    itsSketchPad.ChangeFont(new Font(theString, Font.PLAIN, itsSketchPad.sketchFontSize));
    itsCurrentFontMenu = (CheckboxMenuItem) theMenuItem;
    itsCurrentFontMenu.setState(true);
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
    itsCurrentSizesMenu.setState(false);

    int fontSize = itsSketchPad.sketchFontSize;
    try {
      fontSize = Integer.parseInt(theString);
    } catch (NumberFormatException e) {}
    
    itsCurrentSizesMenu = (CheckboxMenuItem)theMenuItem;
    itsCurrentSizesMenu.setState(true);
    
    itsSketchPad.sketchFontSize = fontSize;
    itsSketchPad.ChangeFont(new Font(itsSketchPad.sketchFont.getName(), itsSketchPad.sketchFont.getStyle(), fontSize));
    return true;
  }

  private boolean JustificationMenuAction(MenuItem theMenuItem, String theString) {
    itsCurrentJustificationMenu.setState(false);
    itsSketchPad.ChangeJustification(theString);
    itsCurrentJustificationMenu = (CheckboxMenuItem)theMenuItem;
    itsCurrentJustificationMenu.setState(true);
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
    itsSketchPad.itsFirstClick = true;
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
      if (isSubPatcher) {
	setVisible(false);
      }
      else {
	//MaxApplication.ObeyCommand(MaxApplication.CLOSE_WINDOW);
	Close();
	//dispose();
      }
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
  public void SetAutorouting(){
    itsSketchPad.SetAutorouting();
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

}




