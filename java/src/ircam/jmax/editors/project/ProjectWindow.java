package ircam.jmax.editors.project;

import java.awt.*;
import java.util.*;
import ircam.jmax.*;
import ircam.jmax.utils.*;
import ircam.jmax.dialogs.*;

import ircam.jmax.editors.ermes.*;


//IMPORTANT WARNING:
// the editors loaded in this class MUST be handled dinamically (this regards the NewFile and OpenFile methods)
// 1)the imports of every editor's package will be useless
// 2)the NewFile method will not be changed when we add an editor
// 3)the OpenFile method will work without modifications

/**
 * The main class for the project managing. It handles the list
 * of entries, and all the documents associated. It knows how to
 * associate a file to an editor and then to a java package.
 */
public class ProjectWindow extends Frame {
  
  private static int untitledTxtCounter = 1;
  private static int untitledTabCounter = 1;
  private static int untitledSeqCounter = 1; 
  boolean inAnApplet = false;
  public Project itsProject = new Project(this);
  ErmesScrollerView itsScrollerView = new ErmesScrollerView(this, itsProject);
  ProjectBar itsProjectBar;
  Menu itsFileMenu;
  Menu itsNewFileMenu;
  Menu itsEditMenu;	
  Menu itsProjectMenu;	
  Menu itsWindowsMenu;
  static final ExtensionFilter itsTpaFilter = new ExtensionFilter(".tpa");
  static final ExtensionFilter itsPatFilter = new ExtensionFilter(".pat");
  
  Vector itsWindowMenuList;

    
  //--------------------------------------------------------
  //	CONSTRUCTOR
  //
  //--------------------------------------------------------
  public ProjectWindow() {
    itsWindowMenuList = new Vector();

    GridBagLayout gridbag = new GridBagLayout();
    GridBagConstraints c = new GridBagConstraints();       
    setLayout(gridbag);
            
    // create the projectBar
    itsProjectBar = new ProjectBar();
    
    //menu & app        
    MenuBar mb = new MenuBar();

    itsFileMenu = CreateFileMenu();
    mb.add(itsFileMenu);
    
    itsEditMenu = CreateEditMenu();
    mb.add(itsEditMenu);
    
    itsProjectMenu = CreateProjectMenu();
    mb.add(itsProjectMenu);
    
    itsWindowsMenu = CreateWindowsMenu();
    mb.add(itsWindowsMenu);
    
    setMenuBar(mb);
               
    ///////////
    
    c.gridwidth = GridBagConstraints.REMAINDER;
    c.gridx = GridBagConstraints.RELATIVE;
    c.gridy = GridBagConstraints.RELATIVE;
    c.weightx = 0.1;
    c.weighty = 0.0;
    c.anchor = GridBagConstraints.NORTHWEST;
    gridbag.setConstraints(itsProjectBar, c);
    add(itsProjectBar);
        
    c.weightx = 1.0;
    c.weighty = 1.0;
    c.gridy = 1;
    Insets insets = new Insets(10,0,0,0);
    c.insets = insets;
    c.anchor = GridBagConstraints.EAST;
    c.fill = GridBagConstraints.BOTH;
    gridbag.setConstraints(itsScrollerView, c);
    add(itsScrollerView);
  }
    
  private Menu CreateNewFileMenu(){
    MaxResourceId aResId;
    String aString;
    Menu newFileMenu = new Menu("New...  Ctrl+N");

    for(int i=0; i< MaxApplication.getApplication().resourceVector.size();i++){
      aResId = (MaxResourceId)MaxApplication.getApplication().resourceVector.elementAt(i);
      aString = aResId.GetName();
      newFileMenu.add(new MenuItem(aString));
    }
    return newFileMenu;
  }

