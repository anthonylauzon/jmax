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
import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;

import ircam.fts.client.*;
import ircam.jmax.editors.console.*;

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
 * - recent file history
 * - quit of the application
 */

public class MaxApplication extends Object
{
  // Recent file history

  static RecentFileHistory recentFileHistory = new RecentFileHistory(5);

  public static RecentFileHistory getRecentFileHistory()
  {
    return recentFileHistory;
  }

  // The global server

  static FtsServer server;
  public static FtsServer getServer()
  {
    return server;
  }

  public static Clipboard systemClipboard = Toolkit.getDefaultToolkit().getSystemClipboard();

    // Unfortunately, the actual java implementation on Irix (3.1.1) is buggy,
    // and the X11 selection contained in the systemClipboard returns a null
    // array for the list of the MIME types (DataFlavors) supported, and
    // exits with an internal null exception on the isDataFlavorSupported() call


    // (fd) changed because FtsServer uses MaxApplication.getProperty for timeOut
    // This is not clean because you cannot use the application layer without calling
    // MaxApplication.main !!! 
    // i.e. cannot write an application in Java that is not jMax...

  private static Properties jmaxProperties = new Properties(System.getProperties());

  public static String getProperty(String key)
  {
    return jmaxProperties.getProperty(key);
  }
  public static String getProperty(String key, String defaultValue)
  {
    return jmaxProperties.getProperty(key, defaultValue);
  }
  public static String setProperty(String key, String value)
  {
    return (String)jmaxProperties.put( key, value);
  }
  public static Properties getProperties()
  {
    return jmaxProperties;
  }

  public static SplashDialog splash = null;

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

    //Splash Screen Dialog
    if((jmaxProperties.get("jmaxSplashScreen") == null)||(!jmaxProperties.get("jmaxSplashScreen").equals("hide")))
      splash = new SplashDialog(jmaxProperties.get("jmaxRoot")+"/images/Splash.gif", JMaxVersion.getVersion());  
	
    //the version number as a system property
    try
      {
	jmaxProperties.put("jmaxVersion", JMaxVersion.getVersion());
      }
    catch (java.lang.NoClassDefFoundError e)
      {
	jmaxProperties.put("jmaxVersion", "version info not available");
      }

    ircam.jmax.Platform.setValues();

    // Initialize all the submodules;
    //ircam.jmax.fts.FtsModule.initModule();
    ircam.jmax.dialogs.DialogsModule.initModule();
    ircam.jmax.editors.console.ConsoleModule.initModule();
    ircam.jmax.editors.patcher.ErmesModule.initModule(true);

    DefaultIcons.init();

    new ConsoleWindow();

    ConsoleWindow.append( "jMax copyright (C) 1994, 1995, 1998, 1999 IRCAM - Centre Georges Pompidou");
    ConsoleWindow.append( "jMax is free software with ABSOLUTELY NO WARRANTY.");
    ConsoleWindow.append( "(see file LICENSE for more details)");	

    if (jmaxProperties.get("jmaxConnection") == null)
      {
	jmaxProperties.put("jmaxConnection", "pipe");
      }

    try
      {
	String[] argv = new String[10];
	int argc = 0;
	String connectionType = ((String)jmaxProperties.get("jmaxConnection"));
	String hostName = (String)jmaxProperties.get("jmaxHost");
	String hostType = (String)jmaxProperties.get("jmaxHostType");

	ConsoleWindow.append( "jMax starting server on "+hostName+" ("+hostType+") via "+connectionType+" connection"); 
		
	argv[argc++] = jmaxProperties.get("jmaxServerDir")+File.separator+"fts";	
	argv[argc++] = "--no-watchdog";
		
	if (connectionType.equals("pipe"))
	  argv[argc++] = "--stdio";
	    
	Object o = jmaxProperties.get("attach");

	FtsProcess fts = null;

	if (o != null && o instanceof Boolean && ((Boolean)o).booleanValue())
	  fts = new FtsProcess( argc, argv);
	else
	  System.out.println( "Attaching to FTS on host " + hostName);
		
	FtsServerConnection connection;
		
	if (connectionType.equals("pipe"))
	  {
	    if (fts == null)
	      {
		System.err.println( "Cannot attach to a pipe connection...");
		System.exit( 1);
	      }
	    connection = new FtsPipeConnection( fts);
	  }
	else //(connectionType.equals("socket"))
	  {
	    Integer port = (Integer)jmaxProperties.get("jmaxPort");
			
	    if(port == null && hostName==null)			
	      connection = new FtsSocketConnection();
	    else
	      if(port == null)
		connection = new FtsSocketConnection(hostName);
	      else
		connection = new FtsSocketConnection(hostName, port.intValue());
	  }

	server = new FtsServer( connection);
      }
    catch( Exception e)
      {
	e.printStackTrace();
      }

    // debug
    try
      {
	JMaxPackageLoader.load( "guiobj");
      }
    catch ( JMaxPackageLoadingException e)
      {
	System.err.println( "Cannot load package" + e);
      }

    /*
      jmaxSetSampleRate $jmaxSampleRate
      jmaxSetAudioBuffer $jmaxAudioBuffer
    */

    ircam.jmax.editors.console.ConsoleWindow.init();

    /* ircam.jmax.editors.patcher.ErmesSketchWindow.touch(fts);*/

    // Load recent file history
    // This should be really here, right before we eventually
    // open command line documents
    recentFileHistory.load();

    // Look if there are documents to open in the 
    // command line.

    /*for (int i = 0 ; i < toOpen.size(); i++)
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
      }
      catch (MaxDocumentException e)
      {
      // Ignore MaxDocumentException exception in running the editor
      // May be an hack, may be is ok; move this stuff to an action
      // handler !!
      }
      }*/
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
    
    /* WARNING:    */
    /*ListModel types = Mda.getDocumentTypes();

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
      }*/

    recentFileHistory.save();

    if (MaxWindowManager.getTopFrame() != null)
      {
	MaxWindowManager.getTopFrame().setVisible(false);
	MaxWindowManager.getTopFrame().dispose();
      }

    if(server != null)
      try{
	server.shutdown();
      }
      catch(FtsClientException e){}

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
}
