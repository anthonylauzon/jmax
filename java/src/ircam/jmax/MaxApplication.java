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
  public static Clipboard systemClipboard = new Clipboard("system"); // ??

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

  static MaxWhenHookTable  itsHookTable = null;

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
    MaxVector toOpen = new MaxVector();

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

    for (int i = 0; i <= args.length; i++)
      {
	if (inOpt)
	  {
	    // Waiting for a value

	    if (i == args.length)
	      {
		// End of arguments

		jmaxProperties.put(option.substring(1), "true");
		inOpt  = false;
	      }
	    else if (args[i].startsWith("-"))
	      {
		// Got another option

		jmaxProperties.put(option.substring(1), "true");
		option = args[i];
		inOpt  = true;
	      }
	    else
	      {
		// Got a value

		jmaxProperties.put(option.substring(1), args[i]);

		inOpt  = false;
	      }
	  }
	else
	  {
	    // Waiting for a option

	    if (i == args.length)
	      {
		// do nothing
		inOpt  = false;
	      }
	    else if (args[i].startsWith("-"))
	      {
		// Got option

		option = args[i];
		inOpt  = true;
	      }
	    else
	      {
		// Got a value, i.e. a argument to open

		toOpen.addElement(args[i]);
		inOpt  = false;
	      }
	  }

      }


    // Default values
    if (jmaxProperties.get("root") == null)
      {
	//user didn't specify the root. Take the /usr/local/max default directory.

	jmaxProperties.put("root", "/usr/local/max");
      }

    //the version number as a system property
    try
      {
	jmaxProperties.put("jmaxVersion", MaxVersion.getMaxVersion());
      }
    catch (java.lang.NoClassDefFoundError e)
      {
	jmaxProperties.put("jmaxVersion", "version info not available");
      }

    // Get optional username and password

    Fts.setUserName((String) jmaxProperties.get("jmaxUserName"));
    Fts.setUserPassword((String) jmaxProperties.get("jmaxUserPassword"));

    Boolean noRealTime = new Boolean( (String)jmaxProperties.get( "jmaxNoRealTime"));
    Fts.setNoRealTime( noRealTime.booleanValue());

    itsHookTable = new MaxWhenHookTable(); 

    ircam.jmax.utils.Platform.setValues();

    // Create and initialize the tcl interpreter

    makeTclInterp(); 

    // Initialize all the submodules; first the kernel modules

    ircam.jmax.mda.MdaModule.initModule();
    ircam.jmax.fts.FtsModule.initModule();

    // Initialize dialogs

    ircam.jmax.dialogs.DialogsModule.initModule();

    // than the builtin editors 

    ircam.jmax.editors.console.ConsoleModule.initModule();
    ircam.jmax.editors.ermes.ErmesModule.initModule();
    ircam.jmax.editors.control.ControlModule.initModule();

    // Before booting the server, check if it is asked to run in real-time mode,
    // and if yes, inform the application layer
    
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
    
    if (Fts.getServer() == null)
      {
	System.err.println("No Fts Server Specified");
	return;
      }
    
    // Look if there are documents to open in the 
    // command line.
    
    for (int i = 0 ; i < toOpen.size(); i++)
      {
	MaxDocument document = null;
	File file = new File((String) toOpen.elementAt(i));
	
	try
	  {
	    document = Mda.loadDocument(file);
	  }
	catch (MaxDocumentException e)
	  {
	    System.out.println("Cannot load " + file);
	  }

	try
	  { 
	    if ((document != null) && document.getDocumentType().isEditable())
	      document.edit();
	  }
	catch (MaxDocumentException e)
	  {
	    // Ignore MaxDocumentException exception in running the editor
	    // May be an hack, may be is ok; move this stuff to an action
	    // handler !!
	  }
      }
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
	QuitDialog quitDialog = new QuitDialog(MaxWindowManager.getWindowManager().getTopFrame());

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

		if (YesOrNo.ask(MaxWindowManager.getWindowManager().getTopFrame(),
				"Save " + document.getName(), "Save", "Don't Save"))
		  {
		    if (! document.canSave())
		      {
			File file;
			file= MaxFileChooser.chooseFileToSave(null, document.getDocumentFile());

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
			new ErrorDialog(MaxWindowManager.getWindowManager().getTopFrame(),
					"Cannot Save " + document.getName());
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

    if (Fts.getServer() != null)
      Fts.getServer().stop();

    Runtime.getRuntime().exit(0);
  }
}