  private Menu CreateFileMenu() {
    Menu fileMenu = new Menu("File");
    itsNewFileMenu = CreateNewFileMenu();
    fileMenu.add(itsNewFileMenu);
    fileMenu.add(new MenuItem("Open... Ctrl+O"));
    fileMenu.add(new MenuItem("Import..."));
    fileMenu.add(new MenuItem("Close   Ctrl+W"));
    fileMenu.add(new MenuItem("-"));
    CheckboxMenuItem aCheckItem1 = new CheckboxMenuItem("Open with Autorouting");
    aCheckItem1.setState(true);
    fileMenu.add(aCheckItem1);
    fileMenu.add(new MenuItem("-"));
    fileMenu.add(new MenuItem("Save  Ctrl+S"));
    fileMenu.add(new MenuItem("Save As..."));
    fileMenu.add(new MenuItem("-"));
    fileMenu.add(new MenuItem("Print... Ctrl+P"));
    fileMenu.add(new MenuItem("System statistics..."));
    fileMenu.add(new MenuItem("Quit    Ctrl+Q"));
    return fileMenu;
  }

  private boolean IsInFileMenu(String theName) {
    return( theName.equals("Open... Ctrl+O")||theName.equals("Import...")
	    ||theName.equals("Close   Ctrl+W")||theName.equals("Open with Autorouting") 
	    ||theName.equals("Save  Ctrl+S")||theName.equals("Save As...") 
	    ||theName.equals("Print... Ctrl+P")||theName.equals("Quit    Ctrl+Q")
	    ||theName.equals("System statistics...")||IsInNewFileMenu(theName));
  }

  private boolean IsInNewFileMenu(String theName){
    MaxResourceId aResId;
    String aString;

    for(int i=0; i< MaxApplication.getApplication().resourceVector.size();i++){
      aResId = (MaxResourceId)MaxApplication.getApplication().resourceVector.elementAt(i);
      aString = aResId.GetName();
      if(aString.equals(theName)) return true;
    }
    return false;
  }

  private Menu CreateEditMenu() {
    Menu editMenu = new Menu("Edit");
    editMenu.add(new MenuItem("Cut"));
    editMenu.add(new MenuItem("Copy"));
    editMenu.add(new MenuItem("Paste"));
    editMenu.add(new MenuItem("Clear")); 
    editMenu.getItem(0).disable();
    editMenu.getItem(1).disable();
    editMenu.getItem(2).disable();
    editMenu.getItem(3).disable();
    return editMenu;
  }

  private boolean IsInEditMenu(String theName) {
    return(	theName.equals("Cut") || theName.equals("Copy") || theName.equals("Paste") 
		|| theName.equals("Clear"));
  }
  
  private Menu CreateProjectMenu() {
    Menu ProjectMenu = new Menu("Project");
    ProjectMenu.add(new MenuItem("Add Window"));
    ProjectMenu.add(new MenuItem("Add files..."));
    ProjectMenu.add(new MenuItem("Remove files"));
    ProjectMenu.getItem(0).disable();
    ProjectMenu.getItem(2).disable();
    return ProjectMenu;
  }

  private boolean IsInProjectMenu(String theName) {
    return(theName.equals("Add Window") || theName.equals("Add files...") || theName.equals("Remove files"));
  }

  private Menu CreateWindowsMenu() {
    Menu windowsMenu = new Menu("Windows");
    windowsMenu.add(new MenuItem("Stack"));
    windowsMenu.add(new MenuItem("Tile"));
    windowsMenu.add(new MenuItem("Tile Vertical"));
    windowsMenu.add(new MenuItem("-"));
    windowsMenu.add(new MenuItem("Ermes Console"));
    windowsMenu.add(new MenuItem("-"));
    windowsMenu.add(new MenuItem("Jacl Console"));
    return windowsMenu;
  }
	
  public void AddWindowToMenu(String theName){
    if(itsWindowsMenu.countItems()==7) itsWindowsMenu.add(new MenuItem("-"));
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
	if(itsWindowsMenu.countItems()==8)itsWindowsMenu.remove(7);
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
    return(theName.equals("Stack") || theName.equals("Tile") || theName.equals("Tile Vertical")||theName.equals("Ermes Console")||theName.equals("Jacl Console")||IsAWindowName(theName) || IsAnEditorFrameName(theName));
  }
  
