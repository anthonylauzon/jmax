package ircam.jmax; 

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.editors.console.*;
import ircam.jmax.editors.ermes.*;
import ircam.jmax.editors.project.*;
import tcl.lang.*;

/**
 * The main application class in Ermes. Contains the global parameters 
 * and/or the global functionalities of the system. 
 * It is also the entry point for ermes TCL commands. It handles, for example:
 * - the startup process, the TCL initialization
 * - the FTS connection
 * - the resource definition loading
 * - the project manager activation
 * - a set of global functions related to the window list (stack, tile, close)
 * - file format versions handling
 * - system properties
 * - creation of new windows (new, double-click on a subpatcher, etc.)
 * - quit of the application
 */
public class MaxApplication extends Object {

  // Static global services

  static MaxApplication theApplication;

  /** Get the unique instance of getApplication */

  public static MaxApplication getApplication()
  {
    return theApplication;
  }

  /** Get the unique active FTS Server, if any */

  public static FtsServer getFtsServer()
  {
    return theApplication.itsServer;
  }

  /** Get the unique active TCL interpreter */

  public static Interp getTclInterp()
  {
    return itsInterp;
  }

  static Interp itsInterp;//e.m.
  public static Vector itsSketchWindowList;
  public static Vector itsEditorsFrameList;
  ConnectionDialog itsConnDialog;
  String	fileToLoad;
  String	pathForLoading;
  public FtsServer     itsServer = null;
  public boolean doAutorouting = true; // Should become a static in the Patcher editor
  //e.m.public static ConsShell itsShell;
  public static Properties ermesProperties;

  public static PrintStream itsPrintStream;
  public Vector resourceVector = new Vector();
  int MAX_RESOURCE_FILE_LENGHT = 1024;
  public ErmesSketchWindow itsSketchWindow;
  public MaxWindow itsWindow;
  public /*static*/ ProjectWindow itsProjectWindow;

  static MaxWhenHookTable  itsHookTable;
  public final static int NEW_COMMAND = 0;
  public final static int REQUIRE_CONNECTION = 1;
  public final static int STACK_WINDOWS = 2;
  public final static int TILE_WINDOWS = 3;
  public final static int TILEVERTICAL_WINDOWS = 4;
  public final static int SNAP_TO_GRID = 5;
  public final static int NEW_PROJECT = 6;
  public final static int CLOSE_WINDOW = 7;
  public final static int QUIT_APPLICATION = 8;
  public final static int OPEN_COMMAND = 9;
  public final static int SAVE_COMMAND = 10;
  public final static int SAVEAS_COMMAND = 11;
  public final static int CONNECTION_CHOOSEN = 12;
  public final static int ADD_WINDOW = 13;
  public final static int ADD_FILES = 14;
  public final static int REMOVE_FILES = 15;
  public final static int AUTO_ROUTING = 16;
  public final static int OPEN_WITH_AUTO_ROUTING = 17;
  public final static int SELECT_ALL = 18;
  public final static int PRINT_WINDOW = 19;

  ConsoleWindow itsConsoleWindow = null;
  Dimension d = new Dimension(java.awt.Toolkit.getDefaultToolkit().getScreenSize());
  final int SCREENVERT = d.height;
  final int SCREENHOR = d.width;

  public MaxApplication() {

    theApplication = this;

    itsHookTable = new MaxWhenHookTable(); 

    ircam.jmax.utils.Platform.setValues();
    itsSketchWindowList = new Vector();
    itsEditorsFrameList = new Vector();

    // The console creation moved in a tcl command !!!

    LoadResources();

    ObeyCommand(NEW_PROJECT);

    itsInterp = new tcl.lang.Interp(); // should go away here !!!

    makeTclInterp(); 

    // Splash screen moved to a tcl command

    //if there were no connection statements in startup.tcl, ask the user
    if (itsServer == null) ObeyCommand(REQUIRE_CONNECTION);	// maybe to be moved in ConnectionDialog
  }
	

