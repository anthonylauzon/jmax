
package ircam.jmax;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;

import ircam.jmax.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.editors.project.*; // @@@ !!
import ircam.jmax.editors.ermes.*; // @@@ !!

/**
 * The abstract base class for all the Ermes editors. It provides utility methods
 * such as the Window menu handling, initialisation, and others.
 */
public abstract class MaxEditor extends Frame implements MaxWindow, KeyListener,FocusListener,WindowListener,ActionListener, ItemListener{
  public Menu itsFileMenu;
  public Menu itsProjectMenu;
  public Menu itsNewFileMenu;
  public Menu itsEditMenu;	
  public Menu itsWindowsMenu;

  Vector itsWindowMenuList;
	
  public Project itsProject;
  
  public MaxEditor(String title) {
    super(title);
  }
  
  
  public MaxEditor() {
    super("");
  }
  
  public final void Init(Project theProject){
    itsProject = theProject;
    itsWindowMenuList = new Vector();
    
    MenuBar mb = new MenuBar();
    itsFileMenu = CreateFileMenu();
    itsEditMenu = CreateEditMenu();
    itsProjectMenu = CreateProjectMenu();
    itsWindowsMenu = CreateWindowsMenu();
    mb.add(itsFileMenu);
    mb.add(itsEditMenu);
    mb.add(itsProjectMenu);
    mb.add(itsWindowsMenu);

    setMenuBar(mb);
    
    addKeyListener(this);
  }

  private Menu CreateNewFileMenu(){
    MaxResourceId aResId;
    MenuItem aMenuItem;
    String aString;
    Menu newFileMenu = new Menu("New...  Ctrl+N");
    
    for(int i=0; i< MaxApplication.resourceVector.size();i++){
      aResId = (MaxResourceId)MaxApplication.resourceVector.elementAt(i);
      aString = aResId.GetName();
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

  private Menu CreateProjectMenu() {
    MenuItem aMenuItem;
    Menu ProjectMenu = new Menu("Project");
    ProjectMenu.add(aMenuItem = new MenuItem("Add Window"));
    aMenuItem.addActionListener(this);
    return ProjectMenu;
  }
  private boolean IsInProjectMenu(String theName) {
    return(theName.equals("Add Window"));
  }

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
    windowsMenu.add(aMenuItem = new MenuItem("Project Manager Ctrl+M"));
    aMenuItem.addActionListener(this);
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
    MaxResourceId aResId;
    String aString;
    
    for(int i=0; i< MaxApplication.resourceVector.size();i++){
      aResId = (MaxResourceId)MaxApplication.resourceVector.elementAt(i);
      aString = aResId.GetName();
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
    editMenu.getItem(0).setEnabled(false);
    editMenu.getItem(1).setEnabled(false);
    editMenu.getItem(2).setEnabled(false);
    editMenu.getItem(3).setEnabled(false);
    return editMenu;
  }

  private boolean IsInEditMenu(String theName) {
    return(theName.equals("Cut") || theName.equals("Copy") || theName.equals("Paste") 
		|| theName.equals("Clear"));
  }

  private boolean IsInWindowsMenu(String theName) {
    return(theName.equals("Stack") || theName.equals("Tile") || theName.equals("Tile Vertical")||theName.equals("Project Manager Ctrl+M")||theName.equals("jMax Console  Ctrl+J")||IsAWindowName(theName)|| IsAnEditorFrameName(theName));
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
      if (IsInProjectMenu(itemName)) ProjectMenuAction(aMenuItem, itemName);
      if (IsInWindowsMenu(itemName)) WindowsMenuAction(aMenuItem, itemName);
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
    else if (theString.equals("Close Ctrl+W")) {
      itsProject.CloseThisWindow();
    }
    else if (theString.equals("Print... Ctrl+P")) {
      //MaxApplication.ObeyCommand(MaxApplication.PRINT_WINDOW);
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
      NewFile(theString);
    }
    return true;
  }
  
  public boolean Close(){
    if(!GetDocument().GetSaveFlag()){
      FileNotSavedDialog aDialog = new FileNotSavedDialog(this);
      aDialog.setLocation(300, 300);
      aDialog.setVisible(true);
      if(aDialog.GetNothingToDoFlag()) return false;
      if(aDialog.GetToSaveFlag()){
	GetDocument().Save();
	if(itsProject.HaveAEntry(GetDocument().GetTitle())){
	  if(GetDocument().GetNeverSavedFlag()) itsProject.RemoveFromProject(GetDocument());
	  else itsProject.ResetEntry(GetDocument());
	}
      }
      else{
	if(itsProject.HaveAEntry(GetDocument().GetTitle())) {
	  if(GetDocument().GetNeverSavedFlag()) itsProject.RemoveFromProject(GetDocument());
	  else itsProject.ResetEntry(GetDocument());
	}
      }
      aDialog.dispose();
    }
    else{ 
      if(itsProject.HaveAEntry(GetDocument().GetTitle()))
	itsProject.ResetEntry(GetDocument());
    }
    itsProject.ResetMenus(this);
    itsProject.RemoveFromFrameList(this);
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
    NewFile(aNewFileType);
  }

  private MaxEditor NewFile(String theFileType){
    if(theFileType.equals("patcher")) MaxApplication.ObeyCommand(MaxApplication.NEW_COMMAND);
    else if(theFileType.equals("")) return null;
    /* @@@@@@ Change new to use resources to find dynamically the editor
     */
    return null;//WARNING
  }


  private boolean EditMenuAction(MenuItem theMenuItem, String theString) {
    return true;
  }

  public boolean ProjectMenuAction(MenuItem theMenuItem, String theString) {
    if (theString.equals("Add Window")) {
      //MaxApplication.ObeyCommand(MaxApplication.ADD_WINDOW);
      itsProject.AddToProject(GetDocument(), this);
      itsProject.UpdateProjectMenu();
      //qui deve resettare i menu come in ObeyCommand(MaxApplication.ADD_WINDOW)
    }
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
    else if (theString.equals("Project Manager Ctrl+M")) {
      itsProject.itsProjectWindow.toFront();
    }
    if (theString.equals("jMax Console  Ctrl+J")) {
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

   ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////focusListener --inizio
  public void focusGained(FocusEvent e){
    itsProject.SetThisWindow(this);
  }
  public void focusLost(FocusEvent e){}

  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////focusListener --fine
   ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////WindowListener --inizio  
  public void windowClosing(WindowEvent e){
    itsProject.CloseThisWindow();
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
      return OpenFile(file);
    else
	return false;
  }

  public abstract boolean OpenFile(File file);
  public abstract MaxDocument GetDocument();

  public Frame GetFrame(){
    return this;
  }
  
  public void ToFront(){
    toFront();
  }

  /////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////keyListener --inizio  
  public void keyTyped(KeyEvent e){}
  public void keyReleased(KeyEvent e){}

  public void keyPressed(KeyEvent e){
    int aInt = e.getKeyCode();
    if (e.isControlDown()){
      if(aInt == 77) MaxApplication.GetProjectWindow().toFront();//m
      else if(aInt == 78) New();//n
      else if(aInt == 79) MaxApplication.itsProjectWindow.Open();//o
      else if(aInt == 80)MaxApplication.ObeyCommand(MaxApplication.PRINT_WINDOW);//p
      else if(aInt == 81) MaxApplication.Quit(); //q
      else if(aInt == 83) GetDocument().Save();//s
      else if(aInt == 87) itsProject.CloseThisWindow();//w
    }
  }
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// keyListener --fine

}





