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


/**
 * The window that contains the sketchpad. It knows the document
 * it is showing, and the fospatcher to which it is associated.
 * It handles all the sketch menus, it knows how to load from a fospatcher.
 */
public class ErmesSketchWindow extends Frame implements MaxWindow, KeyListener,FocusListener,WindowListener,ActionListener, ItemListener {
  public boolean inAnApplet = false;
  public boolean isSubPatcher = false;
  final String FILEDIALOGMENUITEM = "File dialog...";
  public static int preferredWidth = 490;
  public static int preferredHeight = 450;
  Dimension preferredsize = new Dimension(preferredWidth,preferredHeight);
  public ErmesSketchPad itsSketchPad = new ErmesSketchPad(this);
  public ErmesPatcherDoc itsDocument;
  ErmesScrollerView itsScrollerView = new ErmesScrollerView(this, itsSketchPad);
  ErmesToolBar itsToolBar = new ErmesToolBar(itsSketchPad);
  ErmesSketchWindow itsTopWindow = null;
  ProjectEntry itsProjectEntry = null;
  static String[] itsFontList = Toolkit.getDefaultToolkit().getFontList();


  // Menus are public beacause enabled/disabled from MaxApplication !!!
  // should be done locally

  public Menu itsFileMenu;	
  public Menu itsEditMenu;	
  public Menu itsJustificationMenu;
  public Menu itsFontMenu;	
  public Menu itsSizesMenu;	
  public Menu itsProjectMenu;	
  public Menu itsWindowsMenu;	
  public Menu itsSubWindowsMenu;
  CheckboxMenuItem itsCurrentSizesMenu;
  CheckboxMenuItem itsCurrentFontMenu;
  CheckboxMenuItem itsCurrentJustificationMenu;

  boolean itsClosing = false;
  boolean itsChangingRunEditMode = false;
  public Vector itsSubWindowList;
  Vector itsWindowMenuList;
  
    //--------------------------------------------------------
    //	CONSTRUCTOR
    //
    //--------------------------------------------------------
    public ErmesSketchWindow(boolean theIsSubPatcher, ErmesSketchWindow  theTopWindow) {

      isSubPatcher = theIsSubPatcher;
      itsTopWindow = theTopWindow;
      itsSketchPad.SetToolBar(itsToolBar);	// inform the Sketch of the ToolBar to 
      itsSubWindowList = new Vector();
      itsWindowMenuList = new Vector();
      // communicate with
      //String aFontName = (Toolkit.getDefaultToolkit().getFontList())[0];
      itsSketchPad.setFont(new Font(ircam.jmax.utils.Platform.FONT_NAME, Font.PLAIN, ircam.jmax.utils.Platform.FONT_SIZE));						// communicate with
      
      itsDocument = new ErmesPatcherDoc(this);
      InitSketchWin();
      //if(isSubPatcher){
      //if(itsTopWindow!=null)itsSketchPad.doAutorouting = itsTopWindow.itsSketchPad.doAutorouting;
      //}

      addKeyListener(this);
      addFocusListener(this);
      addWindowListener(this);

      validate();
    }