  void LoadResources() {
    byte buffer[] = new byte[1024];
    String aTempString;
    
    FileInputStream fis;
    int nOfBytes;
    StringTokenizer aST;
    MaxResourceId aResId;
    
    String pathForResources = ermesProperties.getProperty("root")+ermesProperties.getProperty("file.separator")+"config"+ermesProperties.getProperty("file.separator")+"resources.erm";
    try {
      fis = new FileInputStream(pathForResources);
    }
    catch(FileNotFoundException e) {
      itsPrintStream.println("ERROR: can't find resource configuration file in "+pathForResources);
      return;
    }
    try {
      nOfBytes = fis.read(buffer, 0, MAX_RESOURCE_FILE_LENGHT);
    }
    catch(IOException e) {
      itsPrintStream.println("ERROR: can't read resource configuration file");
      return;
    }
    aST = new StringTokenizer(new String(buffer, 0));
    if (!Start_resource_type_list(aST)) {
      itsPrintStream.println("wrong resources.erm file");
      return;
    }

		
    while (aST.hasMoreTokens() ) {
      aTempString = aST.nextToken();
      if (aTempString.equals("end_resource_type_list"))
	break;
      else if (aTempString.equals("***new_resource")){
	aResId = Resource(aST);
	if (aResId == null) {
	  itsPrintStream.println("failed to successfull parse resource");
	  break;
	}
	else resourceVector.addElement(aResId); 
      } else {
	itsPrintStream.println("wrong resources.erm file");
	return;
      }
    }
		

    
    /*for(Enumeration e = resourceVector.elements(); e.hasMoreElements();) {
      aResId = (MaxResourceId) e.nextElement();
      itsPrintWrite.println("RESOURCE TYPE "+aResId.resourceName);
      itsPrintStream.println("associated extensions: ");
      for (Enumeration e1 = aResId.resourceExtensions.elements(); e1.hasMoreElements();) {
      itsPrintStream.println("\""+e1.nextElement()+"\"");
      }
      itsPrintStream.println("associated handler: "+aResId.preferred_resource_handler);
      }*/
  }
  
  boolean Start_resource_type_list(StringTokenizer aST) {
    if (aST.hasMoreTokens())
      return(aST.nextToken().equals("start_resource_type_list"));
    else return false;
  }

	
  boolean New_resource(StringTokenizer aST) {
    if (aST.hasMoreTokens())
      return(aST.nextToken().equals("***new_resource"));
    else return false;
  }
	
  MaxResourceId Resource(StringTokenizer aST) {
    String aTempString;
    if (!aST.hasMoreTokens()) return null;
    MaxResourceId aResId = new MaxResourceId(aST.nextToken(" \t\n\r\""));
    if (!aST.nextToken().equals("resource_extension_list")) return null;
    if (!aST.hasMoreTokens()) return null;
    
    while (aST.hasMoreTokens() ) {
      aTempString = aST.nextToken();
      if (aTempString.equals("end_resource_extension_list"))
	break;
      else aResId.resourceExtensions.addElement(aTempString);
    }
    if (!aST.hasMoreTokens()) return null;
    if (!aST.nextToken().equals("preferred_resource_handler")) return null;
    if (!aST.hasMoreTokens()) return null;
    aResId.preferred_resource_handler = aST.nextToken();
    // a more clever implementation would now SKIP the tokens until "end_resource", 
    // that must be present anyway.
    // This would allow future extensions of the file format
    if (!aST.hasMoreTokens()) return null;
    if (!aST.nextToken().equals("end_resource")) return null;
    if (!aST.hasMoreTokens()) return null;
    if (!aST.nextToken().equals(aResId.resourceName)) return null;
    return aResId;	
  }

  public boolean FTSConnect(String theFtsdir, String theFtsname, String mode, String server, String port)
  {
    if (mode.equals("socket")) 
      itsServer = new FtsSocketServer(server, Integer.parseInt(port));
    else if (mode.equals("client"))
      itsServer = new FtsSocketClientServer(server);
    else if (mode.equals("local"))
      itsServer = new FtsSubProcessServer();

    itsServer.setPostStream(itsPrintStream);
    itsServer.setParameter("ftsdir", theFtsdir);
    itsServer.setParameter("ftsname", theFtsname);
    itsServer.start();

    // don't run the "start" hooks here, we do it from TCL
    // to make sure all the config and lib loading is done.

    return itsServer != null;
  }
  
  public boolean Load(String file, String path) {
    //for now, only '.pat' accepted...
    // so, if fileToLoad does not terminate with '.pat', error
    fileToLoad = file;
    pathForLoading = path;
    return ObeyCommand(OPEN_COMMAND);
  }
  
  public boolean AddToProject(String file, String path){
    fileToLoad = file;
    pathForLoading = path;
    return ObeyCommand(ADD_FILES);
  }
	
  public void AddThisWindowToMenus(ErmesSketchWindow theSketchWindow){
    ErmesSketchWindow aSketchWindow;
    MaxWindow aWindow;
    if(!theSketchWindow.isSubPatcher){
      for(int i=0;i<itsSketchWindowList.size();i++){
	aSketchWindow = (ErmesSketchWindow)itsSketchWindowList.elementAt(i);
	if(aSketchWindow != theSketchWindow) 
	  aSketchWindow.AddWindowToMenu(theSketchWindow.getTitle());
      }
      for(int i=0;i<itsEditorsFrameList.size();i++){
	aWindow = (MaxWindow)itsEditorsFrameList.elementAt(i);
	aWindow.AddWindowToMenu(theSketchWindow.getTitle());
      }
      itsProjectWindow.AddWindowToMenu(theSketchWindow.getTitle());

      if (itsConsoleWindow != null)
	itsConsoleWindow.AddWindowToMenu(theSketchWindow.getTitle());
    }
  }

