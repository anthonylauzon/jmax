//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
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

/*import javax.swing.*;*/
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.ListModel;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;

import ircam.fts.client.*;
import ircam.jmax.editors.console.*;
import ircam.jmax.editors.configuration.*;
import ircam.jmax.editors.patcher.*;

class FtsSystemOutConsole extends FtsObject {
  public FtsSystemOutConsole( FtsServer server, FtsPatcherObject rootPatcher) throws IOException
{
  super( server, rootPatcher, FtsSymbol.get("console_stream"));
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
}

class LoadPatcherMessageHandler implements FtsMessageHandler {
  public void invoke( FtsObject obj, FtsArgs args)
  {
    if ( args.isInt( 0) )
    {
      FtsPatcherObject patcher = new FtsPatcherObject( JMaxApplication.getFtsServer(), 
                                                       JMaxApplication.getRootPatcher(), 
                                                       args.getInt( 0), "jpatcher", null, 0, 0);
      ErmesSketchWindow win = new ErmesSketchWindow(patcher);
      String name =  args.getSymbol( 1).toString();
      win.setTitle( name);
      patcher.setEditorFrame( win);
      patcher.setName( name);
      patcher.setType( args.getInt( 2));
    }
  }
}

class ProjectMessageHandler implements FtsMessageHandler {
  public void invoke( FtsObject obj, FtsArgs args)
  {
    if ( args.isInt( 0) )
    {
      JMaxApplication.setCurrentProject(new FtsProject( JMaxApplication.getFtsServer(), 
                                                        JMaxApplication.getRootPatcher(),
                                                        args.getInt( 0)));
    }
  }
}

class ConfigMessageHandler implements FtsMessageHandler {
  public void invoke( FtsObject obj, FtsArgs args)
  {
    if ( args.isInt( 0) )
    {
      JMaxApplication.setConfig( new FtsConfig( JMaxApplication.getFtsServer(), 
                                                JMaxApplication.getRootPatcher(),
                                                args.getInt( 0)));
    }
  }
}

class PackageMessageHandler implements FtsMessageHandler {
  public void invoke( FtsObject obj, FtsArgs args)
  {
    if ( args.isInt( 0) )
      new FtsPackage( JMaxApplication.getFtsServer(), JMaxApplication.getRootPatcher(), args.getInt( 0));
  }
}

class RootPatcher extends FtsPatcherObject {
  RootPatcher( FtsServer server)
  {
    super( server, null, FtsObject.NO_ID, "jpatcher", null, 0, 0);
  }
  public void releaseObject( FtsObject obj)
  { 
    obj.dispose();
  }
}

class JMaxClient extends FtsObject {
  JMaxClient( FtsServer server, FtsPatcherObject rootPatcher)
  {
    super( server, rootPatcher, FtsObject.NO_ID);
  }
  
  void load( String name) throws IOException
  {
    FtsArgs args = new FtsArgs();
    
    args.addString( name);
    
    send( FtsSymbol.get("load"), args);
  }
  
  void loadProject( String name) throws IOException
  {
    FtsArgs args = new FtsArgs();
    
    args.addString( name);
    
    send( FtsSymbol.get("load_project"), args);
  }
  
  void loadPackage( String name, String fileName) throws IOException
  {
    FtsArgs args = new FtsArgs();
    
    args.addString( name);
    args.addString( fileName);
    
    send( FtsSymbol.get("load_package"), args);
  }
  
  void loadSummary( String name) throws IOException
  {
    FtsArgs args = new FtsArgs();
    
    args.addSymbol( FtsSymbol.get(name));
    
    send( FtsSymbol.get("load_summary"), args);
  }
  
  FtsArgs args = new FtsArgs();
  void registerObject(int nArgs , FtsAtom argums[])
  {            
    if(nArgs > 1 && argums[0].isInt() && argums[1].isSymbol())
    {
      int id = argums[0].intValue;
      String className = argums[1].symbolValue.toString();
      String description = argums[nArgs-1].symbolValue.toString();
      
      FtsObject obj = JMaxApplication.getFtsServer().getObject(id);
      if(obj == null)
      {
        args.clear();
        for(int i = 1; i<nArgs-1; i++)
          args.add(argums[i].getValue());
        
        obj = JMaxApplication.getObjectManager().makeFtsObject(id, className, args.getAtoms());
        if(obj != null)
          obj.setDescription(description);      
      }
    }  
  }  
  