  private boolean IsAWindowName(String theName){
    ErmesSketchWindow aSketchWindow; 
    String aString;
    for (int i=0; i< MaxApplication.itsSketchWindowList.size(); i++) {
      aSketchWindow = (ErmesSketchWindow) MaxApplication.itsSketchWindowList.elementAt(i);
      aString = aSketchWindow.GetDocument().GetName();
      if(aString.equals(theName)) return true;
    }
    return false;
  } 
  
  private boolean IsAnEditorFrameName(String theName){
    MaxWindow aWindow; 
    for (int i=0; i< MaxApplication.itsEditorsFrameList.size(); i++) {
      aWindow = (MaxWindow) MaxApplication.itsEditorsFrameList.elementAt(i);
      if(aWindow.GetDocument().GetName().equals(theName)) return true;
    }
    return false;
  } 
  
  //--------------------------------------------------------
  //	GetProject
  //	returns the associated Project
  //--------------------------------------------------------
  public Project GetProject() {
    return itsProject;
  }


  //--------------------------------------------------------
  //	action
  //	high-level events handler
  //--------------------------------------------------------
  public boolean action(Event event, Object arg) {
    if (event.target instanceof MenuItem) {
      MenuItem aMenuItem = (MenuItem) event.target;
      String itemName = aMenuItem.getLabel();
      
      if (IsInFileMenu(itemName)) return FileMenuAction(aMenuItem, itemName);
      if (IsInEditMenu(itemName)) return EditMenuAction(aMenuItem, itemName);
      if (IsInProjectMenu(itemName)) return ProjectMenuAction(aMenuItem, itemName);
      if (IsInWindowsMenu(itemName)) return WindowsMenuAction(aMenuItem, itemName);
    }
    return true;
  }
  
  public void New(){
     String aNewFileType;
     ProjectNewDialog aNewDialog = new ProjectNewDialog(this);
     Point aPoint = location();
     aNewDialog.move(aPoint.x+100, aPoint.y+100);
     aNewDialog.show();
     aNewFileType = aNewDialog.GetNewFileType();
     NewFile(aNewFileType);
  }

  /*private*/public MaxEditor NewFile(String theFileType){//OK OK, this SHOULD be private, but it's called
    //also in ErmesObjExternal when he realizes to be a table
    if(theFileType.equals("patcher"))MaxApplication.getApplication().ObeyCommand(MaxApplication.NEW_COMMAND);
    else if(theFileType.equals("")) return null;
    /* @@@@@@ Change new to use resources to find dynamically the editor
    else if(theFileType.equals("table")){
      Tabler aTable = new Tabler();
      aTable.Init(itsProject);
      aTable.InitDoc("tab_untitled"+(untitledTabCounter++), itsProject);
      MaxApplication.itsEditorsFrameList.addElement(aTable.GetWindow());
      MaxApplication.getApplication().SetCurrentWindow(aTable);
      MaxApplication.getApplication().AddThisFrameToMenus(aTable.GetName());
      return aTable;
    }
    else if((theFileType.equals("espresso"))||(theFileType.equals("text"))){
      TextEditor aTextEditor = new TextEditor();
      aTextEditor.Init(itsProject);
      aTextEditor.InitDoc("tx_untitled"+(untitledTxtCounter++), itsProject);
      MaxApplication.itsEditorsFrameList.addElement(aTextEditor.GetWindow());
      MaxApplication.getApplication().SetCurrentWindow(aTextEditor);
      MaxApplication.getApplication().AddThisFrameToMenus(aTextEditor.GetName());
    }
    else if(theFileType.equals("sequence")){
      Sequencer aSequencer = new Sequencer();
      aSequencer.Init(itsProject);
      aSequencer.InitDoc("seq_untitled"+(untitledSeqCounter++), itsProject);
      MaxApplication.itsEditorsFrameList.addElement(aSequencer.GetWindow());
      MaxApplication.getApplication().SetCurrentWindow(aSequencer);
      MaxApplication.getApplication().AddThisFrameToMenus(aSequencer.GetName());
    }
    */
    return null;//WARNING
  }
  
