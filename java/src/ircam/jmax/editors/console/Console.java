
package ircam.jmax.editors.console;

import java.awt.*;
import ircam.jmax.*;
import ircam.jmax.utils.*;
import ircam.jmax.editors.ermes.*;
import java.util.*;

/**
 * The ermes message window.
 */
public class Console extends TextWindow implements MaxWindow {
  public Menu itsWindowsMenu;

  Vector itsWindowMenuList;

  public Console(String title) {
    super(title);
    
    itsWindowMenuList = new Vector();

    MenuBar mb = new MenuBar();
    itsWindowsMenu = CreateWindowsMenu();
    mb.add(itsWindowsMenu);
    setMenuBar(mb);
  }

  private Menu CreateWindowsMenu() {
    Menu windowsMenu = new Menu("Windows");
    windowsMenu.add(new MenuItem("Project Manager Ctrl+M"));
    windowsMenu.add(new MenuItem("Jacl Console"));
    AddWindowItems(windowsMenu);
    return windowsMenu;
  }

  private void AddWindowItems(Menu theWindowMenu){
    ErmesSketchWindow aSketchWindow;
    MaxWindow aWindow;
    for (int i=0; i< MaxApplication.itsSketchWindowList.size(); i++) {
      aSketchWindow = (ErmesSketchWindow) MaxApplication.itsSketchWindowList.elementAt(i);
      theWindowMenu.add(new MenuItem(aSketchWindow.getTitle()));
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

  private boolean IsInWindowsMenu(String theName) {
    return(theName.equals("Project Manager Ctrl+M")||theName.equals("Jacl Console")|IsAWindowName(theName)|| IsAnEditorFrameName(theName));
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
      if (IsInWindowsMenu(itemName)) return WindowsMenuAction(aMenuItem, itemName);
    }
    return true;
  }

  private boolean WindowsMenuAction(MenuItem theMenuItem, String theString) {
    if (theString.equals("Project Manager Ctrl+M")) {
      MaxApplication.getApplication().itsProjectWindow.toFront();
    }
    if (theString.equals("Jacl Console")) {
      MaxApplication.getApplication().GetShell().ToFront();
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

   public boolean keyDown(Event evt,int key){
    if (evt.controlDown()){
      if(key == 13) MaxApplication.getApplication().itsProjectWindow.toFront();
      return true;
    }
    return false;
  }
  
  public boolean handleEvent(Event event) {
    if (event.id == Event.WINDOW_DESTROY) {
      return true;
    }
    return super.handleEvent(event);
  }

  public void InitFromDocument(MaxDocument theDocument){}
  
  public Frame GetFrame(){
    return this;
  }

  public void ToFront(){
    toFront();
  }

  public MaxDocument GetDocument(){
    return null;
  }

  public boolean Close(){
    return false;
  }  
}






