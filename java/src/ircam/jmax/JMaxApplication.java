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

package ircam.jmax; 

import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;
import java.util.*;
import java.io.*;
import java.net.*;

import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;

import ircam.fts.client.*;
import ircam.jmax.editors.console.*;
import ircam.jmax.editors.patcher.*;

// JMaxApplication.getProperty should disappear, and properties stored in the system
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

public class JMaxApplication extends FtsClient {
 
  // **********************************************************************
  // Still ugly
  // **********************************************************************

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
    boolean someOneNeedSave = false;
    boolean doTheSave = false;
    FtsPatcherObject patcher;
    
    // First, search if there is anything to save
    // Loop in all the documents in all the  types.
    
    ListModel windows = MaxWindowManager.getWindowManager().getWindowList();

  search: for (int i = 0; i < windows.getSize(); i++)
    {
      Frame win = (Frame) windows.getElementAt(i);
      if( win instanceof ErmesSketchWindow)
	{
	  patcher = ((ErmesSketchWindow)win).getSketchPad().getFtsPatcher();
	  if(patcher.isDirty())
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
	int result = JOptionPane.showOptionDialog( singleInstance.consoleWindow, 
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
    Frame win;
    for (int i = 0; i < windows.getSize(); i++)
      {
	win = (Frame) windows.getElementAt(i);
	if( win instanceof ErmesSketchWindow)
	  {
	    patcher = ((ErmesSketchWindow)win).getSketchPad().getFtsPatcher();
	    
	    if( patcher.isARootPatcher() )
	      {
		if( doTheSave && patcher.isDirty())
		  {		 
		    if( patcher.canSave())
		      patcher.save();
		    else
		      {
			File file = MaxFileChooser.chooseFileToSave(null, null, "Save As", MaxFileChooser.JMAX_FILE_TYPE);
		      
			if( file != null)
			  {
			    
			    int result = JOptionPane.showConfirmDialog( win,
									"File \"" + file.getName()+"\" exists.\nOK to overwrite ?",
									"Warning",
									JOptionPane.YES_NO_OPTION,
									JOptionPane.WARNING_MESSAGE);

			    if ( result == JOptionPane.OK_OPTION)
			      patcher.save( MaxFileChooser.getSaveType(), file.getAbsolutePath());
			  }		    
		      }
		  }
		patcher.stopUpdates();		      
		patcher.requestDestroyEditor();
		//FtsPatcherObject.fireAtomicAction(true); 
		try
		  {
		    patcher.delete();
		  }
		catch(IOException e){
		  System.err.println("[ErmesSketchPad]: IO error deleting patcher");
		}
		
		((ErmesSketchWindow)win).Destroy();
	      }
	  }
      }
    /////////////////////////////////////////////////////////////////////////

    singleInstance.recentFileHistory.save();

    if (MaxWindowManager.getTopFrame() != null)
      {
	MaxWindowManager.getTopFrame().setVisible(false);
	MaxWindowManager.getTopFrame().dispose();
      }

    if(singleInstance.killFtsOnQuit)
      if ( singleInstance.server != null)
	try{
	  singleInstance.server.shutdown();
	}
	catch(FtsClientException e){}
	catch(IOException e)
	  {
	    System.err.println("JMaxApplication: IOEception quitting application "+e);
	  }
    
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
  // **********************************************************************


  private static JMaxApplication singleInstance;

  public static void main( String args[]) 
  {
    singleInstance = new JMaxApplication();
    singleInstance.start( args);
  }

  // Static accessors
  public static RecentFileHistory getRecentFileHistory()
  {
    return singleInstance.recentFileHistory;
  }

  public static FtsServer getFtsServer()
  {
    return singleInstance.server;
  }

  public static FtsDspControl getDspControl()
  {
    return singleInstance.consoleWindow.getControlPanel().getDspControl();
  }

  public static Clipboard getSystemClipboard()
  {
    return Toolkit.getDefaultToolkit().getSystemClipboard();
  }

  public static String getProperty(String key)
  {
    return singleInstance.properties.getProperty(key);
  }

  public static String getProperty(String key, String defaultValue)
  {
    return singleInstance.properties.getProperty(key, defaultValue);
  }

  public static void reportException( Throwable t)
  {
    ByteArrayOutputStream bs = new ByteArrayOutputStream();
    PrintWriter pw = new PrintWriter( bs);

    t.printStackTrace( pw);
    pw.flush();

    JTextArea textArea = new JTextArea( bs.toString());

    textArea.setEditable( false);
    JScrollPane scrollPane = new JScrollPane( textArea);
                
    Object[] message = new Object[2];
    message[0] = new JLabel( "Caught Java exception:");
    message[1] = scrollPane;

    String[] options = { "OK"};

    int ret = JOptionPane.showOptionDialog( null, message, "Java exception", JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, options, options[0]);
  }

  private JMaxApplication()
  {
    singleInstance = this;
    recentFileHistory = new RecentFileHistory(5);
    properties = new Properties( System.getProperties());
    toOpen = new MaxVector();
  }

  private void start( String[] args)
  {
    parseCommandLineOptions( args);
    findRootDirectory();
    showSplashScreen();
    properties.put( "jmaxVersion", JMaxVersion.getVersion());
    ircam.jmax.Platform.setValues();
    initModules();
    openConsole();
    openConnection();
    /*initConsole();*/

    // This should be really here, right before we eventually open command line documents
    recentFileHistory.load();

    openCommandLineFiles();
  }


  private void parseCommandLineOptions( String[] args)
  {
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
			
	    properties.put( option.substring(1), args[i]);
	    inOpt  = false;
	  }
	else
	  {
	    // Waiting for a option
	    if (args[i].startsWith("-"))
	      {
		option = args[i];
		inOpt  = true;
	      }
	    else
	      {
		toOpen.addElement(args[i]);
	      }
	  }
      }
  }

  private void findRootDirectory()
  {
    if (properties.get( "jmaxRoot") != null)
      return;

    URL url = ClassLoader.getSystemResource( "jmax.jar.root");
    String u = url.toString();

    String root = u.substring( u.indexOf( '/'), u.lastIndexOf( "/Java/jmax.jar!/jmax.jar.root")) + "/jmaxRoot";

    properties.put( "jmaxRoot", root);
  }

  private void showSplashScreen()
  {
    if((properties.get("jmaxSplashScreen") == null)||(!properties.get("jmaxSplashScreen").equals("hide")))
      new SplashDialog(properties.get("jmaxRoot")+"/images/Splash.gif", JMaxVersion.getVersion());  
  }

  private void initModules()
  {
    ircam.jmax.editors.patcher.ErmesModule.initModule();
  }

  private void openConsole()
  {
    consoleWindow = new ConsoleWindow( "jMax Console");

    noConsole = true;
    if ( (JMaxApplication.getProperty("jmaxNoConsole") == null) || 
	 (JMaxApplication.getProperty("jmaxNoConsole").equals("false")))
      {
	System.setOut( new PrintStream( new ConsoleOutputStream( consoleWindow.getConsoleArea())));
	noConsole = false;
      }
    else
      consoleWindow.getConsoleArea().append( "Output redirected to Java standard output");
    
    MaxWindowManager.getWindowManager().addToolFinder( new MaxToolFinder() {
	public String getToolName() { return "Console";}
	public void open() { consoleWindow.toFront();}
      });
  }

  private class LoadPackageHandler implements FtsMessageHandler {
    public void invoke( FtsObject obj, FtsArgs args)
    {
      if ( args.isSymbol( 0) )
	{
	  System.out.println( "package: " + args.getSymbol(0));

	  try
	    {
	      JMaxPackageLoader.load( args.getSymbol( 0).toString());
	    }
	  catch( JMaxPackageLoadingException e)
	    {
	      JMaxApplication.reportException( e);
	    }

	  for(int i = 1; i<args.getLength(); i+=2)
	    FtsHelpPatchTable.addSummary( args.getSymbol( i).toString(), args.getSymbol( i+1).toString());
	}
    }
  }

  private void openConnection()
  {
    if (properties.get("jmaxConnection") == null)
      properties.put("jmaxConnection", "pipe");

    try
      {
	String[] argv = new String[10];
	int argc = 0;
	String connectionType = ((String)properties.get("jmaxConnection"));
	String hostName = (String)properties.get("jmaxHost");

	System.out.println( "jMax starting server on "
			    + ((hostName == null) ? "localhost" : hostName)
			    + " via "+ connectionType + " connection"); 


	String ftsDir = (String)properties.get( "jmaxServerDir");

	if (ftsDir == null)
	  ftsDir = (String)properties.get( "jmaxRoot") + "/../bin";

	String ftsName = (String)properties.get( "jmaxServerName");

	if (ftsName == null)
	  ftsName = "fts";

	argv[argc++] = ftsDir + "/" + ftsName;
		
	if (connectionType.equals("pipe"))
	  argv[argc++] = "--stdio";
	    
	Object o = properties.get("attach");

	FtsProcess fts = null;

	if (o != null && ((String)o).equals( "true"))
	  {
	    System.out.println( "Attaching to FTS on host " + hostName);
	    killFtsOnQuit = false;
	  }	
	else
	  {
	    fts = new FtsProcess( argc, argv);
	    killFtsOnQuit = true;
	  }
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
	    Integer port = (Integer)properties.get("jmaxPort");
			
	    if(port == null && hostName==null)			
	      connection = new FtsSocketConnection();
	    else
	      if(port == null)
		connection = new FtsSocketConnection(hostName);
	      else
		connection = new FtsSocketConnection(hostName, port.intValue());
	  }

	server = new FtsServer( connection, this);

	FtsObject.registerMessageHandler( JMaxApplication.class, FtsSymbol.get( "package_loaded"), new LoadPackageHandler());
	FtsObject.registerMessageHandler( JMaxApplication.class, FtsSymbol.get( "patcher_loaded"), new FtsMessageHandler(){
	    public void invoke( FtsObject obj, FtsArgs args)
	    {
	      if ( args.isInt( 0) )
		{
		  FtsPatcherObject patcher = new FtsPatcherObject( JMaxApplication.getFtsServer(), 
								   JMaxApplication.getFtsServer().getRoot(), 
								   args.getInt( 0), "jpatcher", null, 0, 0);
		  ErmesSketchWindow win = new ErmesSketchWindow(patcher);
		  String name =  args.getSymbol( 1).toString();
		  win.setTitle( name);
		  patcher.setEditorFrame( win);
		  patcher.setName( name);
		  patcher.setType( args.getInt( 2));
		}
	    }
	  });

	
	initConsole();

	send( FtsSymbol.get( "get_packages"));
      }
    catch( Exception e)
      {
	JMaxApplication.reportException( e);
      }
  }

