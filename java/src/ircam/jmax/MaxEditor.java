
package ircam.jmax;

import java.awt.*;
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
public abstract class MaxEditor extends Frame implements MaxWindow {
  public Menu itsFileMenu;
  public Menu itsProjectMenu;
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
    itsProjectMenu = CreateProjectMenu();
    itsWindowsMenu = CreateWindowsMenu();
    mb.add(itsFileMenu);
    mb.add(itsProjectMenu);
    mb.add(itsWindowsMenu);

    setMenuBar(mb);
  }


  private Menu CreateFileMenu() {
    Menu fileMenu = new Menu("File");
    fileMenu.add(new MenuItem("Open... Ctrl+O"));
    fileMenu.add(new MenuItem("Close Ctrl+W"));
    fileMenu.add(new MenuItem("-"));
    fileMenu.add(new MenuItem("Save Ctrl+S"));
    fileMenu.add(new MenuItem("Save As..."));
    fileMenu.add(new MenuItem("-"));
    fileMenu.add(new MenuItem("Print... Ctrl+P"));
    return fileMenu;
  }

  private Menu CreateProjectMenu() {
    Menu ProjectMenu = new Menu("Project");
    ProjectMenu.add(new MenuItem("Add Window"));
    return ProjectMenu;
  }
  private boolean IsInProjectMenu(String theName) {
    return(theName.equals("Add Window"));
  }

  private Menu CreateWindowsMenu() {
    Menu windowsMenu = new Menu("Windows");
    windowsMenu.add(new MenuItem("Project Manager Ctrl+M"));
    windowsMenu.add(new MenuItem("jMax Console"));
    AddWindowItems(windowsMenu);
    return windowsMenu;
  }

  private void AddWindowItems(Menu theWindowMenu){
    ErmesSketchWindow aSketchWindow;
    ErmesSketchWindow aSubWindow;
    MaxWindow aWindow;
    Menu aMenu;
    for (int i=0; i< MaxApplication.itsSketchWindowList.size(); i++) {
      aSketchWindow= (ErmesSketchWindow) MaxApplication.itsSketchWindowList.elementAt(i);
      if(!aSketchWindow.isSubPatcher) {
	if(aSketchWindow.itsSubWindowList.size()==0)
	  theWindowMenu.add(new MenuItem(aSketchWindow.getTitle()));
	else{
	  aMenu = new Menu(aSketchWindow.getTitle());
	  aMenu.add(new MenuItem(aSketchWindow.getTitle()));
	  for(int k=0; k<aSketchWindow.itsSubWindowList.size(); k++ ){
	    aSubWindow = (ErmesSketchWindow)aSketchWindow.itsSubWindowList.elementAt(k);
	    aMenu.add(new MenuItem(aSubWindow.getTitle()));
	  }
	  theWindowMenu.add(aMenu);
	  itsWindowMenuList.addElement(aMenu);
	}
      }
    }
    for (int j=0; j< MaxApplication.itsEditorsFrameList.size(); j++) {
      aWindow = (MaxWindow) MaxApplication.itsEditorsFrameList.elementAt(j);
     if(aWindow!=this)theWindowMenu.add(new MenuItem(aWindow.GetDocument().GetName()));
    }
  }

  public void AddWindowToMenu(String theName){
    itsWindowsMenu.add(new MenuItem(theName));
  }

  public void AddToSubWindowsMenu(String theTopWindowName, String theSubWindowName, boolean theFirstItem){
    MenuItem aItem;
    Menu aMenu;
    int aIndex = 0;
    if(theFirstItem){
      for(int i=0; i<itsWindowsMenu.countItems();i++){
	aItem = itsWindowsMenu.getItem(i);
	if(aItem.getLabel().equals(theTopWindowName)){
	  itsWindowsMenu.remove(aItem);
	  aIndex = i;
	  break;
	}
      }
      aMenu = new Menu(theTopWindowName);
      aMenu.add(new MenuItem(theTopWindowName));
      aMenu.add(new MenuItem(theSubWindowName));
      itsWindowsMenu.insert(aMenu, aIndex);
      itsWindowMenuList.addElement(aMenu);
    }
    else{
      for(Enumeration e = itsWindowMenuList.elements(); e.hasMoreElements();) {
	aMenu = (Menu) e.nextElement();
	if(theTopWindowName.equals(aMenu.getLabel())){
	  aMenu.add(new MenuItem(theSubWindowName));
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
      for(int i=0; i<itsWindowsMenu.countItems();i++){
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
	  for(int j=0; j<aMenu.countItems();j++){
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
    for(int i=0; i<itsWindowsMenu.countItems();i++){
      aItem = itsWindowsMenu.getItem(i);
      if(aItem.getLabel().equals(theName)){
	itsWindowsMenu.remove(aItem);
	return;
      }
    }
  }

  public void ChangeWinNameMenu(String theOldName, String theNewName){
    MenuItem aItem;
    for(int i=0; i<itsWindowsMenu.countItems();i++){
      aItem = itsWindowsMenu.getItem(i);
      if(aItem.getLabel().equals(theOldName)){
	aItem.setLabel(theNewName);
	return;
      }
    }
  }

  private boolean IsInFileMenu(String theName) {
    return( theName.equals("Open... Ctrl+O")|| theName.equals("Close Ctrl+W") 
	    || theName.equals("Save Ctrl+S") || theName.equals("Save As...") 
	    || theName.equals("Print... Ctrl+P"));
  }

  private boolean IsInWindowsMenu(String theName) {
    return(theName.equals("Project Manager Ctrl+M")||theName.equals("Ermes Console")||theName.equals("Jacl Console")||IsAWindowName(theName)|| IsAnEditorFrameName(theName));
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
      if(aWindow.GetDocument().GetName().equals(theName)) return true;
    }
    return false;
  } 
  

  //--------------------------------------------------------
  //	action
  //	high-level events handler
  //--------------------------------------------------------
  public boolean action(Event event, Object arg) {
    if (event.target instanceof MenuItem) {
      MenuItem aMenuItem = (MenuItem) event.target;
      String itemName = aMenuItem.getLabel();
      if (IsInFileMenu(itemName)) return FileMenuAction(aMenuItem, itemName);
      if (IsInProjectMenu(itemName)) return ProjectMenuAction(aMenuItem, itemName);
      if (IsInWindowsMenu(itemName)) return WindowsMenuAction(aMenuItem, itemName);
    }
    return true;
  }
  
 public boolean FileMenuAction(MenuItem theMenuItem, String theString) {
    if (theString.equals("Open... Ctrl+O")) {
      Open();
    }
    else if (theString.equals("Save Ctrl+S")) {
      GetDocument().Save();
    }
    else if (theString.equals("Save As...")) {
      GetDocument().SetFileName("");
      GetDocument().Save();
    }
    else if (theString.equals("Close Ctrl+W")) {
      itsProject.CloseThisWindow();
    }
    else if (theString.equals("Print... Ctrl+P")) {
      //MaxApplication.getApplication().ObeyCommand(MaxApplication.PRINT_WINDOW);
    }
    return true;
  }
  
  public boolean Close(){
    if(!GetDocument().GetSaveFlag()){
      FileNotSavedDialog aDialog = new FileNotSavedDialog(this);
      aDialog.move(300, 300);
      aDialog.show();
      if(aDialog.GetNothingToDoFlag()) return false;
      if(aDialog.GetToSaveFlag()){
	GetDocument().Save();
	if(itsProject.HaveAEntry(GetDocument().GetWholeName())){
	  if(GetDocument().GetNeverSavedFlag()) itsProject.RemoveFromProject(GetDocument());
	  else itsProject.ResetEntry(GetDocument());
	}
      }
      else{
	if(itsProject.HaveAEntry(GetDocument().GetWholeName())){ 
	  if(GetDocument().GetNeverSavedFlag()) itsProject.RemoveFromProject(GetDocument());
	  else itsProject.ResetEntry(GetDocument());
	}
      }
      aDialog.dispose();
    }
    else{ 
      if(itsProject.HaveAEntry(GetDocument().GetWholeName()))
	itsProject.ResetEntry(GetDocument());
    }
    itsProject.ResetMenus(this);
    itsProject.RemoveFromFrameList(this);
    hide();
    dispose();
    return true;
  }

  public boolean ProjectMenuAction(MenuItem theMenuItem, String theString) {
    if (theString.equals("Add Window")) {
      //MaxApplication.getApplication().ObeyCommand(MaxApplication.ADD_WINDOW);
      itsProject.AddToProject(GetDocument(), this);
      itsProject.UpdateProjectMenu();
      //qui deve resettare i menu come in ObeyCommand(MaxApplication.ADD_WINDOW)
    }
    return true;
  }


  private boolean WindowsMenuAction(MenuItem theMenuItem, String theString) {
    if (theString.equals("Project Manager Ctrl+M")) {
      itsProject.itsProjectWindow.toFront();
    }
    if (theString.equals("jMax Console")) {
      MaxApplication.getApplication().GetConsoleWindow().ToFront();
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
      if(aWindow.GetDocument().GetName().equals(theName)) {
	aWindow.ToFront();
	return;
      }
    }
  }

  public boolean handleEvent(Event event) {
    if (event.id == Event.WINDOW_DESTROY) {
      itsProject.CloseThisWindow();
    }
    else 
      if (event.id == Event.GOT_FOCUS)
	itsProject.SetThisWindow(this);
    return super.handleEvent(event);
  }
  
  public void InitFromDocument(MaxDocument theDocument){}  
  
  public boolean Open(){
    FileDialog fd = new FileDialog(this, "FileDialog");
    String aOpeningFile;
    fd.setFile("");
    fd.show();
    aOpeningFile = fd.getFile();
    if(aOpeningFile==null) return false;
    if(!(aOpeningFile.equals(""))){
     return OpenFile(aOpeningFile, fd.getDirectory());
    }
    return false;
  }

  public abstract boolean OpenFile(String thename, String thePath);
  public abstract MaxDocument GetDocument();

  public Frame GetFrame(){
    return this;
  }
  
  public void ToFront(){
    toFront();
  }

  public boolean keyDown(Event evt,int key){
    if (evt.controlDown()){
      if(key == 13) itsProject.itsProjectWindow.toFront();
      else if(key == 15) Open();
      //else if(key == 16) MaxApplication.getApplication().ObeyCommand(MaxApplication.PRINT_WINDOW);
      else if(key == 19) GetDocument().Save();
      else if(key == 23) itsProject.CloseThisWindow();
    }
    return true;
  }
}





