
package ircam.jmax;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;

import ircam.jmax.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.mda.*;
import ircam.jmax.editors.project.*; // @@@ !!
import ircam.jmax.editors.ermes.*; // @@@ !!
import com.sun.java.swing.*;

/**
 * The abstract base class for all the Ermes editors. It provides utility methods
 * such as the Window menu handling, initialisation, and others.
 */
public abstract class MaxEditor extends JFrame implements MaxWindow, KeyListener,FocusListener,WindowListener,ActionListener, ItemListener{
  public Menu itsFileMenu;
  //public Menu itsProjectMenu;
  public Menu itsNewFileMenu;
  public Menu itsEditMenu;	
  public Menu itsWindowsMenu;

  Vector itsWindowMenuList;
	
  //public Project itsProject;
  
  public MaxEditor(String title) {
    super(title);
  }
  
  
  public MaxEditor() {
    super("");
  }
  
  public final void Init(){
    //itsProject = theProject;
    itsWindowMenuList = new Vector();
    
    MenuBar mb = new MenuBar();
    itsFileMenu = CreateFileMenu();
    itsEditMenu = CreateEditMenu();
    //itsProjectMenu = CreateProjectMenu();
    itsWindowsMenu = CreateWindowsMenu();
    mb.add(itsFileMenu);
    mb.add(itsEditMenu);
    //mb.add(itsProjectMenu);
    //mb.add(itsWindowsMenu);

    setMenuBar(mb);
    
    addKeyListener(this);
    SetupMenu();

    mb.add(itsWindowsMenu);
  }

  private Menu CreateNewFileMenu(){
    MaxDataType aDataType;
    MenuItem aMenuItem;
    String aString;
    Menu newFileMenu = new Menu("New...  Ctrl+N");
    
    // the ResId mechanism is substituted by  the MaxDataType getTypes() call.
    // The installation of MaxDataTypes is dynamic (it's the execution of the 
    // resources.erm tcl script)

    for(Enumeration e = MaxDataType.getTypes().elements(); e.hasMoreElements();) {
      aDataType = (MaxDataType) e.nextElement();
      aString = aDataType.getName();
      newFileMenu.add(aMenuItem = new MenuItem(aString));
      aMenuItem.addActionListener(this);
    }

    return newFileMenu;
  }
  
  private Menu CreateFileMenu() {
    MenuItem aMenuItem;
    CheckboxMenuItem aCheckItem;
    Menu fileMenu = new Menu("File");
    itsNewFileMenu = CreateNewFileMenu();
    fileMenu.add(itsNewFileMenu);
    fileMenu.add(aMenuItem = new MenuItem("Open... Ctrl+O"));
    aMenuItem.addActionListener(this);
    fileMenu.add(aMenuItem = new MenuItem("Import..."));
    aMenuItem.addActionListener(this);
    fileMenu.add(aMenuItem = new MenuItem("Close   Ctrl+W"));
    aMenuItem.addActionListener(this);
    fileMenu.add(new MenuItem("-"));
    aCheckItem = new CheckboxMenuItem("Open with Autorouting");
    aCheckItem.setState(true);
    fileMenu.add(aCheckItem);
    aCheckItem.addItemListener(this);
    fileMenu.add(new MenuItem("-"));
    fileMenu.add(aMenuItem = new MenuItem("Save  Ctrl+S"));
    aMenuItem.addActionListener(this);
    fileMenu.add(aMenuItem = new MenuItem("Save As..."));
    aMenuItem.addActionListener(this);
    fileMenu.add(new MenuItem("-"));
    fileMenu.add(aMenuItem = new MenuItem("Print... Ctrl+P"));
    aMenuItem.addActionListener(this);
    fileMenu.add(aMenuItem = new MenuItem("System statistics..."));
    aMenuItem.addActionListener(this);
    fileMenu.add(aMenuItem = new MenuItem("Quit    Ctrl+Q"));
    aMenuItem.addActionListener(this);
    return fileMenu;
  }