  class FtsSystemOutConsole extends FtsObject {
    public FtsSystemOutConsole() throws IOException
    {
      super( server, server.getRoot(), FtsSymbol.get("console_stream"));
    }
  }

  static
  {
    FtsObject.registerMessageHandler( FtsSystemOutConsole.class, FtsSymbol.get("print_line"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  System.out.println( args.getString(0)); 
	}
      });
  }
    

  private void initConsole()
  {
    FtsObject ftsConsole;

    try
      {
	if (noConsole)
	  ftsConsole = new FtsSystemOutConsole();
	else
	  ftsConsole = new FtsConsole( consoleWindow.getConsoleArea());

	ftsConsole.send( FtsSymbol.get("set_default"));
      }
    catch(IOException e)
      {
	System.err.println("ConsoleWindow: Error in FtsConsole creation!");
	e.printStackTrace();
      }

    consoleWindow.getControlPanel().init();
  }


  private static void openCommandLineFiles()
  {
    String fileName = null;
    try
      {	
	
	for(Enumeration e = singleInstance.toOpen.elements(); e.hasMoreElements(); )
	  {
	    fileName = (String)e.nextElement();
	    JMaxApplication.getFtsServer().getRoot().load(fileName);
	  }
      }
    catch(IOException e)
      {
	System.err.println("[JMaxApplication]: I/O error loading file "+fileName);
      }
  }

  private RecentFileHistory recentFileHistory;
  private FtsServer server;
  private Properties properties;
  private ConsoleWindow consoleWindow;
  private MaxVector toOpen;
  private boolean noConsole;
  private boolean killFtsOnQuit;
}