  static
  {
    FtsObject.registerMessageHandler( JMaxClient.class, FtsSymbol.get( "patcher_loaded"), new LoadPatcherMessageHandler());
    FtsObject.registerMessageHandler( JMaxClient.class, FtsSymbol.get( "project"), new ProjectMessageHandler());
    FtsObject.registerMessageHandler( JMaxClient.class, FtsSymbol.get( "config"), new ConfigMessageHandler());
    FtsObject.registerMessageHandler( JMaxClient.class, FtsSymbol.get( "package"), new PackageMessageHandler());
    FtsObject.registerMessageHandler( JMaxClient.class, FtsSymbol.get( "showMessage"), new FtsMessageHandler() {
      public void invoke( FtsObject obj, FtsArgs args)
    {
        if ( args.isSymbol( 0) )
          JOptionPane.showMessageDialog( JMaxApplication.getConsoleWindow(), 
                                         args.getSymbol( 0).toString(), "Warning", JOptionPane.INFORMATION_MESSAGE);
    }
    });
    FtsObject.registerMessageHandler( JMaxClient.class, FtsSymbol.get("register_object"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
    {
        ((JMaxClient)obj).registerObject( args.getLength(), args.getAtoms());
    }
    });  
  }
}

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

public class JMaxApplication {
  
  // **********************************************************************
  // Still ugly
  // **********************************************************************
  
