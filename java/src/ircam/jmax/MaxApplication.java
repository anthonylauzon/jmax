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

public class MaxApplication extends Object
{
  // (fd) changed because FtsServer uses MaxApplication.getProperty for timeOut
  // This is not clean because you cannot use the application layer without calling
  // MaxApplication.main !!! 
  // i.e. cannot write an application in Java that is not jMax...

  //  private static Properties jmaxProperties;
  private static Properties jmaxProperties = new Properties(System.getProperties());

  /** Method to get system wide properties, stored
    in jmaxProperties */

  public static String getProperty(String key)
  {
    return jmaxProperties.getProperty(key);
  }

  /** Method to get system wide properties, stored
    in jmaxProperties */

  public static String getProperty(String key, String defaultValue)
  {
    return jmaxProperties.getProperty(key, defaultValue);
  }

  public static Properties getProperties()
  {
    return jmaxProperties;
  }

  /** Get the unique active TCL interpreter */

  static private Interp itsInterp = null;

  public static Interp getTclInterp()
  {
    if (itsInterp == null)
      makeTclInterp();

    return itsInterp;
  }

  public static Clipboard systemClipboard = new Clipboard("system");




  private static ErmesSketchWindow itsSketchWindow;

  static MaxWhenHookTable  itsHookTable = null;

  /**
   * @deprecated
   */

  public static void setCurrentWindow(MaxEditor theWindow){
    if (theWindow instanceof ErmesSketchWindow)
      itsSketchWindow = (ErmesSketchWindow)theWindow;
  }

  public static ErmesSketchWindow getCurrentWindow() {
    return itsSketchWindow;
  }
	
  /** Functions to add application hooks */

  public static void addHook(String name, String code)
  {
    if (itsHookTable == null)
      itsHookTable = new MaxWhenHookTable(); 
      
    itsHookTable.addHook(name, code);
  }

  /** Functions to run application hooks */

  public static void runHooks(String name)
  {
    if (itsHookTable == null)
      itsHookTable = new MaxWhenHookTable(); 
      
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
    try {
      jmaxProperties.put("jmaxVersion", MaxVersion.getMaxVersion());
    } catch (java.lang.NoClassDefFoundError e) {
      jmaxProperties.put("jmaxVersion", "version not available");
    }
    
    itsHookTable = new MaxWhenHookTable(); 

    ircam.jmax.utils.Platform.setValues();

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
	
	itsInterp.evalFile(getProperty("root") +
			   getProperty("file.separator") + "tcl" +
			   getProperty("file.separator") +  "jmaxboot.tcl");
      }
    catch (TclException e)
      {
	System.out.println("TCL error in initialization: " + itsInterp.getResult());
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

  /**
   * Quit verify if there is anything to save
   * Currently, this is done in the wrong way: we get the list
   * of windows from the MaxWindowManager, and for those that 
   * are MaxEditor, we do the check;
   * In reality, we should look in the MDA document data base
   * to do the work.
   */

  public static void Quit()
  {
    ListModel windows;


    boolean someOneNeedSave = false;
    boolean doTheSave = false;

    windows = MaxWindowManager.getWindowManager().getWindowList();

    // First, search if there is anything to save,

    for (int i = 0; i < windows.getSize(); i++)
      {
	Frame w = (Frame) windows.getElementAt(i);

	if (w instanceof MaxEditor)
	  if (((MaxEditor) w).ShouldSave())
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
	for (int i = 0; i < windows.getSize(); i++)
	  {
	    Frame w = (Frame) windows.getElementAt(i);

	    if (w instanceof MaxEditor)
	      ((MaxEditor) w).Close();
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












