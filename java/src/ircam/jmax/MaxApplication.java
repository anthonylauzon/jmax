//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax; 

import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;
import java.util.*;
import java.io.*;

import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.script.*;
import ircam.jmax.script.pkg.*;

// MaxApplication.getProperty should disappear, and properties stored in the system
// properties, that can *also* be loaded from a file.

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
  // The global server

  static Fts fts;

  public static void log(String s)
  {
      //
      // I added the log function to trace some bug on windows. Will
      // remove it as soon as I found the nasty bastard [peter]
      //
//        try {
//  	  int len = s.length();
//  	  RandomAccessFile file = new RandomAccessFile("C:\\jmax_log.txt", "rw");
//  	  file.seek(file.length());
//  	  for (int i = 0; i < len; i++) {
//  	      int c = s.charAt(i);
//  	      file.writeByte(c & 0x000000ff);	  
//  	  }
//  	  file.close();
//        } catch (Exception e) {
//        }
  }

  public static Fts getFts()
  {
    return fts;
  }

  public static void setFts(Fts s)
  {
    fts = s;
  }

  // public static Clipboard systemClipboard = new Clipboard("system"); 

  // (em 13-01-99)  in order to use the real system clipboard
  // instead of a local one, the following declaration would be sufficient:

  public static Clipboard systemClipboard = Toolkit.getDefaultToolkit().getSystemClipboard();

  // Unfortunately, the actual java implementation on Irix (3.1.1) is buggy,
  // and the X11 selection contained in the systemClipboard returns a null
  // array for the list of the MIME types (DataFlavors) supported, and
  // exits with an internal null exception on the isDataFlavorSupported() call


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

  /** Method to set system wide properties, stored in jmaxProperties */

  public static String setProperty(String key, String value)
  {
    return (String)jmaxProperties.put( key, value);
  }

  public static Properties getProperties()
  {
    return jmaxProperties;
  }

    /* The name of the class that implements the interpreter
     * interface. By default we load the Silk interpreter. This value
     * can be overriden with the -jmaxInterp <fullClassName> option */

    static private Interpreter itsInterp = null;

    public static Interpreter getInterpreter()
    {
	return itsInterp;
    }

    /** The package handler that keeps trace of the loaded package. */
  
    static private PackageHandler itsPackageHandler = null;

    public static PackageHandler getPackageHandler()
    {
	return itsPackageHandler;
    }

  static MaxWhenHookTable  itsHookTable = null;

  /** Functions to add application hooks */

  public static void addHook(String name, Script code)
  {
    if (itsHookTable == null)
      itsHookTable = new MaxWhenHookTable(); 
      
    itsHookTable.addHook(name, code);
  }

  /** Functions to run application hooks */

  public static boolean runHooks(String name)
  {
    if (itsHookTable == null)
      return false;
    else
      return itsHookTable.runHooks(name);
  }

  public static SplashDialog splash = null;

  /** His majesty the main method */

  public static void main(String args[]) 
  {
    MaxVector toOpen = new MaxVector();

    log("main 1\n");

    // main function parse the argument line and create the main class...
    //create a new default Properties object

    jmaxProperties = new Properties(System.getProperties());

    //start parsing arguments
    // don't check for valid options, so the user can set
    // command line arguments that can be accessed from tcl scripts
    // and we don't know yet
    // The parsing is done with a finite state automata; any option
    // that don't have a value (i.e. followed by nothing or another value)
    // get "true" as value, any value that is *not* a option value
    // is interpreted as a file name and opened by jMax.

    boolean inOpt = false;
    String option = null;

    log("main 2\n");


    for (int i = 0; i < args.length; i++)
      {
	if (inOpt)
	  {
	    // Waiting for an option's value

	    jmaxProperties.put(option.substring(1), args[i]);
	    inOpt  = false;
	  }
	else
	  {
	    // Waiting for a option

	    if (args[i].startsWith("-"))
	      {
		// Got option

		option = args[i];
		inOpt  = true;
	      }
	    else
	      {
		// Got an argument
		toOpen.addElement(args[i]);
	      }
	  }

      }

    log("main 3\n");


    // Default values
    if (jmaxProperties.get("jmaxRoot") == null)
      {
	//user didn't specify the root. Take the default directory.
	jmaxProperties.put("jmaxRoot", "/usr/lib/jmax");
      }

    log("main 4\n");


    //Splash Screen Dialog
    if((jmaxProperties.get("jmaxSplashScreen") == null)||(!jmaxProperties.get("jmaxSplashScreen").equals("hide")))
	splash = new SplashDialog(jmaxProperties.get("jmaxRoot")+"/images/Splash.gif", MaxVersion.getMaxVersion());  

    log("main 5\n");


    if (jmaxProperties.get("jmaxInterp") == null)
      {
	//user didn't specify the interp. Take default.
	jmaxProperties.put("jmaxInterp", "jacl");
	//jmaxProperties.put("jmaxInterp", "silk");
      }

    log("main 6\n");


    //the version number as a system property
    try
      {
	jmaxProperties.put("jmaxVersion", MaxVersion.getMaxVersion());
      }
    catch (java.lang.NoClassDefFoundError e)
      {
	jmaxProperties.put("jmaxVersion", "version info not available");
      }

    log("main 7\n");


    // Get optional username and password

    itsHookTable = new MaxWhenHookTable(); 

    ircam.jmax.Platform.setValues();

    // Create the package handler

    itsPackageHandler = new PackageHandler();

    log("main 8\n");


    // Create and initialize the tcl interpreter
    try
	{
	    bootstrapInterp(); 
	}
    catch (Exception e)
	{
	    e.printStackTrace(); // FIXME
	    System.out.println("Can't create the interpreter: " + e.getMessage());
	    return;
	}
    // Initialize all the submodules; first the kernel modules

    log("main 9\n");


    ircam.jmax.fts.FtsModule.initModule();

    log("main 10\n");


    // Initialize dialogs

    ircam.jmax.dialogs.DialogsModule.initModule();

    log("main 11\n");


    // then the builtin editors 
    ircam.jmax.editors.console.ConsoleModule.initModule();
    ircam.jmax.editors.patcher.ErmesModule.initModule(true);

    log("main 12\n");


    // Before booting the server, check if it is asked to run in real-time mode,
    // and if yes, inform the application layer
    try
      {
	itsInterp.boot(MaxApplication.getProperty("jmaxRoot"));
      }
    catch (ScriptException e)
      {
	System.out.println("Interpreter error in initialization: " + e.getMessage());
      }
    //##########################################################
    //##########################################################
    /*
      ///source $jmaxRootDir/tcl/icons.tcl
      String jmaxRoot = MaxApplication.getProperty("jmaxRoot");
      /// for declareSystemIcon cmd
      SystemIcons.loadIcon("_max_patcher_file_", jmaxRoot+"/images/mini_icon_pat.gif");
      /// for declareIcon cmd
      Icons.loadIcon("%jmax", jmaxRoot+"/images/jmax_logo_tiny.gif");

      ///if(systemProperty new == "true")source $jmaxRootDir/tcl/menu.tcl
      if(MaxApplication.getProperty("new").equals("true"))
      {
      AddPopUp.addAbbreviation("_object_", " ", "Adding New Object", true);
      AddPopUp.addAbbreviation("_message_box_", "messbox", "Adding New Message Box", true);
      AddPopUp.addAbbreviation("_inlet_", "inlet -1", "Adding New Inlet", false);
      }
      ///package provide jMax 2.0.2
      ///source .jmaxrc ?????????????????????????????
      //if {[file exists $jmaxRootDir/tcl/$jmaxHost.tcl]} then {
      //source $jmaxRootDir/tcl/$jmaxHost.tcl
      //}
      ///
      ///if {[file exists $jmaxRootDir/tcl/$jmaxHostType.tcl]} then {
      ///source $jmaxRootDir/tcl/$jmaxHostType.tcl
      ///} else {
      ///puts "Host Type $jmaxHostType do not exists"
      ///exit
      ///}
      ///source $jmaxRootDir/tcl/startup.tcl
      ftsconnect $jmaxServerDir $jmaxServerName $jmaxConnection $jmaxHost $jmaxServerOptions $jmaxPort

      sync

      package require system
      package require guiobj
      
      sourceFile $jmaxRootDir/tutorials/basics/project.env
      package require utils
      package require data
      package require control
      package require mess
      package require numeric
      package require math
      package require lists
      package require midi
      package require sequence
      package require signal
      package require ispw
      package require ispwmath
      package require qlist
      package require explode      
      package require io

      jmaxSetSampleRate $jmaxSampleRate
      jmaxSetAudioBuffer $jmaxAudioBuffer

      runHooks "platformStart"

      if {[runHooks "start"] != "true"} {
      runHooks "defaultStart"

    */
    //##########################################################
    //##########################################################

    log("main 13\n");

    ircam.jmax.editors.console.ConsoleWindow.init();
    ircam.jmax.editors.patcher.ErmesSketchWindow.touch(fts);
    //if there were no connection statements in startup.tcl, ask the user

    log("main 14\n");


    if (fts == null)
      {
	System.err.println("No Fts Server Specified");
	return;
      }

    // Look if there are documents to open in the 
    // command line.

    log("main 15\n");


    long startTime = 0;
    if (getProperty("jmaxLoadBenchmark") != null)
      startTime = System.currentTimeMillis();

    for (int i = 0 ; i < toOpen.size(); i++)
      {
	MaxDocument document = null;
	File file = new File((String) toOpen.elementAt(i));
	
	try
	  {
	    document = Mda.loadDocument(fts, file);
	  }
	catch (MaxDocumentException e)
	  {
	    System.out.println("Cannot load " + file);
	  }

	try
	  { 
	    if ((document != null) && document.getDocumentType().isEditable())
	      document.edit();

	    /* Special Support for load/unload benchmark  */

	    if (getProperty("jmaxLoadUnloadBenchmark") != null)
	      {
		final MaxDocument documentToKill = document;

		/* Use a invoke Later to close, so that we destroy 
		   the editor after the first paint completed, otherwise
		   the AWT will try to pain an inconsistent editor */
		
		SwingUtilities.invokeLater(new Runnable() {
		  public void run() {documentToKill.dispose();}});
	      }
	  }
	catch (MaxDocumentException e)
	  {
	    // Ignore MaxDocumentException exception in running the editor
	    // May be an hack, may be is ok; move this stuff to an action
	    // handler !!
	  }
      }


    log("main 16\n");


    /* Report the loading time if needed */


    if (getProperty("jmaxLoadBenchmark") != null)
      {
	long elapsedTime;

	elapsedTime = System.currentTimeMillis() - startTime;

	System.err.println("jMax version " + MaxVersion.getMaxVersion());
	System.err.println(" date " + new Date());
	System.err.println(" loading time " + elapsedTime + " msecs");
	System.err.println("Total memory " + Runtime.getRuntime().totalMemory());
	System.err.println("Used memory " + (Runtime.getRuntime().totalMemory() -
					     Runtime.getRuntime().freeMemory()));

      }


    log("main 17\n");

  }
  

  /** This private method build the interpreter. */

  static private void bootstrapInterp() throws Exception
  {
      String interpName = (String) jmaxProperties.get("jmaxInterp");
      String interpClassName = interpName;
      
      if (interpName.equalsIgnoreCase("jacl")) {
	  interpClassName = "ircam.jmax.script.tcl.TclInterpreter";
      } else if (interpName.equalsIgnoreCase("silk")) {
	  interpClassName = "ircam.jmax.script.scm.silk.SilkInterpreter";
      } else if (interpName.equalsIgnoreCase("kawa")) {
	  interpClassName = "ircam.jmax.script.scm.kawa.KawaInterpreter";
      }

      itsInterp = (Interpreter) Class.forName(interpClassName).newInstance();
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

    // First, search if there is anything to save
    // Loop in all the documents in all the  types.
    
    ListModel types = Mda.getDocumentTypes();

  search: for (int i = 0; i < types.getSize(); i++)
    {
      MaxDocumentType type = (MaxDocumentType) types.getElementAt(i);
      ListModel documents = type.getDocuments();

      for (int j = 0; j < documents.getSize(); j++)
	{
	  MaxDocument document = (MaxDocument) documents.getElementAt(j);
	
	  if (! document.isSaved())
	    {
	      someOneNeedSave = true;
	      break search;
	    }
	}
    }

    // in such case, should give the offer to cancel the quit.
    if (someOneNeedSave)
      {
	Object[] options = { "Quit", "Review unsaved and quit", "Cancel" };
	int result = JOptionPane.showOptionDialog(null, 
						  "There are unsaved documents; you really want to Quit ?", 
						  "Quit", 
						  JOptionPane.YES_NO_CANCEL_OPTION,
						  JOptionPane.QUESTION_MESSAGE,
						  null, options, options[0]);
	if(result == JOptionPane.YES_OPTION)
	    doTheSave = false;	
	if(result == JOptionPane.NO_OPTION)
	    doTheSave = true;	
	if(result == JOptionPane.CANCEL_OPTION)
	    return;
      }

    // dispose (and optionally save) all the documents

    for (int i = 0; i < types.getSize(); i++)
      {
	MaxDocumentType type = (MaxDocumentType) types.getElementAt(i);
	ListModel documents = type.getDocuments();

	for (int j = 0; j < documents.getSize(); j++)
	  {
	    MaxDocument document = (MaxDocument) documents.getElementAt(j);
	
	    if (doTheSave && (! document.isSaved()))
	      {
		  int result = JOptionPane.showConfirmDialog(MaxWindowManager.getWindowManager().getTopFrame(),
							     "Save "+document.getName()+" ?",
							     "Warning",
							     JOptionPane.YES_NO_OPTION,
							     JOptionPane.QUESTION_MESSAGE);  
		  if (result == JOptionPane.OK_OPTION)
		  {
		    if (! document.canSave())
		      {
			File file;
			file= MaxFileChooser.chooseFileToSave(null, document.getDocumentFile(), "Save As");

			if (file != null)
			  document.bindToDocumentFile(file);
		      }

		    if (document.canSave())
		      {
			try
			  {
			    document.save();
			  }
			catch (MaxDocumentException e)
			  {
			    System.err.println(e.toString());
			  }
		      }
		    else
		      { 
			  JOptionPane.showMessageDialog(MaxWindowManager.getWindowManager().getTopFrame(), 
							"Cannot Save "+document.getName(), 
							"Error", JOptionPane.ERROR_MESSAGE); 
		      }
		  }		    

		document.dispose();
	      }
	  }
      }

    // Now, the quit is sure, we execute the exit hooks

    runHooks("exit");

    if (MaxWindowManager.getTopFrame() != null)
      {
	MaxWindowManager.getTopFrame().setVisible(false);
	MaxWindowManager.getTopFrame().dispose();
      }

    if (fts != null)
      fts.stop();

    Runtime.getRuntime().exit(0);
  }

  private static boolean shown = false;
  public static void ftsQuitted()
  {
      if(!shown)
	  {
	      shown = true;
	      JOptionPane.showMessageDialog(null, "jMax server connection lost! \n Quit user interface.", 
					    "Fatal Error", JOptionPane.ERROR_MESSAGE); 
	      Runtime.getRuntime().exit(0);
	  }
  } 

    // Scripting interface

    // The theMaxApplication is an empty object since everything in
    // the MaxApplication class is static. It is only intended to
    // provide a Scriptable interface for the interpreter.

    static MaxApplication itsMaxApplication = new MaxApplication();

    /** Retreive application property */

    public String getSystemProperty(String name) 
    {
	return MaxApplication.getProperty(name);
    }

    /** Set application property */

    public String setSystemProperty(String name, String value)
    {
      return MaxApplication.setProperty(name, value);
    }

    /* Application "when" hooks */

    public void addSystemHook(String when, Script script) 
    {
	//MaxApplication.addHook(name, code);
    }

    // Methods to install new document handlers and types.

    /** Install a document handler in Mda */

    public void installDocumentHandler(MaxDocumentHandler handler)
    {
	Mda.installDocumentHandler(handler);
    }

    /** Install a document type in Mda */

    public void installDocumentType(MaxDocumentType type)
    {
	Mda.installDocumentType(type);
    }
}