  /**
  * Quit verify if there is anything to save
   * Currently, this is done in the wrong way: we get the list
   * of windows from the MaxWindowManager, and for those that 
   * are MaxEditor, we do the check;
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
closeAllWindows( doTheSave);

//Look if current project needs to be saved
if( getProject().isDirty() && (getProject().getFileName() != null))
{
	Object[] options = { "Save", "Don't save"};
	int result = JOptionPane.showOptionDialog( getConsoleWindow(), 
                                             "Project File is not saved.\nDo you want to save it now?", 
                                             "Project Not Saved", 
                                             JOptionPane.YES_NO_CANCEL_OPTION,
                                             JOptionPane.QUESTION_MESSAGE,
                                             null, options, options[0]);
	if( result == JOptionPane.YES_OPTION)
	  getProject().save( getProject().getFileName());		  
}

//Look if current audio/midi configuration needs to be saved
if( getConfig().isDirty() && ( getConfig().getFileName() != null))
{
	Object[] options = { "Save", "Don't save"};
	int result = JOptionPane.showOptionDialog( getConsoleWindow(), 
                                             "Configuration File is not saved.\nDo you want to save it now?", 
                                             "Config Not Saved", 
                                             JOptionPane.YES_NO_CANCEL_OPTION,
                                             JOptionPane.QUESTION_MESSAGE,
                                             null, options, options[0]);
	if( result == JOptionPane.YES_OPTION)
	  getConfig().save( getConfig().getFileName());	
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
  singleInstance.clientObject.send( FtsSymbol.get( "shutdown"));
}
catch(IOException e)
{
  System.err.println("JMaxApplication: IOEception quitting application "+e);
}

if (!singleInstance.isAttached)
{
  FtsErrorStreamer.stopFtsErrorStreamer();
}
Runtime.getRuntime().exit(0);
  }

public static void closeAllWindows( boolean save)
{
  Frame win;
  FtsPatcherObject patcher;
  Vector patches = new Vector();
  Object[] windows = MaxWindowManager.getWindowManager().getWindowListArrayCopy();
  
  for (int i = 0; i < windows.length ; i++)
  {
    win = (Frame) windows[i];
    if( win instanceof ErmesSketchWindow)
	  {
	    patcher = ((ErmesSketchWindow)win).getSketchPad().getFtsPatcher();
      
	    if( patcher.isARootPatcher() )
      {
        if( save && patcher.isDirty())
        {		 
          if( patcher.canSave())
            patcher.save();
          else
		      {
            File file = MaxFileChooser.chooseFileToSave(null, null, "Save As", MaxFileChooser.JMAX_FILE_TYPE);
            
            if( file != null)
            {
              if( file.exists())
              {
                int result = JOptionPane.showConfirmDialog( win,
                                                            "File \"" + file.getName()+"\" exists.\nOK to overwrite ?",
                                                            "Warning",
                                                            JOptionPane.YES_NO_OPTION,
                                                            JOptionPane.WARNING_MESSAGE);
                
                if ( result == JOptionPane.OK_OPTION)
                  patcher.save( MaxFileChooser.getSaveType(), file.getAbsolutePath());
              }		    
              else
                patcher.save( MaxFileChooser.getSaveType(), file.getAbsolutePath());
            }
		      }
        }
        if( patcher.getName() != null)
          patches.add( patcher);
        
        patcher.stopUpdates();		      
        patcher.requestDestroyEditor();
        getRootPatcher().requestDeleteObject( patcher);
        ((ErmesSketchWindow)win).Destroy();
      }
	  }
  }
  /* save the open windows in the project */
  if( patches.size() > 0)
    getProject().saveWindows( patches.elements());
}

private static boolean shown = false;

public static void ftsQuitted()
{
  if(!shown)
  {
    shown = true;
    JOptionPane.showMessageDialog( getConsoleWindow(), 
                                   "jMax server connection lost! \n Quit user interface.", 
                                   "Fatal Error", JOptionPane.ERROR_MESSAGE); 
    
    FtsErrorStreamer.stopFtsErrorStreamer();
    Runtime.getRuntime().exit(0);
  }
} 

public static void load( String name) throws IOException
{
  singleInstance.clientObject.load( name);
}

public static void loadProject( String name) throws IOException
{
  singleInstance.clientObject.loadProject( name);
}

public static void loadPackage( String name, String fileName) throws IOException
{
  singleInstance.clientObject.loadPackage( name, fileName);
}

public static void loadSummary( String name) throws IOException
{
  singleInstance.clientObject.loadSummary( name);
}

// **********************************************************************


private static JMaxApplication singleInstance;

public static void main( String args[]) 
{
  singleInstance = new JMaxApplication();
  singleInstance.start( args);
}

// Static accessors
public static FtsPatcherObject getRootPatcher()
{
  return singleInstance.rootPatcher;
}

public static RecentFileHistory getRecentFileHistory()
{
  return singleInstance.recentFileHistory;
}

public static FtsServer getFtsServer()
{
  return singleInstance.server;
}

public static FtsProject getProject()
{
  return singleInstance.project;
}

public static void setCurrentProject( FtsProject proj)
{
  /* discard all classes and helpPatch from old project */	 
  JMaxClassMap.clear();
  FtsHelpPatchTable.clear();
  
  /* set the current project */
  singleInstance.project = proj;
}

public static FtsConfig getConfig()
{
  return singleInstance.config; 
}

public static void setConfig( FtsConfig c)
{
  singleInstance.config = c;
}

public static FtsDspControl getDspControl()
{
  return singleInstance.consoleWindow.getControlPanel().getDspControl();
}

public static ConsoleWindow getConsoleWindow()
{
  return singleInstance.consoleWindow;
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
  singleInstance._reportException( t);
}

public static JMaxObjectManager getObjectManager()
{
  return JMaxClassMap.getInstance();
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
  Platform.setValues();
  
  properties.put( "jmaxVersion", JMaxVersion.getVersion());
  
  parseCommandLineOptions( args);
  
  guessDirectories();
  
  showSplashScreen();
  
  initModules();
  
  openConsole();
  
  openConnection();
  
  createPredefinedObjects();
  
  initConsole();
  
  startReceive();
  
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

private void guessDirectories()
{
  if (properties.get( "jmaxRoot") == null)
  {
    URL url = ClassLoader.getSystemResource( "jmax.jar.root");

    String root, u;
    u = url.toString();
    
    if ( u.endsWith( "/Contents/Resources/Java/jmax.jar!/jmax.jar.root"))
    {
      // Mac OS X MRJAppBuilder and ProjectBuilder case
      root = u.substring( u.indexOf( '/'), u.lastIndexOf( "/Java/jmax.jar!/jmax.jar.root"));
      properties.put( "macosx", "true");
    }
    else
    {
      // Linux case, Mac OS X shell script case
      root = u.substring( u.indexOf( '/'), u.lastIndexOf( "/lib/jmax/java/jmax.jar!/jmax.jar.root"));
    }
    
    properties.put( "jmaxRoot", root + "/lib/jmax");
    if (properties.get( "jmaxServerDir") == null)
      properties.put( "jmaxServerDir", root + "/bin");
  }
}

private void showSplashScreen()
{
  if((properties.get("jmaxSplashScreen") == null)||(!properties.get("jmaxSplashScreen").equals("hide")))
    new SplashDialog(properties.get("jmaxRoot")+"/images/Splash.gif", JMaxVersion.getVersion());  
}

private void initModules()
{
  ircam.jmax.editors.patcher.ErmesSketchWindow.initModule();
  //     ircam.jmax.editors.configuration.ConfigurationEditor.registerConfigEditor();
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
    
    if(hostName == null)
      hostName = "localhost";
    
    System.out.println( "jMax starting server on "
                        + hostName + " via "+ connectionType + " connection"); 
    
    String ftsDir = (String)properties.get( "jmaxServerDir");
    String ftsName = (String)properties.get( "jmaxServerName");
    String ftsOpt  = (String)properties.get( "jmaxServerArg");
    
    if (ftsName == null)
      ftsName = "fts";
    
    argv[argc++] = ftsDir + "/" + ftsName;
    
    if((String)properties.get("macosx") != null)
      argv[argc++] = "--root="+((String)properties.get( "jmaxRoot"));
    
    if (connectionType.equals("pipe"))
      argv[argc++] = "--stdio";
    
    if(ftsOpt != null)
	argv[argc++] = ftsOpt;

    // Support for project and configuration file given in command line
    if ((String)properties.get("jmaxProject") != null)
      argv[argc++] = "--project="+((String)properties.get("jmaxProject"));
    
    if ((String)properties.get("jmaxConfig") != null)
      argv[argc++] = "--config="+((String)properties.get("jmaxConfig"));
    
    Object o = properties.get("attach");
    
    FtsProcess fts = null;
    
    /*if(true) DEBUG ATTACH*/
    if (o != null && ((String)o).equals( "true"))
	  {
	    System.out.println( "Attaching to FTS on host " + hostName);
	    killFtsOnQuit = true;
	    isAttached = true;
	  }	
    else
	  {                 
	    fts = new FtsProcess( argc, argv);
	    FtsErrorStreamer.startFtsErrorStreamer( fts.getErrorStream());
      
	    killFtsOnQuit = true;
	  }
    FtsServerConnection connection;
		
    /*if(false) DEBUG ATTACH*/
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
    
    server = new FtsServer( connection);
  }
  catch( Exception e)
  {
    JMaxApplication.reportException( e);
  }
}

private void createPredefinedObjects()
{
  rootPatcher = new RootPatcher( server);
  server.setRootObject( rootPatcher);
  clientObject = new JMaxClient( server, rootPatcher);
  server.setClientObject( clientObject);
}

private void initConsole()
{
  FtsObject ftsConsole;
  
  try
  {
    if (noConsole)
      ftsConsole = new FtsSystemOutConsole( server, rootPatcher);
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

private void startReceive()
{
  server.start();
  
  try
  {
    clientObject.send( FtsSymbol.get( "get_project"));
  }
  catch(IOException e)
  {
    JMaxApplication.reportException( e);
  }
  
  try
  {
    clientObject.send( FtsSymbol.get( "config"));
  }
  catch(IOException e)
  {
    JMaxApplication.reportException( e);
  }
}

private static void openCommandLineFiles()
{
  String fileName = null;
  try
  {	
    
    for(Enumeration e = singleInstance.toOpen.elements(); e.hasMoreElements(); )
	  {
	    fileName = (String)e.nextElement();
	    JMaxApplication.load(fileName);
	  }
  }
  catch(IOException e)
  {
    System.err.println("[JMaxApplication]: I/O error loading file "+fileName);
  }
}

private void _reportException( Throwable t)
{
  if (exceptionsPrintWriter == null)
  {
    exceptionsOutputStream = new ByteArrayOutputStream();
    exceptionsPrintWriter = new PrintWriter( exceptionsOutputStream);
  }
  
  exceptionsPrintWriter.println( "**************************************************");
  t.printStackTrace( exceptionsPrintWriter);
  exceptionsPrintWriter.flush();
  
  JTextArea textArea = new JTextArea( exceptionsOutputStream.toString());
  
  textArea.setEditable( false);
  JScrollPane scrollPane = new JScrollPane( textArea);
  
  Object[] message = new Object[2];
  message[0] = new JLabel( "Caught Java exception:");
  message[1] = scrollPane;
  
  String[] options = { "OK"};
  
  int ret = JOptionPane.showOptionDialog( null, message, "Java exception", JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, options, options[0]);
}

private RecentFileHistory recentFileHistory;
private FtsServer server;
private Properties properties;
private ConsoleWindow consoleWindow;
private MaxVector toOpen;
private boolean noConsole;
private boolean killFtsOnQuit;
private boolean isAttached;
private FtsPatcherObject rootPatcher;
private JMaxClient clientObject;
private FtsProject project;
private FtsConfig config;

private OutputStream exceptionsOutputStream;
private PrintWriter exceptionsPrintWriter;
}
