package ircam.jmax; 

import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;
import java.util.*;
import java.io.*;

import com.sun.java.swing.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.editors.console.*;
import ircam.jmax.editors.ermes.*;
import tcl.lang.*;

/**
 * The main application class in jMax. Contains the global parameters 
 * and/or the global functionalities of the system. 
 * It is also the entry point for several TCL commands. It handles, for example:
 * - the startup process, the TCL initialization
 * - the FTS connection
 * - the resource definition loading
 * - a set of global functions related to the window list (stack, tile, close)
 * - file format versions handling
 * - system properties
 * - creation of new windows (new, double-click on a subpatcher, etc.)
 * - quit of the application
 */

public class MaxApplication extends Object {

  // Static global services

  /** Get the unique active TCL interpreter */

  public static Interp getTclInterp()
  {
    return itsInterp;
  }

  public static Clipboard systemClipboard = new Clipboard("system");
  static Interp itsInterp;//e.m.
  public static Vector itsSketchWindowList;
  public static Vector itsEditorsFrameList;
  static ConnectionDialog itsConnDialog;
  public static boolean doAutorouting = true; // Should become a static in the Patcher editor

  public static Properties jmaxProperties;

  public static ErmesSketchWindow itsSketchWindow;
  public static MaxEditor itsWindow;

  static MaxWhenHookTable  itsHookTable;


  static public void AddThisWindowToMenus(ErmesSketchWindow theSketchWindow){
    ErmesSketchWindow aSketchWindow;
    MaxEditor aWindow;
    if(!theSketchWindow.isSubPatcher){
      for(int i=0;i<itsSketchWindowList.size();i++){
	aSketchWindow = (ErmesSketchWindow)itsSketchWindowList.elementAt(i);
	if(aSketchWindow != theSketchWindow) 
	  aSketchWindow.AddWindowToMenu(theSketchWindow.getTitle());
      }
      for(int i=0;i<itsEditorsFrameList.size();i++){
	aWindow = (MaxEditor)itsEditorsFrameList.elementAt(i);
	aWindow.AddWindowToMenu(theSketchWindow.getTitle());
      }

      if (ConsoleWindow.getConsoleWindow() != null)
	ConsoleWindow.getConsoleWindow().AddWindowToMenu(theSketchWindow.getTitle());
    }
  }

  static public void AddThisFrameToMenus(String theName){
    ErmesSketchWindow aSketchWindow;
    MaxEditor aWindow;
    for(int i=0;i<itsSketchWindowList.size();i++){
      aSketchWindow = (ErmesSketchWindow)itsSketchWindowList.elementAt(i);
      aSketchWindow.AddWindowToMenu(theName);
    }
    for(int i=0;i<itsEditorsFrameList.size();i++){
      aWindow = (MaxEditor)itsEditorsFrameList.elementAt(i);
	if(!theName.equals(aWindow.GetTitle()))
	  aWindow.AddWindowToMenu(theName);
    }

    if (ConsoleWindow.getConsoleWindow() != null)
      ConsoleWindow.getConsoleWindow().AddWindowToMenu(theName);
  }

  public static void AddToSubWindowsList(ErmesSketchWindow theTopWindow,ErmesSketchWindow theSubWindow, boolean theFirstItem){
    ErmesSketchWindow aSketchWindow;
    MaxEditor aWindow;
    for(int i=0;i<itsSketchWindowList.size();i++){
      aSketchWindow = (ErmesSketchWindow)itsSketchWindowList.elementAt(i);
      if(aSketchWindow != theTopWindow) 
	aSketchWindow.AddToSubWindowsMenu(theTopWindow.getTitle(), theSubWindow.getTitle(), theFirstItem);
    }
    for(int i=0;i<itsEditorsFrameList.size();i++){
      aWindow = (MaxEditor)itsEditorsFrameList.elementAt(i);
      aWindow.AddToSubWindowsMenu(theTopWindow.getTitle(), theSubWindow.getTitle(), theFirstItem);
    }

    if (ConsoleWindow.getConsoleWindow() != null)
      ConsoleWindow.getConsoleWindow().AddToSubWindowsMenu(theTopWindow.getTitle(), theSubWindow.getTitle(), theFirstItem);
  }