  public void AddThisFrameToMenus(String theName){
    ErmesSketchWindow aSketchWindow;
    MaxWindow aWindow;
    for(int i=0;i<itsSketchWindowList.size();i++){
      aSketchWindow = (ErmesSketchWindow)itsSketchWindowList.elementAt(i);
      aSketchWindow.AddWindowToMenu(theName);
    }
    for(int i=0;i<itsEditorsFrameList.size();i++){
      aWindow = (MaxWindow)itsEditorsFrameList.elementAt(i);
	if(!theName.equals(aWindow.GetDocument().GetName()))
	  aWindow.AddWindowToMenu(theName);
    }
    itsProjectWindow.AddWindowToMenu(theName);

    if (itsConsoleWindow != null)
      itsConsoleWindow.AddWindowToMenu(theName);
  }

  public void AddToSubWindowsList(ErmesSketchWindow theTopWindow,ErmesSketchWindow theSubWindow, boolean theFirstItem){
    ErmesSketchWindow aSketchWindow;
    MaxWindow aWindow;
    for(int i=0;i<itsSketchWindowList.size();i++){
      aSketchWindow = (ErmesSketchWindow)itsSketchWindowList.elementAt(i);
      if(aSketchWindow != theTopWindow) 
	aSketchWindow.AddToSubWindowsMenu(theTopWindow.getTitle(), theSubWindow.getTitle(), theFirstItem);
    }
    for(int i=0;i<itsEditorsFrameList.size();i++){
      aWindow = (MaxWindow)itsEditorsFrameList.elementAt(i);
      aWindow.AddToSubWindowsMenu(theTopWindow.getTitle(), theSubWindow.getTitle(), theFirstItem);
    }
    itsProjectWindow.AddToSubWindowsMenu(theTopWindow.getTitle(), theSubWindow.getTitle(), theFirstItem);

    if (itsConsoleWindow != null)
      itsConsoleWindow.AddToSubWindowsMenu(theTopWindow.getTitle(), theSubWindow.getTitle(), theFirstItem);
  }

   public void RemoveFromSubWindowsList(ErmesSketchWindow theTopWindow,ErmesSketchWindow theSubWindow, boolean theLastItem){
    ErmesSketchWindow aSketchWindow;
    MaxWindow aWindow;
    for(int i=0;i<itsSketchWindowList.size();i++){
      aSketchWindow = (ErmesSketchWindow)itsSketchWindowList.elementAt(i);
      if(aSketchWindow != theTopWindow) 
	aSketchWindow.RemoveFromSubWindowsMenu(theTopWindow.getTitle(),theSubWindow.getTitle(),theLastItem);
    }
    for(int i=0;i<itsEditorsFrameList.size();i++){
      aWindow = (MaxWindow)itsEditorsFrameList.elementAt(i);
      aWindow.RemoveFromSubWindowsMenu(theTopWindow.getTitle(), theSubWindow.getTitle(), theLastItem);
    }
    itsProjectWindow.RemoveFromSubWindowsMenu(theTopWindow.getTitle(), theSubWindow.getTitle(),theLastItem);
    if (itsConsoleWindow != null)
      itsConsoleWindow.RemoveFromSubWindowsMenu(theTopWindow.getTitle(), theSubWindow.getTitle(), theLastItem);

    itsSketchWindowList.removeElement(theSubWindow);
  }


  public void RemoveThisWindowFromMenus(MaxWindow theWindow){
    ErmesSketchWindow aSketchWindow;
    MaxWindow aWindow;
    for(int i=0;i<itsSketchWindowList.size();i++){
      aSketchWindow = (ErmesSketchWindow)itsSketchWindowList.elementAt(i);
      if(aSketchWindow != theWindow) 
	aSketchWindow.RemoveWindowFromMenu(theWindow.GetDocument().GetName());
    }
    for(int i=0;i<itsEditorsFrameList.size();i++){
      aWindow = (MaxWindow)itsEditorsFrameList.elementAt(i);
      if(aWindow != theWindow)
	aWindow.RemoveWindowFromMenu(theWindow.GetDocument().GetName());
    }
    itsProjectWindow.RemoveWindowFromMenu(theWindow.GetDocument().GetName());

    if (itsConsoleWindow != null)
      itsConsoleWindow.RemoveWindowFromMenu(theWindow.GetDocument().GetName());
  }
  
