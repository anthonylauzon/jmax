/*
 * ConsShell.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */
 /* we need this simple derivation of Shell because we need to have the knowledge of
 	ermes inside the cornell university shell.
 	a better design would have been to have a Shell constructor that accepts an interpreter
 	(our MaxInterp)
 */

package ircam.jmax.editors.console;

import java.awt.*;
import java.awt.image.*;
import java.net.*;
import java.applet.*;
import java.util.*;
import cornell.Jacl.*;
import cornell.awt.AWTConsole;


import ircam.jmax.*;
import ircam.jmax.tcl.*;
import ircam.jmax.fts.tcl.*;
import ircam.jmax.editors.ermes.*;
import ircam.jmax.editors.ermes.tcl.*;

/**
 * The TCL shell used inside ermes.
 */

public class ConsShell extends cornell.applet.Shell implements MaxWindow {
  
  public Menu itsWindowsMenu;
  Vector itsWindowMenuList;

  public ConsShell() {    

    super();
    interp = null;
    
    itsWindowMenuList = new Vector();
    
    MenuBar mb = new MenuBar();
    itsWindowsMenu = CreateWindowsMenu();
    mb.add(itsWindowsMenu);
    setMenuBar(mb);
    validate();
  }

  public void init() {     
    if (interp != null) {
      /*
       * Some browsers will call the init() method more than once.
       * If this happens, don't do anything. Otherwise we'd create
       * more than one interpreter thread and things will go into
       * chaos.
       */
      return;
    }

    /*
     * Create the interpreter, initialize the packages and hook      
     * it up with the console.
     */
    
    interp = new MaxInterp(new AWTConsole(0, this, false));
    interp.AddPackage(new cornell.Tcl.TclPackage(interp));
    interp.AddPackage(new cornell.awt.AWTPackage(interp));
    interp.AddPackage(new TclMaxPackage(interp));
    interp.AddPackage(new TclFtsPackage(interp));
    interp.AddPackage(new TclErmesPackage(interp));

    // the html browser

    //interp.AddPackage(new editors.browser.ErmesBrowserPackage(interp));

    /*	  
     * Call the InitEventHandler() method so that the event handler
     * thread is created.
     */
    interp.InitEventHandler();
  }

  public void start() {
    // (ToDo) call some Tcl handler ...
  }
 
  public void stop() {
    // (ToDo) call some Tcl handler ...
  }

  public void destroy() {
    if (interp != null) {
      interp.GetConsole().stop();
      Thread mainThread = interp.GetMainThread();
      if (mainThread != null) {
	mainThread.stop();
      }
    }
    interp = null;
  }
    
  //we no more need a main() function in Ermes
  /*public static void main(String args[]) {
    // main function just create an instance of Shell()...
    Shell aShell = new Shell();
    aShell.init();
    }*/

   private Menu CreateWindowsMenu() {
    Menu windowsMenu = new Menu("Windows");
    windowsMenu.add(new MenuItem("Project Manager"));
    windowsMenu.add(new MenuItem("Ermes Console"));
    AddWindowItems(windowsMenu);
    return windowsMenu;
  }
  
  public void AddWindowItems(Menu theWindowMenu){
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
    return(theName.equals("Project Manager")||theName.equals("Ermes Console")||IsAWindowName(theName)|| IsAnEditorFrameName(theName));
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
    if (theString.equals("Project Manager")) {
      MaxApplication.getApplication().itsProjectWindow.toFront();
    }
    if (theString.equals("Ermes Console")) {
      MaxApplication.getApplication().itsConsole.ToFront();
    }
    else BringToFront(theString);
    return true;
  }
  
  private void BringToFront(String theName){
    ErmesSketchWindow aSketchWindow;
    MaxWindow aWindow;
    for (int i=0; i< MaxApplication.getApplication().itsSketchWindowList.size(); i++) {
      aSketchWindow = (ErmesSketchWindow) MaxApplication.getApplication().itsSketchWindowList.elementAt(i);
      if(aSketchWindow.getTitle().equals(theName)) {
	aSketchWindow.toFront();
	return;
      }
    }
    for (int j=0; j< MaxApplication.getApplication().itsEditorsFrameList.size(); j++) {
      aWindow = (MaxWindow) MaxApplication.getApplication().itsEditorsFrameList.elementAt(j);
      if(aWindow.GetDocument().GetName().equals(theName)) {
	aWindow.ToFront();
	return;
      }
    }
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
    //super.toFront();
    toFront();
  }

  public MaxDocument GetDocument(){
    return new ErmesPatcherDoc();//!!!!!!!!!!!orrore!!!!!!!!!!!!
  }

  public boolean Close(){
    return false;
  }  

}