  public boolean Open(){
    FileDialog fd = new FileDialog(this, "FileDialog");
    String aOpeningFile;
    fd.setFile("");
    fd.show();
    aOpeningFile = fd.getFile();
    if(aOpeningFile==null) return false;
    if(!(aOpeningFile.equals(""))){
      return OpenFile(aOpeningFile, fd.getDirectory());
    }
    return false;
  }


  public String GetExtension(String theFileName){
    int aLength = theFileName.length();
    String aExtension = theFileName.substring(aLength-3, aLength);
    if(theFileName.endsWith("."+aExtension)) return aExtension;
    else return "";
  }

  public boolean OpenFile(String theFileName, String theDirectory){
    
    String aExtension = GetExtension(theFileName);
    MaxResourceId aResId = null;

    if(aExtension.equals("tpa")){
            try {
	      MaxApplication.getTclInterp().EvalFile(theDirectory+theFileName);
	}
	catch (Exception e) {
	MaxApplication.getPostStream().println("error while opening .tpa"+theDirectory+theFileName+" "+e.toString());
	e.printStackTrace();
	return false;
	}
      MaxApplication.itsSketchWindowList.addElement(MaxApplication.getApplication().itsSketchWindow);
      MaxApplication.getApplication().AddThisWindowToMenus(MaxApplication.getApplication().itsSketchWindow);
      return true;
    }
    else if(aExtension.equals("pat")){
      boolean temp = MaxApplication.getApplication().doAutorouting;//files .pat charged without autorouting
      MaxApplication.getApplication().doAutorouting = false;
      MaxApplication.getApplication().Load(theFileName, theDirectory);
      MaxApplication.getApplication().doAutorouting = temp;
      return true;
    }
    else if ((aResId = ResIdWithExtension("."+aExtension))!=null) {
      Object placeHolder;
      MaxDocument aDocument;
      try {
	placeHolder = Class.forName("editors." + aResId.resourceName +"." + aResId.preferred_resource_handler).newInstance();
      }catch(ClassNotFoundException k) {
	MaxApplication.getPostStream().println("editor not found:" + aResId.preferred_resource_handler); 
	return false;
      }
      catch(IllegalAccessException k) {
	MaxApplication.getPostStream().println("cannot execute " + aResId.preferred_resource_handler + " illegal access - is this \"public\"?");
	return false;
      }
      catch(InstantiationException k) {
	MaxApplication.getPostStream().println("cannot execute " + aResId.preferred_resource_handler + ": instantiation error");
	return false;
      }     
      if(placeHolder instanceof MaxDocument){
	if(placeHolder instanceof MaxEditor){
	  MaxEditor aEditor = (MaxEditor) placeHolder;
	  aEditor.Init(itsProject);
	}
	
	aDocument = (MaxDocument) placeHolder;
	aDocument.InitDoc(theFileName, theDirectory, theFileName, itsProject);
	MaxApplication.itsEditorsFrameList.addElement(aDocument.GetWindow());
	MaxApplication.getApplication().SetCurrentWindow(aDocument.GetWindow());
	MaxApplication.getApplication().AddThisFrameToMenus(aDocument.GetName());
      }
      return true;
    }
    //tutto il resto viene aperto con il text editor
    else {
      
      //       TextEditor aDocument = null; 
      //       aDocument = new TextEditor();
      //       aDocument.Init(itsProject);
      //       aDocument.InitDoc(theFileName,theDirectory,theFileName,itsProject); 
      //       MaxApplication.itsEditorsFrameList.addElement(aDocument.GetWindow());
      //       MaxApplication.getApplication().SetCurrentWindow(aDocument.GetWindow());
      //       MaxApplication.getApplication().AddThisFrameToMenus(aDocument.GetName()); 
    }
    return true;
 }