  public void ChangeWinNameMenus(String theOldName, String theNewName){
    ErmesSketchWindow aSketchWindow;
    MaxWindow aWindow;
    for(int i=0;i<itsSketchWindowList.size();i++){
      aSketchWindow = (ErmesSketchWindow)itsSketchWindowList.elementAt(i); 
      aSketchWindow.ChangeWinNameMenu(theOldName,theNewName);
    }
    for(int i=0;i<itsEditorsFrameList.size();i++){
      aWindow = (MaxWindow)itsEditorsFrameList.elementAt(i);
      aWindow.ChangeWinNameMenu(theOldName,theNewName);
    }
    itsProjectWindow.ChangeWinNameMenu(theOldName, theNewName);

    if (itsConsoleWindow != null)
      itsConsoleWindow.ChangeWinNameMenu(theOldName, theNewName);
  }
  

  public ErmesSketchWindow NewPatcherWindow(FtsObject theFtsPatcher) {
    ErmesPatcherDoc aPatcherDoc = new ErmesPatcherDoc(theFtsPatcher);
    aPatcherDoc.alreadySaved = true;
    itsSketchWindow = new ErmesSketchWindow(false, itsSketchWindow);
    theFtsPatcher.open();
    itsSketchWindow.repaint();
    itsWindow = itsSketchWindow;
    ErmesSketchWindow aSketch = (ErmesSketchWindow)itsSketchWindow;
    itsSketchWindow.InitFromDocument(aPatcherDoc);
    itsSketchWindow.inAnApplet = false;
    itsSketchWindow.setTitle(itsSketchWindow.itsDocument.GetName());
    aPatcherDoc.SetWindow(itsSketchWindow);
    if(itsProjectWindow.itsProject.GetItems().size()==0)
      itsSketchWindow.getMenuBar().getMenu(2).getItem(2).disable();
    if(itsSketchWindowList.size() == 1)
      itsProjectWindow.getMenuBar().getMenu(2).getItem(0).enable();
    itsSketchWindow.show();
    return itsSketchWindow;
  }


  public ErmesSketchWindow NewSubPatcherWindow(FtsObject theFtsPatcher) {
    ErmesPatcherDoc aPatcherDoc = new ErmesPatcherDoc(theFtsPatcher);
    aPatcherDoc.alreadySaved = true;
    boolean temp = itsSketchWindow.itsSketchPad.doAutorouting;
    itsSketchWindow = new ErmesSketchWindow(true, itsSketchWindow);
    itsSketchWindow.itsSketchPad.doAutorouting = temp;
    theFtsPatcher.open();
    itsSketchWindow.repaint();
    itsWindow = itsSketchWindow;
    //ErmesSketchWindow aSketch = (ErmesSketchWindow)itsSketchWindow;
    itsSketchWindow.InitFromDocument(aPatcherDoc);
    itsSketchWindowList.addElement(itsSketchWindow);
    itsSketchWindow.inAnApplet = false;
    itsSketchWindow.setTitle(itsSketchWindow.GetDocument().GetName());
    aPatcherDoc.SetWindow(itsSketchWindow);
    CheckboxMenuItem aEditMenuItem = (CheckboxMenuItem)itsSketchWindow.itsEditMenu.getItem(9);
    aEditMenuItem.setState(temp);
    if(itsProjectWindow.itsProject.GetItems().size()==0)
      itsSketchWindow.getMenuBar().getMenu(2).getItem(2).disable();
    if(itsSketchWindowList.size() == 1)
      itsProjectWindow.getMenuBar().getMenu(2).getItem(0).enable();
    itsSketchWindow.pack();
    itsSketchWindow.show();
    return itsSketchWindow;
  }
  
  public ErmesSketchWindow NewDefaultSubPatcher( FtsObject theFtsPatcher) {//to use just for 'patcher' externals
    theFtsPatcher.setWindowDescription(new FtsWindowDescription(100, 100, 300, 300));
    ErmesSketchWindow aSketchWindow = NewSubPatcherWindow(theFtsPatcher);
    aSketchWindow.isSubPatcher = true;
    return aSketchWindow;
  }
  