    //--------------------------------------------------------
    //	CONSTRUCTOR FROM DOCUMENT
    //
    //--------------------------------------------------------
    public void InitFromDocument(MaxDocument theDocument) {
		
      Object aObject;
      
      itsDocument = (ErmesPatcherDoc)theDocument;
      /* if (itsDocument == null) {
	System.out.println("INTERNAL ERROR: try to initialize a window with a null document");
	return;
	} *//*
      else if (itsDocument.GetFtsPatcher() == null) {
	System.out.println("INTERNAL ERROR: document has a null FtsPatcher");
	return;
      }*/

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
    GridBagLayout gridbag = new GridBagLayout();
    GridBagConstraints c = new GridBagConstraints();       
    setLayout(gridbag);
    
    setBackground(ErmesSketchPad.sketchColor);
    //menu & app
    MenuBar mb = new MenuBar();
    
    itsFileMenu = CreateFileMenu();
    mb.add(itsFileMenu);
    
    itsEditMenu = CreateEditMenu();
    mb.add(itsEditMenu);
      
    itsFontMenu = CreateFontMenu();	//this assigns also the itsSizesMenu...
    mb.add(itsFontMenu);
      
    itsProjectMenu = CreateProjectMenu();
    mb.add(itsProjectMenu);
      
    itsWindowsMenu = CreateWindowsMenu();
    mb.add(itsWindowsMenu);
      
    setMenuBar(mb);
    
    CheckDefaultSizeFontMenuItem();
    CheckDefaultFontItem();
      
    itsSubWindowsMenu = new Menu("SubWindows");

    ///////////
    
    c.gridwidth = GridBagConstraints.REMAINDER;
    c.gridx = GridBagConstraints.RELATIVE;
    c.gridy = GridBagConstraints.RELATIVE;
    c.weightx = 0.1;
    c.weighty = 0.0;
    c.anchor = GridBagConstraints.NORTHWEST;
    gridbag.setConstraints(itsToolBar, c);
    add(itsToolBar);
      
    c.weightx = 1.0;
    c.weighty = 1.0;
    c.gridy = 1;
    Insets insets = new Insets(10,0,0,0);
    c.insets = insets;
    c.anchor = GridBagConstraints.EAST;
    c.fill = GridBagConstraints.BOTH;
    gridbag.setConstraints(itsScrollerView, c);
    add(itsScrollerView);
  }
	
  private Menu CreateFileMenu() {
    MenuItem aMenuItem;
    Menu fileMenu = new Menu("File");
    fileMenu.add(aMenuItem = new MenuItem("Close   Ctrl+W"));
    aMenuItem.addActionListener(this);
    fileMenu.add(new MenuItem("-"));
    fileMenu.add(aMenuItem = new MenuItem("Save  Ctrl+S"));
    aMenuItem.addActionListener(this);
    fileMenu.add(aMenuItem = new MenuItem("Save As..."));
    aMenuItem.addActionListener(this);
    fileMenu.add(new MenuItem("-"));
    fileMenu.add(aMenuItem = new MenuItem("Print... Ctrl+P"));
    aMenuItem.addActionListener(this);
    return fileMenu;
  }

  private boolean IsInFileMenu(String theName) {
    return(theName.equals("Close   Ctrl+W")|| theName.equals("Save  Ctrl+S") || theName.equals("Save As...")
	   || theName.equals("Print... Ctrl+P"));
  }

  private boolean IsInSizesMenu(String theName) {
    return(theName.equals("8")|| theName.equals("9")||theName.equals("10")||theName.equals("12")||theName.equals("14")||theName.equals("18")||theName.equals("24")||theName.equals("36")||theName.equals("48"));
  }

  private boolean IsInJustificationMenu(String theName) {
    return(theName.equals("Left")|| theName.equals("Center")||theName.equals("Right"));
  }

  private Menu CreateEditMenu() {
    MenuItem aMenuItem;
    CheckboxMenuItem aCheckItem;
    Menu editMenu = new Menu("Edit");
    editMenu.add(aMenuItem = new MenuItem("Cut"));
    aMenuItem.addActionListener(this);
    editMenu.add(aMenuItem = new MenuItem("Copy"));
    aMenuItem.addActionListener(this);
    editMenu.add(aMenuItem = new MenuItem("Paste"));
    aMenuItem.addActionListener(this);
    editMenu.add(aMenuItem = new MenuItem("Clear"));
    aMenuItem.addActionListener(this);
    editMenu.add(new MenuItem("-"));
    editMenu.add(aMenuItem = new MenuItem("Select All"));
    aMenuItem.addActionListener(this);
    editMenu.add(new MenuItem("-"));
    editMenu.add(aCheckItem = new CheckboxMenuItem("Snap to Grid"));
    aCheckItem.addItemListener(this);
    editMenu.add(new MenuItem("-"));
    aCheckItem = new CheckboxMenuItem("Autorouting", true);
    //aCheckItem.setState(true);
    editMenu.add(aCheckItem);
    aCheckItem.addItemListener(this);
    editMenu.getItem(0).setEnabled(false);
    editMenu.getItem(1).setEnabled(false);
    editMenu.getItem(2).setEnabled(false);
    editMenu.getItem(3).setEnabled(false);
    return editMenu;
  }