  /*private Menu CreateProjectMenu() {
    MenuItem aMenuItem;
    Menu ProjectMenu = new Menu("Project");
    ProjectMenu.add(aMenuItem = new MenuItem("Add Window"));
    aMenuItem.addActionListener(this);
    return ProjectMenu;
    }
    private boolean IsInProjectMenu(String theName) {
    return(theName.equals("Add Window"));
    }*/

  private Menu CreateWindowsMenu() {
    MenuItem aMenuItem;
    Menu windowsMenu = new Menu("Windows");
     windowsMenu.add(aMenuItem = new MenuItem("Stack"));
    aMenuItem.addActionListener(this);
    windowsMenu.add(aMenuItem = new MenuItem("Tile"));
    aMenuItem.addActionListener(this);
    windowsMenu.add(aMenuItem = new MenuItem("Tile Vertical"));
    aMenuItem.addActionListener(this);
    windowsMenu.add(new MenuItem("-"));
    // windowsMenu.add(aMenuItem = new MenuItem("Project Manager Ctrl+M"));
    //aMenuItem.addActionListener(this);
    windowsMenu.add(aMenuItem = new MenuItem("jMax Console  Ctrl+J"));
    aMenuItem.addActionListener(this);
    AddWindowItems(windowsMenu);
    return windowsMenu;
  }