  public boolean ObeyCommand(int command) {
    ErmesSketchWindow aSketchWindow;
    switch (command) {
    case NEW_COMMAND:

      itsSketchWindow = new ErmesSketchWindow(false, null);
      itsWindow = itsSketchWindow;//???????

      itsSketchWindowList.addElement(itsSketchWindow);
      itsSketchWindow.inAnApplet = false;
      itsSketchWindow.itsDocument.itsPatcher.open();	//remember to close
      
      itsSketchWindow.setTitle(itsSketchWindow.itsDocument.GetName());
      itsSketchWindow.pack();
      itsSketchWindow.move(40,40);
      if(itsProjectWindow.itsProject.GetItems().size()==0)
	itsSketchWindow.itsProjectMenu.getItem(2).disable();
      if(itsSketchWindowList.size() == 1)
	itsProjectWindow.getMenuBar().getMenu(2).getItem(0).enable();
      AddThisWindowToMenus(itsSketchWindow);
      itsSketchWindow.show();
      break;	
    case OPEN_COMMAND:
      // create a new document from skratch
      ErmesPatcherDoc aPatcherDoc = new ErmesPatcherDoc();
      // invocate the 'Load' method of the document
      aPatcherDoc.Load(fileToLoad, pathForLoading);
      // create the new SketchWindow based on the Document created....
      itsSketchWindow = new ErmesSketchWindow(false, null);
      itsWindow = itsSketchWindow;
      itsSketchWindow.itsSketchPad.GetOffGraphics();
      try {
	itsSketchWindow.InitFromDocument(aPatcherDoc);
      }
      catch (Exception e) {
	ErrorDialog aErr = new ErrorDialog(itsProjectWindow, "Error while importing "+pathForLoading+fileToLoad);
	aErr.move(100, 100);
	aErr.show();
	e.printStackTrace(); // temporary, MDC
	return false;
      }

      itsSketchWindowList.addElement(itsSketchWindow);
      itsSketchWindow.inAnApplet = false;
      itsSketchWindow.setTitle(itsSketchWindow.itsDocument.GetName());
      aPatcherDoc.SetWindow(itsSketchWindow);
      CheckboxMenuItem aEditMenuItem = (CheckboxMenuItem)itsSketchWindow.itsEditMenu.getItem(9);
      aEditMenuItem.setState(doAutorouting);
      if(itsProjectWindow.itsProject.GetItems().size()==0)
	itsSketchWindow.getMenuBar().getMenu(2).getItem(2).disable();
      if(itsSketchWindowList.size() == 1)
	itsProjectWindow.getMenuBar().getMenu(2).getItem(0).enable();
      AddThisWindowToMenus(itsSketchWindow);
      itsSketchWindow.show();
      break;
      
    case NEW_PROJECT:
      itsProjectWindow = new ProjectWindow();
      itsProjectWindow.setTitle("Project manager");
      itsProjectWindow.move(0, 0);//start in the upper left position
      itsProjectWindow.pack();
      itsProjectWindow.show();
      itsProjectWindow.addKeyListener(itsProjectWindow);
      break;
      
    case REQUIRE_CONNECTION:
      itsConnDialog = new ConnectionDialog(itsProjectWindow);
      itsConnDialog.move(200,200);
      itsConnDialog.show();
      
      // hard-coded for now....
      
      break;
      
    case CONNECTION_CHOOSEN:
      if (itsConnDialog.connectionLine == itsConnDialog.REMOTE_CONNECTION) {
	int port = Integer.parseInt(itsConnDialog.portNo);
	itsServer = new FtsSocketServer(itsConnDialog.hostName, port);

	itsServer.setPostStream(GetPrintStream());

	itsServer.setParameter("ftsdir", "/u/worksta/dececco/projects/imax/fts/bin/origin/debug");

	itsServer.setParameter("ftsname", "fts");
	itsServer.start();				
	runHooks("start");
      }
      else if (itsConnDialog.connectionLine == itsConnDialog.LOCAL_CONNECTION) {
	itsServer = new FtsSubProcessServer();
	itsServer.setPostStream(itsPrintStream);
	itsServer.setParameter("ftsdir", "/u/worksta/maggi/projects/fts");
	itsServer.setParameter("ftsname", "fts");
	itsServer.start();	//?
	runHooks("start");
      }
      else
	itsServer = null; //no server, for now
      break;
    case OPEN_WITH_AUTO_ROUTING:
      doAutorouting = !doAutorouting;
      //qui controlla lo stato del menu corrispondeente e gli adatta quello
      //dei menu delle altre finestre.....
      CheckboxMenuItem aMenuItem = (CheckboxMenuItem)itsProjectWindow.getMenuBar().getMenu(0).getItem(4);
      aMenuItem.setState(doAutorouting);
      for(int i=0; i<itsSketchWindowList.size(); i++){
	aSketchWindow = (ErmesSketchWindow) itsSketchWindowList.elementAt(i);
	aMenuItem = (CheckboxMenuItem)aSketchWindow.getMenuBar().getMenu(0).getItem(4);
	aMenuItem.setState(doAutorouting);
      }
      break;
    case STACK_WINDOWS:
      if ((itsSketchWindowList.size()==0)&&(itsEditorsFrameList.size()==0)) break;
      StackWindows();
      break;
    case TILE_WINDOWS:
      TileWindows();
      break;
    case TILEVERTICAL_WINDOWS:
      TileVerticalWindows();
      break;
    case SELECT_ALL:
      ErmesObject aObject;
      Vector aElementsList = itsSketchWindow.GetSketchPad().GetElements();
      Vector aSelectedList = itsSketchWindow.GetSketchPad().GetSelectedList();
      for (Enumeration e = aElementsList.elements() ; e.hasMoreElements() ;) {
	aObject = (ErmesObject) e.nextElement();
	aSelectedList.addElement(aObject);
	aObject.Select();
      }
      itsSketchWindow.GetSketchPad().repaint();
      break;
    case SNAP_TO_GRID:
      for (int k=0; k<itsSketchWindowList.size(); k++) {
	aSketchWindow = (ErmesSketchWindow) itsSketchWindowList.elementAt(k);
	aSketchWindow.SetSnapToGrid();
      }
      break;
    case AUTO_ROUTING:
      itsSketchWindow.SetAutorouting();
      break;
    case CLOSE_WINDOW:
      if(itsWindow!=null) {
	itsWindow.Close();
	itsWindow.GetFrame().hide();
      }
      if(itsSketchWindowList.isEmpty()){
	itsSketchWindow = null;
	if(itsEditorsFrameList.isEmpty()){
	  itsWindow = null;	
	  itsProjectWindow.getMenuBar().getMenu(2).getItem(0).disable();
	}
      } 
      //if (itsSketchWindow == null)
	//itsPrintStream.println("internal error: null SketchWindow");
      //else if (itsSketchWindow.itsDocument == null)
	//itsPrintStream.println("internal error: null document");
      //else if (((ErmesPatcherDoc)itsSketchWindow.itsDocument).itsPatcher == null)
	//itsPrintStream.println("internal error: null root object");
      //else ((ErmesPatcherDoc)itsSketchWindow.itsDocument).itsPatcher.delete();
      break;
    case QUIT_APPLICATION:
      MaxWindow aWindow;

      runHooks("exit");// run the exit hooks

      for(Enumeration e=itsSketchWindowList.elements(); e.hasMoreElements();){
	aSketchWindow = (ErmesSketchWindow)e.nextElement();
	if(!aSketchWindow.Close()) return true;
      }
      for(Enumeration e=itsEditorsFrameList.elements(); e.hasMoreElements();){
	aWindow = (MaxWindow)e.nextElement();
	if(!aWindow.Close()) return true;
      }

      if (itsConsoleWindow != null)
	{
	  itsConsoleWindow.setVisible(false);
	  itsProjectWindow.hide();
	  itsConsoleWindow.dispose();
	  itsProjectWindow.dispose();
	}

      if (itsServer != null) itsServer.stop();
      Runtime.getRuntime().exit(0);
      break;
    case PRINT_WINDOW: 
      PrintJob aPrintJob = Toolkit.getDefaultToolkit().getPrintJob(itsSketchWindow, "print1", null);
      Graphics aPrintGraphics = aPrintJob.getGraphics();
	
      itsSketchWindow.paint(aPrintGraphics);
      aPrintGraphics.dispose();
      aPrintJob.end();
      /* aPrintJob.finalize(); */
      /*      ErrorDialog aErr = new ErrorDialog(itsProjectWindow, "Printing not implemented yet.");
      aErr.move(100, 100);
      aErr.show(); */
      break;
    case ADD_WINDOW:
      itsProjectWindow.itsProject.AddToProject((ErmesPatcherDoc)itsSketchWindow.GetDocument(), itsSketchWindow);
      UpdateProjectMenu();
      break;
    case ADD_FILES:
      String aType = new String();
      MaxResourceId aResId;
      String aExt;
      boolean found = false;
      /*if (fileToLoad.endsWith(".pat")) aType = "patcher";	
	else if (fileToLoad.endsWith(".esp")) aType = "espresso";*/
      for (Enumeration e= resourceVector.elements(); e.hasMoreElements() && !found;) {
	aResId = (MaxResourceId) e.nextElement();
	for (Enumeration e1 = aResId.resourceExtensions.elements(); e1.hasMoreElements();) {
	  aExt = (String) e1.nextElement();
	  if (fileToLoad.endsWith(aExt)) {
	    aType = aResId.resourceName;
	    found = true;
	    break;
	  }
	}
      }
      if (!found) aType = "unknown";
      itsProjectWindow.itsProject.AddToProject(fileToLoad, aType, pathForLoading);//per ora
      UpdateProjectMenu();
      break;
    case REMOVE_FILES:
      itsProjectWindow.itsProject.RemoveFromProject();
      if(itsProjectWindow.itsProject.GetItems().size()==0){
	itsProjectWindow.getMenuBar().getMenu(2).getItem(2).disable();
	for(int m=0; m<itsSketchWindowList.size(); m++){
	  aSketchWindow = (ErmesSketchWindow) itsSketchWindowList.elementAt(m);
	  aSketchWindow.getMenuBar().getMenu(2).getItem(2).disable();
	}
      }
      break;
    }	
    return true;
  }
  