  private boolean IsInEditMenu(String theName) {
    return(theName.equals("Cut") || theName.equals("Copy") || theName.equals("Paste") 
	   || theName.equals("Clear") || theName.equals("Select All") 
	   || theName.equals("Snap to Grid") || theName.equals("Autorouting"));
  }
  
  private Menu CreateFontMenu() {
    Menu fontMenu = new Menu("Fonts");
    String aString;
    CheckboxMenuItem aCheckItem;

    itsSizesMenu = new Menu("Sizes");
    itsSizesMenu.add(aCheckItem = new CheckboxMenuItem("8"));
    aCheckItem.addItemListener(this);
    itsSizesMenu.add(aCheckItem = new CheckboxMenuItem("9"));
    aCheckItem.addItemListener(this);
    itsSizesMenu.add(aCheckItem = new CheckboxMenuItem("10"));
    aCheckItem.addItemListener(this);
    itsSizesMenu.add(aCheckItem = new CheckboxMenuItem("12"));
    aCheckItem.addItemListener(this);
    itsSizesMenu.add(aCheckItem = new CheckboxMenuItem("14"));
    aCheckItem.addItemListener(this);
    itsSizesMenu.add(aCheckItem = new CheckboxMenuItem("18"));
    aCheckItem.addItemListener(this);
    itsSizesMenu.add(aCheckItem = new CheckboxMenuItem("24"));
    aCheckItem.addItemListener(this);
    itsSizesMenu.add(aCheckItem = new CheckboxMenuItem("36"));
    aCheckItem.addItemListener(this);
    itsSizesMenu.add(aCheckItem = new CheckboxMenuItem("48"));
    aCheckItem.addItemListener(this);
    fontMenu.add(itsSizesMenu);
    fontMenu.add(new MenuItem("-"));
    for(int i = 0;i<itsFontList.length;i++){
      aString = (String) itsFontList[i];
      fontMenu.add(aCheckItem = new CheckboxMenuItem(aString));
      aCheckItem.addItemListener(this);
    }
    fontMenu.add(new MenuItem("-"));
    itsJustificationMenu = new Menu("Justification");
    itsJustificationMenu.add(aCheckItem = new CheckboxMenuItem("Left"));
    aCheckItem.addItemListener(this);	     
    itsJustificationMenu.add(aCheckItem = new CheckboxMenuItem("Center"));
    aCheckItem.addItemListener(this);	
    aCheckItem.setState(true);
    itsCurrentJustificationMenu = aCheckItem;
    itsJustificationMenu.add(aCheckItem = new CheckboxMenuItem("Right"));
    aCheckItem.addItemListener(this);	   
    fontMenu.add(itsJustificationMenu);

    return fontMenu;
  }

  private boolean IsInFontMenu(String theName) {
    for (int i = 0;i<itsFontList.length;i++) {
      if (theName.equals(itsFontList[i])) return true;
    }
    return false;
  }
	
  private Menu CreateProjectMenu() {
    MenuItem aMenuItem;
    Menu ProjectMenu = new Menu("Project");
    ProjectMenu.add(aMenuItem = new MenuItem("Add Window"));
    aMenuItem.addActionListener(this);
    ProjectMenu.add(aMenuItem = new MenuItem("Add files..."));
    aMenuItem.addActionListener(this);
    ProjectMenu.add(aMenuItem = new MenuItem("Remove files"));
    aMenuItem.addActionListener(this);
    return ProjectMenu;
  }

  private boolean IsInProjectMenu(String theName) {
    return(theName.equals("Add Window")||theName.equals("Add files...")||theName.equals("Remove files"));
  }

