
package ircam.jmax.editors.console;
import ircam.jmax.*;
import java.io.*;
import java.util.*;
/**
  The (new) TCL console, integrated with the jmax window.
  This class is essentially a thread that opens a window,
  puts a text area into it, and starts the interaction with the user.
  This console also provides a PrintWriter, to be used by other parts
  of the system (es. FTS)
  */

import java.awt.*;
import java.awt.event.*;
import tcl.lang.*;

public class Console extends Thread implements MaxWindow{
  StringBuffer itsSbuf = new StringBuffer();
  TextArea text;
  Frame itsFrame;
  Interp itsInterp;
  PrintWriter itsPrintWriter;

  // "deprecated" variables needed in the MaxWindow interface. Redesign please.
  public Menu itsWindowsMenu;
  Vector itsWindowMenuList;

  public Console(Interp i, int rows, int columns) {
      StringBuffer sSbuf = new StringBuffer();
      itsFrame = new Frame("JMax Console");
      text = new TextArea(rows, columns);
      itsInterp = i;
      itsFrame.add(text);
      itsFrame.setVisible(true);
      // delegate keyboard events
      text.addKeyListener(new ConsoleKeyListener(this));
      //text.addMouseListener(new ConsoleMouseListener());
      /*
       * The console thread runs as a daemon so that it gets terminated 
       * automatically when all other user threads are terminated.
       */
      itsPrintWriter = new PrintWriter(new ConsoleWriter(this));
      setDaemon(true);
  }

  public Frame getFrame() {
   return itsFrame; 
  }
  
  public PrintWriter getPrintWriter() {
    return itsPrintWriter;
  }

  public TextArea GetText() {
    return text;
  }
  
  public void PutLine(String s) {
    //e.m.text.insert(s + "\n", 100000);
    text.append(s + "\n");
  }
  
  public void Put(String s) {
    text.append(s);
    //e.m.text.insert(s, 100000);
  }
  
  /*
   * The AppletConsole thread loops waiting for notification from the
   * ConsoleKeyListener object, via the LineReadyNotify method.
   */
  
  public synchronized void run() {
    PutLine("\n  **** jmax console/ TCL interpreter started ****");
    Put("\n% ");
    while (true) {
      try {
	wait();
	ProcessLine();
      }
      catch (InterruptedException e) {
	System.out.println("AppletConsole: wait error");		
      }
    }
  }
  
  /*
   * The ConsoleKeyListener object tells the console thread that a 
   * line of input is available and run() can proceed.
   */
  
  public synchronized void LineReadyNotify(String s) {
    itsSbuf.append(s);
    notify();
  }
  
  /*
   * If itsSbuf contains a complete command, evaluate it and display the
   * result in the text box.  Otherwise, display the secondary prompt.
   */
  
  private void ProcessLine() {
    String s = itsSbuf.toString();
    
    if (itsInterp.commandComplete(s)) {
      
      try {
	itsInterp.eval(s);
	String result = itsInterp.getResult().toString();
	if (result.length() > 0) {
	  PutLine(result);
	}
      } catch (TclException e) {
	
	if (e.compCode == TCL.OK) {
	  PutLine(itsInterp.getResult().toString());
	} else if (e.compCode == TCL.ERROR) {
	  PutLine(itsInterp.getResult().toString());
	} else {
	  PutLine("command returned bad code: " 
+ e.compCode);
	}
      }
      itsSbuf.setLength(0);
      Put("% ");
    } else {
      Put("> ");
    }
  }
  
  //----------- implementation of the MaxWindow interface -----------//
  
  /**
   *"deprecated"
   */
  public MaxDocument GetDocument(){
    return null;
  }
  
  public Frame GetFrame(){
    return itsFrame;
  }

  public void InitFromDocument(MaxDocument theDocument){}

  public void ToFront(){
    itsFrame.toFront();
  }

  public boolean Close(){
    return false;
  }
  
  //"deprecated" methods. Redesign please
  public void AddWindowToMenu(String theName){
    itsWindowsMenu.add(new MenuItem(theName));
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

}