  public void TileVerticalWindows(){
    Rectangle aRect = new Rectangle();
    Frame aWindow;
    int num = itsSketchWindowList.size()+itsEditorsFrameList.size();
    if(num!=0){
      int aWidth = (int)java.lang.Math.floor(SCREENHOR/num);
      Dimension d = new Dimension(aWidth - 10, SCREENVERT - 35);
      aRect.x = 7; aRect.y = 5;
      aRect.width= d.width;
      aRect.height = d.height;
      
      for (int k=0; k<num; k++) {
	if(k<itsSketchWindowList.size()) aWindow = (Frame)itsSketchWindowList.elementAt(k);
	else aWindow = (Frame)itsEditorsFrameList.elementAt(k-itsSketchWindowList.size());
	if(k>0)
	  aRect.x+=(d.width+7);
	aWindow.reshape(aRect.x, aRect.y, aRect.width, aRect.height);
      }
    }
  }


  public void TileWindows(){
    Rectangle aRect2 = new Rectangle();
    Rectangle aStartRect = new Rectangle();
    Dimension d2 = new Dimension();
    Frame  aWindow;
    boolean changHor = false;
    int z, j;
    int num = itsSketchWindowList.size()+itsEditorsFrameList.size();
    if(num!=0){
      z=1;
      if(num==1)
	j=1;
      else
	j=2;
      while(z*j<num){
	z++;
	if(z*j<num)
	  j++;
      }
      int res = (int)java.lang.Math.floor(SCREENVERT/j);
      d2.height = res - 35;
      res = (int)java.lang.Math.floor(SCREENHOR/z);
      d2.width = res - 10;
      aRect2.y = 5;aRect2.x = 7;
      aRect2.height = d2.height;
      aRect2.width =d2.width;
      aStartRect.x = aRect2.x;
      aStartRect.y = aRect2.y;
      aStartRect.width = aRect2.width;
      aStartRect.height = aRect2.height;
      
      for (int k=0; k<num; k++) {
	if(k<itsSketchWindowList.size()) aWindow = (Frame) itsSketchWindowList.elementAt(k);
	else aWindow = (Frame) itsEditorsFrameList.elementAt(k-itsSketchWindowList.size());
	res = (int)java.lang.Math.floor(k/j);
	if((res*j) == k)
	  changHor=false;
	if((res!=0)&&(!changHor)){
	  aRect2.x = aStartRect.x + (d2.width+7)*res;
	  aRect2.y = aStartRect.y;
	  aRect2.width = aStartRect.width;
	  aRect2.height = aStartRect.height;
	  changHor=true;
	}
	else
	  if(k>0)
	    aRect2.y += d2.height + 25;
	
	aWindow.reshape(aRect2.x, aRect2.y, aRect2.width, aRect2.height);
      }
    } 
  }