   public static void RemoveFromSubWindowsList(ErmesSketchWindow theTopWindow,ErmesSketchWindow theSubWindow, boolean theLastItem){
    ErmesSketchWindow aSketchWindow;
    MaxEditor aWindow;
    for(int i=0;i<itsSketchWindowList.size();i++){
      aSketchWindow = (ErmesSketchWindow)itsSketchWindowList.elementAt(i);
      if(aSketchWindow != theTopWindow) 
	aSketchWindow.RemoveFromSubWindowsMenu(theTopWindow.getTitle(),theSubWindow.getTitle(),theLastItem);
    }
    for(int i=0;i<itsEditorsFrameList.size();i++){
      aWindow = (MaxEditor)itsEditorsFrameList.elementAt(i);
      aWindow.RemoveFromSubWindowsMenu(theTopWindow.getTitle(), theSubWindow.getTitle(), theLastItem);
    }

    if (ConsoleWindow.getConsoleWindow() != null)
      ConsoleWindow.getConsoleWindow().RemoveFromSubWindowsMenu(theTopWindow.getTitle(), theSubWindow.getTitle(), theLastItem);

    itsSketchWindowList.removeElement(theSubWindow);
  }


  public static void RemoveThisWindowFromMenus(MaxEditor theWindow){
    ErmesSketchWindow aSketchWindow;
    MaxEditor aWindow;
    for(int i=0;i<itsSketchWindowList.size();i++){
      aSketchWindow = (ErmesSketchWindow)itsSketchWindowList.elementAt(i);
      if(aSketchWindow != theWindow) 
	aSketchWindow.RemoveWindowFromMenu(theWindow.GetTitle());
    }
    for(int i=0;i<itsEditorsFrameList.size();i++){
      aWindow = (MaxEditor)itsEditorsFrameList.elementAt(i);
      if(aWindow != theWindow)
	aWindow.RemoveWindowFromMenu(theWindow.GetTitle());
    }
    if (ConsoleWindow.getConsoleWindow() != null)
      ConsoleWindow.getConsoleWindow().RemoveWindowFromMenu(theWindow.GetTitle());
  }
  
  public static void ChangeWinNameMenus(String theOldName, String theNewName){
    ErmesSketchWindow aSketchWindow;
    MaxEditor aWindow;
    for(int i=0;i<itsSketchWindowList.size();i++){
      aSketchWindow = (ErmesSketchWindow)itsSketchWindowList.elementAt(i); 
      aSketchWindow.ChangeWinNameMenu(theOldName,theNewName);
    }
    for(int i=0;i<itsEditorsFrameList.size();i++){
      aWindow = (MaxEditor)itsEditorsFrameList.elementAt(i);
      aWindow.ChangeWinNameMenu(theOldName,theNewName);
    }

    if (ConsoleWindow.getConsoleWindow() != null)
      ConsoleWindow.getConsoleWindow().ChangeWinNameMenu(theOldName, theNewName);
  }
  



  public static void SetCurrentWindow(MaxEditor theWindow){
    if(theWindow instanceof ErmesSketchWindow)itsSketchWindow = (ErmesSketchWindow)theWindow;
    itsWindow = theWindow;
  }

  public static String GetWholeWinName(String theName){
    ErmesSketchWindow aSketchWindow;
    int number = 0;
    String aName;
    for(Enumeration e=itsSketchWindowList.elements(); e.hasMoreElements();){
      aSketchWindow = (ErmesSketchWindow)e.nextElement();
      if(theName.equals(aSketchWindow.itsPatcher.getClassName())) number++;
    }
    if(number>1) aName = theName+" ("+number+")";
    else aName = theName;
    return aName;
  }
  
  public static ErmesSketchWindow GetCurrentWindow() {
    return itsSketchWindow;
  }
	
  /** Functions to add application hooks */

  public static void addHook(String name, String code)
  {
    itsHookTable.addHook(name, code);
  }

  /** Functions to run application hooks */

  public static void runHooks(String name)
  {
    itsHookTable.runHooks(name);
  }

  /** His majesty the main method */