  private Menu CreateWindowsMenu() {
    MenuItem aMenuItem;
    Menu windowsMenu = new Menu("Windows");
    windowsMenu.add(aMenuItem = new MenuItem("Run mode Ctrl+E"));
    aMenuItem.addActionListener(this);
    windowsMenu.add(new MenuItem("-"));
    windowsMenu.add(aMenuItem = new MenuItem("Project Manager Ctrl+M"));
    aMenuItem.addActionListener(this);
    windowsMenu.add(aMenuItem = new MenuItem("jMax Console  Ctrl+J"));
    aMenuItem.addActionListener(this);
    AddWindowItems(windowsMenu);
    return windowsMenu;
  }
	
  public void AddWindowToMenu(String theName){
    MenuItem aMenuItem;
    itsWindowsMenu.add(aMenuItem = new MenuItem(theName));
    aMenuItem.addActionListener(this);
  }

  public void RemoveWindowFromMenu(String theName){
    MenuItem aItem;
    for(int i=0; i<itsWindowsMenu.getItemCount();i++){
      aItem = itsWindowsMenu.getItem(i);
      if(aItem.getLabel().equals(theName)){
	itsWindowsMenu.remove(aItem);
	return;
      }
    }
  }
  
  public void ChangeWinNameMenu(String theOldName, String theNewName){
    MenuItem aItem;
    for(int i=0; i<itsWindowsMenu.getItemCount();i++){
      aItem = itsWindowsMenu.getItem(i);
      if(aItem.getLabel().equals(theOldName)){
	aItem.setLabel(theNewName);
	return;
      }
    }
  }

  private void AddWindowItems(Menu theWindowMenu){
    ErmesSketchWindow aSketchWindow;
    ErmesSketchWindow aSubWindow;
    MaxWindow aWindow;
    MenuItem aMenuItem;
    Menu aMenu;
    for (int i=0; i< MaxApplication.itsSketchWindowList.size(); i++) {
      aSketchWindow = (ErmesSketchWindow) MaxApplication.itsSketchWindowList.elementAt(i);
      if(aSketchWindow!= this){
	if(!aSketchWindow.isSubPatcher) {
	  if(aSketchWindow.itsSubWindowList.size()==0){
	    theWindowMenu.add(aMenuItem = new MenuItem(aSketchWindow.getTitle()));
	    aMenuItem.addActionListener(this);
	  }
	  else{
	    aMenu = new Menu(aSketchWindow.getTitle());
	    aMenu.add(aMenuItem = new MenuItem(aSketchWindow.getTitle()));
	    aMenuItem.addActionListener(this);
	    for(int k=0; k<aSketchWindow.itsSubWindowList.size(); k++ ){
	      aSubWindow = (ErmesSketchWindow)aSketchWindow.itsSubWindowList.elementAt(k);
	      if(aSubWindow!=this) {
		aMenu.add(aMenuItem = new MenuItem(aSubWindow.getTitle()));
		aMenuItem.addActionListener(this);
	      }
	    }
	    theWindowMenu.add(aMenu);
	    itsWindowMenuList.addElement(aMenu);
	  }
	}
      }
    }
    for (int j=0; j< MaxApplication.itsEditorsFrameList.size(); j++) {
      aWindow = (MaxWindow) MaxApplication.itsEditorsFrameList.elementAt(j);
      theWindowMenu.add(aMenuItem = new MenuItem(aWindow.GetDocument().GetTitle()));
      aMenuItem.addActionListener(this);
    }
  }

  private boolean IsInWindowsMenu(String theName) {
    return(theName.equals("Edit mode Ctrl+E")||theName.equals("Run mode Ctrl+E")||theName.equals("Project Manager Ctrl+M")||theName.equals("jMax Console  Ctrl+J")||IsAWindowName(theName)|| IsAnEditorFrameName(theName));
  }
	