  public void StackWindows(){
    Dimension d;
    ErmesSketchWindow aSketchWindow;
    Frame aWindow;
    Rectangle aRect = new Rectangle();
    if(itsSketchWindow!=null) d = itsSketchWindow.preferredSize();
    else d = ((Frame)itsWindow).preferredSize();
    aRect.x = 50; aRect.y = 50;
    aRect.width = d.width;
    aRect.height = d.height;
    for (int i=0; i< itsSketchWindowList.size(); i++) {
      aSketchWindow = (ErmesSketchWindow) itsSketchWindowList.elementAt(i);
      //if(aSketchWindow!=itsSketchWindow){
      aRect.x+=20;aRect.y+=20;
      aSketchWindow.reshape(aRect.x, aRect.y, aRect.width, aRect.height);
      //}
    }
    for (int j=0; j< itsEditorsFrameList.size(); j++) {
      aWindow = (Frame) itsEditorsFrameList.elementAt(j);
      aRect.x+=20;aRect.y+=20;
      //aWindow.reshape(aRect.x, aRect.y, aRect.width, aRect.height);
      aWindow.move(aRect.x, aRect.y);
    }
    //aRect.x+=20;aRect.y+=20;
    //if(itsSketchWindow!=null)itsSketchWindow.reshape(aRect.x, aRect.y, aRect.width, aRect.height);
    //else ((Frame)itsWindow).reshape(aRect.x, aRect.y, aRect.width, aRect.height);
  }