  MaxResourceId ResIdWithExtension(String theExtension) {
    MaxResourceId aResId = null;
    String aExtension;
    for(Enumeration e = MaxApplication.getApplication().resourceVector.elements(); e.hasMoreElements();) {
      aResId = (MaxResourceId) e.nextElement();
      for (Enumeration e1=aResId.resourceExtensions.elements(); e1.hasMoreElements();) {
	aExtension = (String) e1.nextElement();
	if(aExtension.equals(theExtension))
	  return aResId;
      }
    }
    return null;//resource extension is unknown in the current system settings...
  }
    
  private boolean FileMenuAction(MenuItem theMenuItem, String theString) {
    if (theString.equals("Open... Ctrl+O")) {
      return Open();
    }
    else if (theString.equals("Import...")) {
      FileDialog fd = new FileDialog(this, "FileDialog");
      fd.setFile("");
      fd.setFilenameFilter(itsPatFilter);
      fd.show();
      if(fd.getFile()==null) return false;
      if(!(fd.getFile().equals(""))){
	boolean temp = MaxApplication.getApplication().doAutorouting;
	MaxApplication.getApplication().doAutorouting = false;
	MaxApplication.getApplication().Load(fd.getFile(), fd.getDirectory());
	MaxApplication.getApplication().doAutorouting = temp;
      }
    }
    if (theString.equals("Save  Ctrl+S")) {
      if(MaxApplication.getApplication().itsWindow != null) MaxApplication.getApplication().itsSketchWindow.GetDocument().Save();
    }
    if (theString.equals("Save As...")) {
      if(MaxApplication.getApplication().itsWindow != null){
	MaxApplication.getApplication().itsWindow.GetDocument().SetFileName("");
	MaxApplication.getApplication().itsWindow.GetDocument().Save();
      }
    }
    else if (theString.equals("Close   Ctrl+W")) {
      MaxApplication.getApplication().ObeyCommand(MaxApplication.CLOSE_WINDOW);
    }
    //try to print...
    else if (theString.equals("Print... Ctrl+P")) {
      MaxApplication.getApplication().ObeyCommand(MaxApplication.PRINT_WINDOW);
    }
    else if (theString.equals("Quit    Ctrl+Q")) { 
      MaxApplication.getApplication().ObeyCommand(MaxApplication.QUIT_APPLICATION);
    }
    else if (theString.equals("Open with Autorouting")) {
      MaxApplication.getApplication().doAutorouting = !MaxApplication.getApplication().doAutorouting;
    }
    else if (theString.equals("System statistics...")) {
      StatisticsDialog aDialog = new StatisticsDialog(this);
	aDialog.move(100, 100);
	aDialog.show();
    }
    else{//qui siamo nel caso di New...
      NewFile(theString);
    }
    return true;
  }
	
  private boolean EditMenuAction(MenuItem theMenuItem, String theString) {
    return true;
  }

  private boolean ProjectMenuAction(MenuItem theMenuItem, String theString) {
    if (theString.equals("Add Window")) {
      MaxApplication.getApplication().ObeyCommand(MaxApplication.ADD_WINDOW);
    }
    else if (theString.equals("Add files...")) {
      FileDialog fd = new FileDialog(this, "Add To Project");
      fd.setFile("");
      fd.show();
      if(fd.getFile()==null) return false;
      if(fd.getFile().compareTo("")!= 0)
	MaxApplication.getApplication().AddToProject(fd.getFile(), fd.getDirectory());
    }
    else if (theString.equals("Remove files")) {
      MaxApplication.getApplication().ObeyCommand(MaxApplication.REMOVE_FILES);
    }
    return true;
  }