  private boolean IsAWindowName(String theName){
    ErmesSketchWindow aSketchWindow; 
    for (int i=0; i< MaxApplication.itsSketchWindowList.size(); i++) {
      aSketchWindow = (ErmesSketchWindow) MaxApplication.itsSketchWindowList.elementAt(i);
      if(aSketchWindow.getTitle().equals(theName)) return true;
    }
    return false;
  }
  
  private boolean IsAnEditorFrameName(String theName){
    MaxWindow aWindow; 
    for (int i=0; i< MaxApplication.itsEditorsFrameList.size(); i++) {
      aWindow = (MaxWindow) MaxApplication.itsEditorsFrameList.elementAt(i);
      if(aWindow.GetDocument().GetTitle().equals(theName)) return true;
    }
    return false;
  } 

  private void CheckDefaultSizeFontMenuItem(){
    CheckboxMenuItem aCheckboxMenuItem;
    String aFontSize = String.valueOf(itsSketchPad.getFont().getSize());
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
    for(int i=2; i<itsFontList.length+2; i++){
      aCheckboxMenuItem = (CheckboxMenuItem)itsFontMenu.getItem(i);
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

  public void AddToSubWindowsMenu(String theTopWindowName, String theSubWindowName, boolean theFirstItem){
    MenuItem aItem;
    Menu aMenu;
    int aIndex = 0;
    if(theFirstItem){
      for(int i=0; i<itsWindowsMenu.getItemCount();i++){
	aItem = itsWindowsMenu.getItem(i);
	if(aItem.getLabel().equals(theTopWindowName)){
	  itsWindowsMenu.remove(aItem);
	  aIndex = i;
	  break;
	}
      }
      aMenu = new Menu(theTopWindowName);
      aMenu.add(aItem = new MenuItem(theTopWindowName));
      aItem.addActionListener(this);
      aMenu.add(aItem = new MenuItem(theSubWindowName));
      aItem.addActionListener(this);
      itsWindowsMenu.insert(aMenu, aIndex);
      itsWindowMenuList.addElement(aMenu);
    }
    else{
      for(Enumeration e = itsWindowMenuList.elements(); e.hasMoreElements();) {
	aMenu = (Menu) e.nextElement();
	if(theTopWindowName.equals(aMenu.getLabel())){
	  aMenu.add(aItem = new MenuItem(theSubWindowName));
	  aItem.addActionListener(this);
	  return;
	}
      }
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

  public void RemoveFromSubWindowsMenu(String theTopWindowName,String theSubWindowName,boolean theLastItem){
    MenuItem aItem;
    MenuItem aItem1;
    Menu aMenu;
    int aIndex = 0;
    if(theLastItem){
      for(int i=0; i<itsWindowsMenu.getItemCount();i++){
	aItem = itsWindowsMenu.getItem(i);
	if(aItem.getLabel().equals(theTopWindowName)){
	  itsWindowsMenu.remove(aItem);
	  itsWindowMenuList.removeElement(aItem);
	  aIndex = i;
	  break;
	}
      }
      aItem = new MenuItem(theTopWindowName);
      itsWindowsMenu.insert(aItem, aIndex);
    }
    else{
      for(Enumeration e = itsWindowMenuList.elements(); e.hasMoreElements();) {
	aMenu = (Menu) e.nextElement();
	if(theTopWindowName.equals(aMenu.getLabel())){
	  for(int j=0; j<aMenu.getItemCount();j++){
	    aItem1 = aMenu.getItem(j);
	    if(aItem1.getLabel().equals(theSubWindowName)){
	      aMenu.remove(aItem1);
	      return;
	    }
	  }
	  return;
	}
      }
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
  
  //--------------------------------------------------------
  //	GetProjectEntry
  //	returns the associated ProjectEntry
  //--------------------------------------------------------
  public ProjectEntry GetProjectEntry(){
    return itsProjectEntry;
  }
  /////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////keyListener --inizio  
  public void keyTyped(KeyEvent e){}

  public void keyReleased(KeyEvent e){}

  public void keyPressed(KeyEvent e){
    int aInt = e.getKeyCode();
    if(e.isControlDown()){
      if(aInt == 74) MaxApplication.GetConsoleWindow().ToFront();//j
      else if(aInt == 77) MaxApplication.GetProjectWindow().toFront();//m
      else if(aInt == 78) MaxApplication.itsProjectWindow.New();//n
      else if(aInt == 79) MaxApplication.itsProjectWindow.Open();//o
      else if(aInt == 80) MaxApplication.ObeyCommand(MaxApplication.PRINT_WINDOW);//p
      else if(aInt == 81) MaxApplication.Quit(); //q
      else if(aInt == 83)itsDocument.Save();//s
      else if(aInt == 87) {//w
	if (isSubPatcher){
	  setVisible(false);
	  itsTopWindow.RemoveFromSubWindowList(this);
	}
	else {
	  MaxApplication.ObeyCommand(MaxApplication.CLOSE_WINDOW);
	  dispose();
	}
      }       
      else if(aInt == 69){//e
	ErmesObject aObject;
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
	  itsToolBar.RunModeSetted(true);
	  aRunEditItem.setLabel("Edit mode Ctrl+E");
	  aSelectAllItem.setEnabled(false);
	}
	else {
	  itsChangingRunEditMode = true;
	  setBackground(ErmesSketchPad.sketchColor);
	  itsSketchPad.SetRunMode(false);
	  itsToolBar.RunModeSetted(false);
	  aRunEditItem.setLabel("Run mode Ctrl+E");
	  aSelectAllItem.setEnabled(true);
	}
	requestFocus();
      }
      else if (aInt == 47){//?
	//ask help for the reference Manual for the selected element...
	// open one url *only*, because we open only one browser.
      
	ErmesObject aObject;
	String urlToOpen;
	Interp interp  = MaxApplication.getTclInterp();
	
	if (itsSketchPad.itsSelectedList.size() > 0){
	  aObject = (ErmesObject) itsSketchPad.itsSelectedList.elementAt(0);
	  
	  urlToOpen = aObject.itsFtsObject.getReferenceURL();
	  
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
	
	fileToOpen = aObject.itsFtsObject.getHelpPatch();
	
	if (fileToOpen != null)
	  MaxApplication.itsProjectWindow.OpenFile(fileToOpen);
      }
    }
  }
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// keyListener --fine

  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// itemListener --inizio
  public void itemStateChanged(ItemEvent e){
    if(e.getItemSelectable() instanceof CheckboxMenuItem ){
      CheckboxMenuItem aCheckItem = (CheckboxMenuItem)e.getItemSelectable();
      String itemName = aCheckItem.getLabel();
      
      if (IsInEditMenu(itemName)) EditMenuAction(aCheckItem, itemName);
      if (IsInFontMenu(itemName)) FontMenuAction(aCheckItem, itemName);
      if (IsInSizesMenu(itemName)) SizesMenuAction(aCheckItem, itemName);
      if (IsInJustificationMenu(itemName)) JustificationMenuAction(aCheckItem, itemName);
    }
  }

  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// itemListener --fine
  
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// actionListener --inizio

  public void actionPerformed(ActionEvent e){
    if(e.getSource() instanceof MenuItem ){
      MenuItem aMenuItem = (MenuItem)e.getSource();
      String itemName = aMenuItem.getLabel();
    
      if (IsInFileMenu(itemName)) FileMenuAction(aMenuItem, itemName);
      if (IsInEditMenu(itemName)) EditMenuAction(aMenuItem, itemName);
      if (IsInFontMenu(itemName)) FontMenuAction(aMenuItem, itemName);
      if (IsInProjectMenu(itemName)) ProjectMenuAction(aMenuItem, itemName);
      if (IsInWindowsMenu(itemName)) WindowsMenuAction(aMenuItem, itemName);
    }
  }

  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// actionListener --fine
  private void FileMenuAction(MenuItem theMenuItem, String theString) {
    if (theString.equals("Save  Ctrl+S")) {
      itsDocument.Save();
    }
    if (theString.equals("Save As...")) {
      File file;

      file = MaxFileChooser.chooseFileToSave(this, "Save As ", itsDocument.GetFile());
      
      if (file != null)
	{
	  itsDocument.SetFile(file);
	  itsDocument.Save();
	}
    }
    else if (theString.equals("Close   Ctrl+W")) {
      if (isSubPatcher){
	setVisible(false);
	itsTopWindow.RemoveFromSubWindowList(this);
      }
      else {
	MaxApplication.ObeyCommand(MaxApplication.CLOSE_WINDOW);
	dispose();
      }
    }
    //try to print...
    else if (theString.equals("Print... Ctrl+P")) {
      MaxApplication.ObeyCommand(MaxApplication.PRINT_WINDOW);
    }
  }
	
  public boolean Close(){
    itsClosing = true;

    if (!isSubPatcher) itsDocument.itsPatcher.close();
    if(!GetDocument().GetSaveFlag()){
      FileNotSavedDialog aDialog = new FileNotSavedDialog(this);
      aDialog.setLocation(300, 300);
      aDialog.show();
      if(aDialog.GetNothingToDoFlag()) return false;
      if(aDialog.GetToSaveFlag()){
	GetDocument().Save();
	if(itsProjectEntry!=null) {
	  if(GetDocument().GetNeverSavedFlag()) DiscardFromProject();
	  else itsProjectEntry.Reset();
	}
      }
      else if(itsProjectEntry!=null) DiscardFromProject();
      aDialog.dispose();
    }
    else if(itsProjectEntry!=null) itsProjectEntry.Reset();
    

    CloseAllSubWindows();//?????

    MaxApplication.RemoveThisWindowFromMenus(this);
    MaxApplication.itsSketchWindowList.removeElement(this);
    itsDocument.itsPatcher.delete();
    itsDocument.DelWindow();
    itsClosing = false;
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

  private void DiscardFromProject(){
    itsProjectEntry.itsProject.RemoveFromProject(GetDocument());
    ErmesObjPatcher aPatcher;
    ProjectEntry aEntry;
    for (Enumeration e = itsSketchPad.itsPatcherElements.elements(); e.hasMoreElements();){
      aPatcher = (ErmesObjPatcher)e.nextElement();
      aEntry = MaxApplication.GetProjectWindow().GetProject().GetTheEntry(aPatcher.GetName());
      aEntry.DecAbstractionNumber();
      if(aEntry.GetAbstractionNumber()==0) itsProjectEntry.itsProject.RemoveFromProject(aEntry);
    }
  }

  private void EditMenuAction(MenuItem theMenuItem, String theString) {
    CheckboxMenuItem aCheckItem;
    if (theString.equals("Select All")) {
      // Should move to a SketchPad method
      ErmesObject aObject;
      Vector aElementsList = GetSketchPad().GetElements();
      Vector aSelectedList = GetSketchPad().GetSelectedList();
      for (Enumeration e = aElementsList.elements() ; e.hasMoreElements() ;) {
	aObject = (ErmesObject) e.nextElement();
	aSelectedList.addElement(aObject);
	aObject.Select();
      }
      GetSketchPad().repaint();
    }
    if (theString.equals("Snap to Grid")) {
      MaxApplication.ObeyCommand(MaxApplication.SNAP_TO_GRID);

      aCheckItem = (CheckboxMenuItem)theMenuItem;
      if(aCheckItem.getState()) aCheckItem.setState(false);
      else aCheckItem.setState(true);
    }
    else if (theString.equals("Autorouting")) {
      SetAutorouting();
      aCheckItem = (CheckboxMenuItem)theMenuItem;
      if(aCheckItem.getState())aCheckItem.setState(false);
      else aCheckItem.setState(true);
    }
  }

  private void FontMenuAction(MenuItem theMenuItem, String theString) {
    //if we are here, a font name have been choosen from the menu
    itsCurrentFontMenu.setState(false);
    itsSketchPad.ChangeFont(new Font(theString, Font.PLAIN, itsSketchPad.sketchFontSize));
    itsCurrentFontMenu = (CheckboxMenuItem) theMenuItem;
    itsCurrentFontMenu.setState(true);
  }

  private void ProjectMenuAction(MenuItem theMenuItem, String theString) {
    if (theString.equals("Add Window")) {
      MaxApplication.ObeyCommand(MaxApplication.ADD_WINDOW);
    }
    else if (theString.equals("Add files...")) {
      File file = MaxFileChooser.chooseFileToOpen(this, "Add To Project");

      if (file != null)
	MaxApplication.AddToProject(file);
      else
	return;
    }
    else if (theString.equals("Remove files")) {
      MaxApplication.ObeyCommand(MaxApplication.REMOVE_FILES);
    }
  }

  private void WindowsMenuAction(MenuItem theMenuItem, String theString) {
    ErmesObject aObject;
    if (theString.equals("Run mode Ctrl+E")) {   
      itsChangingRunEditMode = true;
      theMenuItem.setLabel("Edit mode Ctrl+E");
      itsEditMenu.getItem(5).setEnabled(false);//selectall
      setBackground(Color.white);
      itsSketchPad.SetRunMode(true);
      for (Enumeration e = itsSketchPad.itsElements.elements(); e.hasMoreElements();) {
	aObject = (ErmesObject)e.nextElement();
	aObject.RunModeSetted();
      }
      itsToolBar.RunModeSetted(true);
    }
    else if (theString.equals("Edit mode Ctrl+E")) {
      itsChangingRunEditMode = true;
      theMenuItem.setLabel("Run mode Ctrl+E");
      itsEditMenu.getItem(5).setEnabled(true);//selectall
      setBackground(ErmesSketchPad.sketchColor);
      itsSketchPad.SetRunMode(false);
      itsToolBar.RunModeSetted(false);
    }
    else if (theString.equals("Project Manager Ctrl+M")) {
      MaxApplication.GetProjectWindow().toFront();
    }
    else if (theString.equals("jMax Console  Ctrl+J")) {
      MaxApplication.GetConsoleWindow().ToFront();
    }
    else BringToFront(theString);
  }
    
  private void BringToFront(String theName){
    ErmesSketchWindow aSketchWindow;
    MaxWindow aWindow;
    for (int i=0; i< MaxApplication.itsSketchWindowList.size(); i++) {
      aSketchWindow = (ErmesSketchWindow) MaxApplication.itsSketchWindowList.elementAt(i);
      if(aSketchWindow.getTitle().equals(theName)) {
	aSketchWindow.toFront();
	return;
      }
    }

    for (int j=0; j< MaxApplication.itsEditorsFrameList.size(); j++) {
      aWindow = (MaxWindow) MaxApplication.itsEditorsFrameList.elementAt(j);
      if(aWindow.GetDocument().GetTitle().equals(theName)) {
	aWindow.ToFront();
	return;
      }
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
      else {MaxApplication.ObeyCommand(MaxApplication.CLOSE_WINDOW);
      dispose();
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
    Color bg = getBackground();
    g.setColor(bg);
    g.draw3DRect(0, 0, 485, /*45*/37, true);
  }
    
    //--------------------------------------------------------
    //	SetSnapToGrid
    //--------------------------------------------------------
    public void SetSnapToGrid(){
      itsSketchPad.SetSnapToGrid();
    }
    
    //--------------------------------------------------------
    //	SetEntry
    //--------------------------------------------------------
    public void SetEntry(ProjectEntry theProjectEntry){
    	itsProjectEntry = theProjectEntry;
    	/*MenuBar aMenuBar = MaxApplication.GetProjectWindow().getMenuBar();
    	aMenuBar.getMenu(2).getItem(3).enable();*/
    }
    

    //--------------------------------------------------------
    //	SetAutorouting
    //--------------------------------------------------------
    public void SetAutorouting(){
    	itsSketchPad.SetAutorouting();
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