  private void AddWindowItems(Menu theWindowMenu){
    ErmesSketchWindow aSketchWindow;
    ErmesSketchWindow aSubWindow;
    MaxWindow aWindow;
    MenuItem aMenuItem;
    Menu aMenu;
    for (int i=0; i< MaxApplication.itsSketchWindowList.size(); i++) {
      aSketchWindow= (ErmesSketchWindow) MaxApplication.itsSketchWindowList.elementAt(i);
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
	    aMenu.add(aMenuItem = new MenuItem(aSubWindow.getTitle()));
	    aMenuItem.addActionListener(this);
	  }
	  theWindowMenu.add(aMenu);
	  itsWindowMenuList.addElement(aMenu);
	}
      }
    }
    for (int j=0; j< MaxApplication.itsEditorsFrameList.size(); j++) {
      aWindow = (MaxWindow) MaxApplication.itsEditorsFrameList.elementAt(j);
     if(aWindow!=this){
       theWindowMenu.add(aMenuItem = new MenuItem(aWindow.GetDocument().GetTitle()));
       aMenuItem.addActionListener(this);
     }
    }
  }

  public void AddWindowToMenu(String theName){
    MenuItem aMenuItem;
    itsWindowsMenu.add(aMenuItem = new MenuItem(theName));
    aMenuItem.addActionListener(this);
  }

  public void AddToSubWindowsMenu(String theTopWindowName, String theSubWindowName, boolean theFirstItem){
    MenuItem aItem, aMenuItem;
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
      aMenu.add(aMenuItem = new MenuItem(theTopWindowName));
      aMenuItem.addActionListener(this);
      aMenu.add(aMenuItem = new MenuItem(theSubWindowName));
      aMenuItem.addActionListener(this);
      itsWindowsMenu.insert(aMenu, aIndex);
      itsWindowMenuList.addElement(aMenu);
    }
    else{
      for(Enumeration e = itsWindowMenuList.elements(); e.hasMoreElements();) {
	aMenu = (Menu) e.nextElement();
	if(theTopWindowName.equals(aMenu.getLabel())){
	  aMenu.add(aMenuItem = new MenuItem(theSubWindowName));
	  aMenuItem.addActionListener(this);
	  return;
	}
      }
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
      aItem.addActionListener(this);
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

  private boolean IsInFileMenu(String theName) {
    return( theName.equals("Open... Ctrl+O")||theName.equals("Import...")
	    ||theName.equals("Close   Ctrl+W")||theName.equals("Open with Autorouting") 
	    ||theName.equals("Save  Ctrl+S")||theName.equals("Save As...") 
	    ||theName.equals("Print... Ctrl+P")||theName.equals("Quit    Ctrl+Q")
	    ||theName.equals("System statistics...")||IsInNewFileMenu(theName));
  }
  

  private boolean IsInNewFileMenu(String theName){
    MaxDataType aDataType;
    String aString;
    // to be optimized: this function builds an enumeration for each menu selection 
    // (even if it is not in the "new" menu). A list of datatype names should be kept
    // somewhere
    for(Enumeration e=MaxDataType.getTypes().elements(); e.hasMoreElements();) {
      aDataType = (MaxDataType) e.nextElement();
      aString = aDataType.getName();
      if(aString.equals(theName)) return true;
    }
    return false;
  }
  
  private Menu CreateEditMenu() {
    MenuItem aMenuItem;
    Menu editMenu = new Menu("Edit");
    editMenu.add(aMenuItem = new MenuItem("Cut"));
    aMenuItem.addActionListener(this);
    editMenu.add(aMenuItem = new MenuItem("Copy"));
    aMenuItem.addActionListener(this);
    editMenu.add(aMenuItem = new MenuItem("Paste"));
    aMenuItem.addActionListener(this);
    editMenu.add(aMenuItem = new MenuItem("Clear")); 
    aMenuItem.addActionListener(this);
    return editMenu;
  }

  private boolean IsInEditMenu(String theName) {
    return(theName.equals("Cut") || theName.equals("Copy") || theName.equals("Paste") 
		|| theName.equals("Clear"));
  }

  private boolean IsInWindowsMenu(String theName) {
    return(theName.equals("Stack") || theName.equals("Tile") || theName.equals("Tile Vertical")/*||theName.equals("Project Manager Ctrl+M")*/||theName.equals("jMax Console  Ctrl+J")||IsAWindowName(theName)|| IsAnEditorFrameName(theName));
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
      aWindow = (MaxWindow)MaxApplication.itsEditorsFrameList.elementAt(i);
      if(aWindow.GetDocument().GetTitle().equals(theName)) return true;
    }
    return false;
  } 
  
 ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// itemListener --inizio
  public void itemStateChanged(ItemEvent e){
    if(e.getItemSelectable() instanceof CheckboxMenuItem ){
      CheckboxMenuItem aCheckItem = (CheckboxMenuItem)e.getItemSelectable();
      String itemName = aCheckItem.getLabel();
      if (IsInFileMenu(itemName)) FileMenuAction(aCheckItem, itemName);
      else CustomMenuItemStateChanged(aCheckItem, itemName);
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
      else if (IsInEditMenu(itemName)) EditMenuAction(aMenuItem, itemName);
      //if (IsInProjectMenu(itemName)) ProjectMenuAction(aMenuItem, itemName);
      else if (IsInWindowsMenu(itemName)) WindowsMenuAction(aMenuItem, itemName);
      else CustomMenuActionPerformed(aMenuItem, itemName);
    }
  }

 public boolean FileMenuAction(MenuItem theMenuItem, String theString) {
    if (theString.equals("Open... Ctrl+O")) {
      Open();
    }
    else if (theString.equals("Import...")) {
      // File file = MaxFileChooser.chooseFileToOpen(this, "Import File", itsPatFilter);
      
      //if (file != null)
      //	MaxApplication.Load(file);
      //else
      return false;
    }
    else if (theString.equals("Save Ctrl+S")) {
      GetDocument().Save();
    }
    else if (theString.equals("Save As...")) {
      GetDocument().SetFile(MaxFileChooser.chooseFileToSave(this, "Save As ", GetDocument().GetFile()));
      GetDocument().Save();
    }
    else if (theString.equals("Close   Ctrl+W")) {
      // MaxApplication.ObeyCommand(MaxApplication.CLOSE_WINDOW);//w
      Close();
    }
    else if (theString.equals("Print... Ctrl+P")) {
      Print();
    }
    else if (theString.equals("Quit    Ctrl+Q")) { 
      MaxApplication.Quit();
    }
    else if (theString.equals("Open with Autorouting")) {
      MaxApplication.doAutorouting = !MaxApplication.doAutorouting;
    }
    else if (theString.equals("System statistics...")) {
      StatisticsDialog aDialog = new StatisticsDialog(this);
      aDialog.setLocation(100, 100);
      aDialog.setVisible(true);
    }
    else{//qui siamo nel caso di New...
      MaxApplication.NewFile(theString);
    }
    return true;
  }
  
  public void Print(){}


  public boolean Close(){
    if(!GetDocument().GetSaveFlag()){
      FileNotSavedDialog aDialog = new FileNotSavedDialog(this);
      aDialog.setLocation(300, 300);
      aDialog.setVisible(true);
      if(aDialog.GetNothingToDoFlag()) return false;
      if(aDialog.GetToSaveFlag()){
	GetDocument().Save();
      }
      aDialog.dispose();
    }
    MaxApplication.RemoveThisWindowFromMenus(this);
    MaxApplication.itsEditorsFrameList.removeElement(this);
    setVisible(false);
    dispose();
    return true;
  }

  public void New(){
    String aNewFileType;
    ProjectNewDialog aNewDialog = new ProjectNewDialog(this);
    Point aPoint = getLocation();
    aNewDialog.setLocation(aPoint.x+100, aPoint.y+100);
    aNewDialog.setVisible(true);
    aNewFileType = aNewDialog.GetNewFileType();
    MaxApplication.NewFile(aNewFileType);
  }


  private boolean EditMenuAction(MenuItem theMenuItem, String theString) {
    return true;
  }

  private boolean WindowsMenuAction(MenuItem theMenuItem, String theString) {
    if (theString.equals("Stack")) {
      MaxApplication.StackWindows();
    }
    else if (theString.equals("Tile")) {
      MaxApplication.TileWindows();
    }
    else if (theString.equals("Tile Vertical")) {
      MaxApplication.TileVerticalWindows();
    }
    /*else if (theString.equals("Project Manager Ctrl+M")) {
      itsProject.itsProjectWindow.toFront();
      }*/
    else if (theString.equals("jMax Console  Ctrl+J")) {
      MaxApplication.GetConsoleWindow().ToFront();
    }
    else BringToFront(theString);
    return true;
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

  public abstract boolean CustomMenuActionPerformed(MenuItem theMenuItem, String theString);
  public abstract boolean CustomMenuItemStateChanged(CheckboxMenuItem theMenuItem, String theString);

   ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////focusListener --inizio
  public void focusGained(FocusEvent e){
    MaxApplication.SetCurrentWindow(this);
  }
  public void focusLost(FocusEvent e){}

  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////focusListener --fine
   ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////WindowListener --inizio  
  public void windowClosing(WindowEvent e){
    //MaxApplication.ObeyCommand(MaxApplication.CLOSE_WINDOW);
    Close();
  }
  public void windowOpened(WindowEvent e){}
  public void windowClosed(WindowEvent e){}
  public void windowIconified(WindowEvent e){}       
  public void windowDeiconified(WindowEvent e){}
  public void windowActivated(WindowEvent e){
    requestFocus();
  }
  public void windowDeactivated(WindowEvent e){}  

  ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////WindowListener --fine
  
  public void InitFromDocument(MaxDocument theDocument){}  
  
  public boolean Open(){
    File file = MaxFileChooser.chooseFileToOpen(this, "Open File");

    if (file != null)
      {
	MaxApplication.OpenFile(file);
	return true;
      }
    else
      return false;
  }


  public abstract MaxDocument GetDocument();

  public Frame GetFrame(){
    return this;
  }
  
  public void ToFront(){
    toFront();
  }
  

  public abstract void SetupMenu();
  /////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////keyListener --inizio  
  public void keyTyped(KeyEvent e){}
  public void keyReleased(KeyEvent e){}

  public void keyPressed(KeyEvent e){
    int aInt = e.getKeyCode();
    if (e.isControlDown()){
      /*if(aInt == 77) MaxApplication.GetProjectWindow().toFront();//m
	else*/ 
      if(aInt == 78) New();//n
      else if(aInt == 79) Open();//o
      else if(aInt == 80) Print();//p
      else if(aInt == 81) MaxApplication.Quit(); //q
      else if(aInt == 83) GetDocument().Save();//s
      else if(aInt == 87) Close();//w 
      //MaxApplication.ObeyCommand(MaxApplication.CLOSE_WINDOW);//w
    }
  }
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// keyListener --fine

  public Menu GetFileMenu(){
    return itsFileMenu;
  }
  
  public Menu GetEditMenu(){
    return itsEditMenu;
  }

  public MenuItem GetMenuItem(String theName){
    Menu aMenu;
    MenuItem aItem;
    int j;
    for(int i=0; i<getMenuBar().getMenuCount();i++){
      aMenu = getMenuBar().getMenu(i);
      for(j=0; j<aMenu.getItemCount();j++){
	aItem = aMenu.getItem(j);
	if(aItem.getLabel().equals(theName)) return aItem;
      }
    }
    return null;
  }

  public MenuItem GetNewMenu(){
    return itsFileMenu.getItem(0);
  }

  public MenuItem GetOpenMenu(){
    return itsFileMenu.getItem(1);
  }

  public MenuItem GetImportMenu(){
    return itsFileMenu.getItem(2);
  }

  public MenuItem GetCloseMenu(){
    return itsFileMenu.getItem(3);
  }

  public MenuItem GetOpenWithAutoroutingMenu(){
    return itsFileMenu.getItem(5);
  }

  public MenuItem GetSaveMenu(){
    return itsFileMenu.getItem(7);
  }

  public MenuItem GetSaveAsMenu(){
    return itsFileMenu.getItem(8);
  }
  
  public MenuItem GetPrintMenu(){
    return itsFileMenu.getItem(10);
  }

  public MenuItem GetSystemStatisticsMenu(){
    return itsFileMenu.getItem(11);
  }

  public MenuItem GetQuitMenu(){
    return itsFileMenu.getItem(12);
  }

  public MenuItem GetCutMenu(){
    return itsEditMenu.getItem(0);
  }
  
  public MenuItem GetCopyMenu(){
    return itsEditMenu.getItem(1);
  }
  
  public MenuItem GetPasteMenu(){
    return itsEditMenu.getItem(2);
  }
  
  public MenuItem GetClearMenu(){
    return itsEditMenu.getItem(3);
  }

  public MenuItem AddMenuItem(Menu theMenu, String theName){
    MenuItem aMenuItem;
    theMenu.add(aMenuItem = new MenuItem(theName));
    aMenuItem.addActionListener(this);
    return aMenuItem;
  }
  
  public void AddMenuSeparator(Menu theMenu){
    theMenu.add(new MenuItem("-"));
  }
  
  public Menu AddSubMenu(Menu theMenu, String theSubMenuName){
    Menu aSubMenu =new Menu(theSubMenuName); 
    theMenu.add(aSubMenu);
    return aSubMenu;
  }


  public CheckboxMenuItem AddCheckboxMenuItem(Menu theMenu, String theName, boolean state){
    CheckboxMenuItem aMenuItem;
    theMenu.add(aMenuItem = new CheckboxMenuItem(theName, state));
    aMenuItem.addActionListener(this);
    return aMenuItem;
  }

  public Menu AddMenu(String theName){
    Menu aMenu = new Menu(theName);
    getMenuBar().add(aMenu);
    return aMenu;
  }
}