  private boolean WindowsMenuAction(MenuItem theMenuItem, String theString) {
    ErmesObject aObject;
    if (theString.equals("Stack")) {
      MaxApplication.getApplication().ObeyCommand(MaxApplication.STACK_WINDOWS);
    }
    else if (theString.equals("Tile")) {
      MaxApplication.getApplication().ObeyCommand(MaxApplication.TILE_WINDOWS);
    }
    else if (theString.equals("Tile Vertical")) {
      MaxApplication.getApplication().ObeyCommand(MaxApplication.TILEVERTICAL_WINDOWS);
    }
    else if (theString.equals("Ermes Console")) {
      MaxApplication.getApplication().GetConsole().ToFront();
    }
    else if (theString.equals("Jacl Console")) {
      MaxApplication.getApplication().GetShell().ToFront();
    }
    else BringToFront(theString);
    return true;
  }
 
  private void BringToFront(String theName){
    ErmesSketchWindow aSketchWindow;
    MaxWindow aWindow;
    String aString;
    for (int i=0; i< MaxApplication.getApplication().itsSketchWindowList.size(); i++) {
      aSketchWindow = (ErmesSketchWindow) MaxApplication.getApplication().itsSketchWindowList.elementAt(i);
      aString = aSketchWindow.getTitle();
      if(aString.equals(theName)) {
	aSketchWindow.toFront();
	return;
      }
    }
    for (int j=0; j< MaxApplication.itsEditorsFrameList.size(); j++) {
      aWindow = (MaxWindow) MaxApplication.itsEditorsFrameList.elementAt(j);
      aString = aWindow.GetDocument().GetName();
      if(aString.equals(theName)) {
	aWindow.ToFront();
	return;
      }
    }
  }

  public boolean keyDown(Event evt,int key){

    if (evt.controlDown()){
      if(key == 14) New();
      else if(key == 15) Open();
      else if(key == 16) MaxApplication.getApplication().ObeyCommand(MaxApplication.PRINT_WINDOW);
      else if(key == 19){
	if(MaxApplication.getApplication().itsWindow != null) 
	  MaxApplication.getApplication().itsSketchWindow.GetDocument().Save();
      }
      else if(key == 23) MaxApplication.getApplication().ObeyCommand(MaxApplication.CLOSE_WINDOW);
      else if(key == 17) MaxApplication.getApplication().ObeyCommand(MaxApplication.QUIT_APPLICATION);
    }
    else{
      if(key == ircam.jmax.utils.Platform.RETURN_KEY) {
	/*if(itsProject.itsCurrentEntry.itsDocument!=null)
	   ((ErmesPatcherDoc)itsProject.itsCurrentEntry.itsDocument).itsSketchWindow.toFront();
	   itsProject.itsCurrentEntry.itsDocument.GetWindow().ToFront();
	  else 
	  if(itsProject.itsCurrentEntry.OpenEntryDocument()) return true;*/
	if(itsProject.itsCurrentEntry.itsDocument==null)
	  if(itsProject.itsCurrentEntry.OpenEntryDocument()) return true;
      }
      else if(key == Event.UP) itsProject.SelectPreviousEntry();
      else if(key == Event.DOWN) itsProject.SelectNextEntry();
    }
    return true;
  }
	
  //--------------------------------------------------------
  //	handleEvent
  //	low-level events handler
  //--------------------------------------------------------
  public boolean handleEvent(Event event) {
    //If we're running as an application, closing the window
    //should quit the application.
    if (event.id == Event.WINDOW_DESTROY) {
      if (inAnApplet) {
	dispose();
      } else {
	dispose();
	Runtime.getRuntime().exit(0);
      }
    }
    return super.handleEvent(event);
  }
	
  //--------------------------------------------------------
  //	paint
  //--------------------------------------------------------
  public void paint(Graphics g) {
    Color bg = getBackground();
    g.setColor(bg);
    Dimension d = itsProjectBar.size();
    g.draw3DRect(0, 0, d.width+5, d.height+5, true);
  }
    
	
  //--------------------------------------------------------
  // minimumSize()
  //--------------------------------------------------------
  public Dimension minimumSize() {
    return new Dimension(300,450); //(depending on the layout manager).
  }

  //--------------------------------------------------------
  // preferredSize()
  //--------------------------------------------------------
  public Dimension preferredSize() {
    return minimumSize();
  }
	
}