  // public void SetCurrentWindow(ErmesSketchWindow theSketchWindow){
  public void SetCurrentWindow(MaxWindow theWindow){
    if(theWindow instanceof ErmesSketchWindow)itsSketchWindow = (ErmesSketchWindow)theWindow;
    itsWindow = theWindow;
    GetCurrentProject().SetCurrentEntry(itsWindow.GetDocument().GetWholeName());
  }
  
  public ErmesSketchWindow GetCurrentWindow() {
    return itsSketchWindow;
  }
	
  public ProjectWindow GetProjectWindow() {
    return itsProjectWindow;
  }
	
  public ConsoleWindow GetConsoleWindow() {
    return itsConsoleWindow;
  }

  public Project GetCurrentProject() {
    return itsProjectWindow.itsProject;
  }
	
  public static PrintStream GetPrintStream(){
	return itsPrintStream;	
  }
	
  public void UpdateProjectMenu(){
    ErmesSketchWindow aSketchWindow;
    if(itsProjectWindow.itsProject.GetItems().size()==1){
      itsProjectWindow.getMenuBar().getMenu(2).getItem(2).enable();
      for(int m=0; m<itsSketchWindowList.size(); m++){
	aSketchWindow = (ErmesSketchWindow) itsSketchWindowList.elementAt(m);
	aSketchWindow.getMenuBar().getMenu(2).getItem(2).enable();
      }
    }
  }

  /** Functions to add application hooks */

  public void addHook(String name, String code)
  {
    itsHookTable.addHook(name, code);
  }

  /** Functions to run application hooks */

  public void runHooks(String name)
  {
    itsHookTable.runHooks(name);
  }

  /** His majesty the main method */

  public static void main(String args[]) {
    // main function parse the argument line and create the main class...
    //create a new default Properties object
    ermesProperties = new Properties(System.getProperties());

    //start parsing arguments
    // don't check for valid options, so the user can set
    // command line arguments that can be accessed from tcl scripts
    // and we don't know yet

    for (int i=0; i<args.length &&args[i].startsWith("-"); i++) 
      ermesProperties.put(args[i].substring(1), args[++i]);

    // Default values
    if (ermesProperties.get("root") == null) {
      //user didn't specify the root. Take the /usr/local/max default directory.
      ermesProperties.put("root", "/usr/local/max");
    }

    //Install as default print stream the system output

    itsPrintStream = System.out;

    new MaxApplication();
  }

  /** This private method build the tcl interpreter, 
      and do all the required initialization to it
      (load the jmax commands, set global jmax variables
      and so on, load the bootstrap file and all the boot
      time packages).
      Must be called after argument parsing.
      */

  static private void makeTclInterp()
  {
    // itsInterp = new tcl.lang.Interp(); should move here !!!

    // Installing the kernel packages 

    ircam.jmax.tcl.TclMaxPackage.installPackage(itsInterp);
    ircam.jmax.fts.tcl.TclFtsPackage.installPackage(itsInterp);
    ircam.jmax.editors.ermes.tcl.TclErmesPackage.installPackage(itsInterp);

    try
      {
	// Set the variables; only global variables that
	// cannot be changed from ~/.ermesrc are fixed here.
	// Global variables are jmaxRootDir, jmaxUserDir
	//

	itsInterp.setVar("jmaxRootDir", TclString.newInstance(ermesProperties.getProperty("root")), TCL.GLOBAL_ONLY);
	itsInterp.setVar("jmaxUserDir", TclString.newInstance(ermesProperties.getProperty("user.home")), TCL.GLOBAL_ONLY);

	// Load the "jmaxboot.tcl" file that will do whatever is needed to
	// create the startup configuration, included reading user files

	itsInterp.evalFile(ermesProperties.getProperty("root") +
			   ermesProperties.getProperty("file.separator") + "tcl" +
			   ermesProperties.getProperty("file.separator") +  "jmaxboot.tcl");
      }
    catch (TclException e)
      {
	itsPrintStream.println("TCL error in initialization " + e);
      }
  }

  /** This method install the console; a part of it should go
    in the UI classes (why to call awt things like pack here ??),
    and another part should just become tcl ??? */

  public void makeMaxConsole()
  {
    Console itsConsole;

    itsConsole = new Console(itsInterp);
    itsConsole.Start();


    itsPrintStream = itsConsole.getPrintStream();
    System.setOut(itsPrintStream);

    itsConsoleWindow = new ConsoleWindow(itsConsole, "jMax Console");
    itsConsoleWindow.Init(itsProjectWindow.itsProject);
    itsConsoleWindow.setLocation(0,0);
    itsConsoleWindow.pack();
    itsConsoleWindow.setVisible(true);

  }
}