  public static void main(String args[])
  {
    // Initialize swing to use heavyweight components for
    // Menus'

    JPopupMenu.setDefaultLightWeightPopupEnabled(false);

    // main function parse the argument line and create the main class...
    //create a new default Properties object

    jmaxProperties = new Properties(System.getProperties());

    //start parsing arguments
    // don't check for valid options, so the user can set
    // command line arguments that can be accessed from tcl scripts
    // and we don't know yet

    for (int i=0; i<args.length &&args[i].startsWith("-"); i++) 
      jmaxProperties.put(args[i].substring(1), args[++i]);

    // Default values
    if (jmaxProperties.get("root") == null) {
      //user didn't specify the root. Take the /usr/local/max default directory.
      jmaxProperties.put("root", "/usr/local/max");
    }

    //the version number as a system property
    jmaxProperties.put("jmaxVersion", " version 2.1.1 beta");

    itsHookTable = new MaxWhenHookTable(); 

    ircam.jmax.utils.Platform.setValues();

    itsSketchWindowList = new Vector(); // move to MaxWindowManager :->
    itsEditorsFrameList = new Vector();

    // Create and initialize the tcl interpreter

    makeTclInterp(); 

    // Initialize all the submodules; first the kernel modules

    ircam.jmax.mda.MdaModule.initModule();
    ircam.jmax.fts.FtsModule.initModule();

    // than the builtin editors 

    ircam.jmax.editors.console.ConsoleModule.initModule();
    ircam.jmax.editors.ermes.ErmesModule.initModule();

    try
      {
	// Load the "jmaxboot.tcl" file that will do whatever is needed to
	// create the startup configuration, included reading user files
	// installing editors, data types and data handlers

	itsInterp.evalFile(jmaxProperties.getProperty("root") +
			   jmaxProperties.getProperty("file.separator") + "tcl" +
			   jmaxProperties.getProperty("file.separator") +  "jmaxboot.tcl");
      }
    catch (TclException e)
      {
	System.out.println("TCL error in initialization " + e + " : " + itsInterp.getResult());
      }

    // Splash screen moved to a tcl command

    //if there were no connection statements in startup.tcl, ask the user

    if (FtsServer.getServer() == null)
      {
	new ConnectionDialog();
	MaxApplication.runHooks("start");
      }

    // Finally, run forever the notifier loop of the 
    // Tcl interpreter, so that the TCL event system work
    // (and in particular, tcl built panels; thanks to the
    // jacl doc, that make this absolutely unclear.

    Notifier notifier = itsInterp.getNotifier();

    while (true)
      notifier.doOneEvent(TCL.ALL_EVENTS);
 }

  /** This private method build the tcl interpreter, 
      and do all the required initialization relative
      to the jmax package *only*; each module load its
      own tcl commands.
      */

  static private void makeTclInterp()
  {
    itsInterp = new tcl.lang.Interp(); 

    ircam.jmax.tcl.TclMaxPackage.installPackage();
    
    ircam.jmax.editors.ermes.tcl.TclErmesPackage.installPackage();

  }

  public static void Quit()
  {
    MaxEditor aWindow;
    ErmesSketchWindow aSketchWindow;
    boolean someOneNeedSave = false;
    boolean doTheSave = false;

    // First, search if there is anything to save,

    for(Enumeration e=itsSketchWindowList.elements(); e.hasMoreElements();){
      aSketchWindow = (ErmesSketchWindow)e.nextElement();
      if (aSketchWindow.ShouldSave())
	someOneNeedSave = true;
    }

    for(Enumeration e=itsEditorsFrameList.elements(); e.hasMoreElements();){
      aWindow = (MaxEditor)e.nextElement();
      if (aWindow.ShouldSave())
	someOneNeedSave = true;
    }

    // in such case, should give the offer to cancel the quit.

    if (someOneNeedSave)
      {
	QuitDialog quitDialog = new QuitDialog();

	switch (quitDialog.getAnswer())
	  {
	  case QuitDialog.JUST_QUIT:
	    doTheSave = false;
	    break;
	  case QuitDialog.REVIEW_AND_QUIT:
	    doTheSave = true;
	    break;
	  case QuitDialog.CANCEL:
	    return;
	  }

	quitDialog.dispose();
      }

    // Now, the quit is sure, we execute the exit hooks

    runHooks("exit");

    // Then, do the close.
    // At the moment, the close is responsible for handling 
    // the save; this should really be done here, so that we
    // can implement a "SaveAll" option.
    // Also, this force us to skip the clean close in case
    // or a real quit; this is no good, there should be
    // a way to do a Close without being asked for the save !!
    // Another problem, since the semantic "Close" action is
    // mixed with the "user interface" Close action, we cannot
    // take away the "Cancel" from the dialogs here, having
    // as a result a "Cancel" button that do no means anything.

    if (doTheSave)
      {
	for(Enumeration e=itsSketchWindowList.elements(); e.hasMoreElements();){
	  aSketchWindow = (ErmesSketchWindow)e.nextElement();
	  aSketchWindow.Close();
	}

	for(Enumeration e=itsEditorsFrameList.elements(); e.hasMoreElements();){
	  aWindow = (MaxEditor)e.nextElement();
	  aWindow.Close();
	}
      }

    if (ConsoleWindow.getConsoleWindow() != null)
      {
	ConsoleWindow.getConsoleWindow().setVisible(false);
	ConsoleWindow.getConsoleWindow().dispose();
      }

    if (FtsServer.getServer() != null)
      FtsServer.getServer().stop();

    Runtime.getRuntime().exit(0);
  }
}












