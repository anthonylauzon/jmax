package ircam.jmax.editors.project;

import java.awt.*;
import java.awt.event.*;
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
public class ProjectWindow extends Frame implements KeyListener, WindowListener, ActionListener, ItemListener, FocusListener, MouseListener {
  
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


    validate();

    addKeyListener(this);
    addWindowListener(this);
    addMouseListener(this);
  }
    
  private Menu CreateNewFileMenu(){
    MaxResourceId aResId;
    MenuItem aMenuItem;
    String aString;
    Menu newFileMenu = new Menu("New...  Ctrl+N");

    for(int i=0; i< MaxApplication.getApplication().resourceVector.size();i++){
      aResId = (MaxResourceId)MaxApplication.getApplication().resourceVector.elementAt(i);
      aString = aResId.GetName();
      newFileMenu.add(aMenuItem = new MenuItem(aString));
      aMenuItem.addActionListener(this);
    }
    return newFileMenu;
  }

  private Menu CreateFileMenu() {
    MenuItem aMenuItem;
    CheckboxMenuItem aCheckItem;
    Menu fileMenu = new Menu("File");
    itsNewFileMenu = CreateNewFileMenu();
    fileMenu.add(itsNewFileMenu);
    fileMenu.add(aMenuItem = new MenuItem("Open... Ctrl+O"));
    aMenuItem.addActionListener(this);
    fileMenu.add(aMenuItem = new MenuItem("Import..."));
    aMenuItem.addActionListener(this);
    fileMenu.add(aMenuItem = new MenuItem("Close   Ctrl+W"));
    aMenuItem.addActionListener(this);
    fileMenu.add(new MenuItem("-"));
    aCheckItem = new CheckboxMenuItem("Open with Autorouting");
    aCheckItem.setState(true);
    fileMenu.add(aCheckItem);
    aCheckItem.addItemListener(this);
    fileMenu.add(new MenuItem("-"));
    fileMenu.add(aMenuItem = new MenuItem("Save  Ctrl+S"));
    aMenuItem.addActionListener(this);
    fileMenu.add(aMenuItem = new MenuItem("Save As..."));
    aMenuItem.addActionListener(this);
    fileMenu.add(new MenuItem("-"));
    fileMenu.add(aMenuItem = new MenuItem("Print... Ctrl+P"));
    aMenuItem.addActionListener(this);
    fileMenu.add(aMenuItem = new MenuItem("System statistics..."));
    aMenuItem.addActionListener(this);
    fileMenu.add(aMenuItem = new MenuItem("Quit    Ctrl+Q"));
    aMenuItem.addActionListener(this);
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
    MenuItem aMenuItem;
    Menu editMenu = new Menu("Edit");
    editMenu.add(aMenuItem = new MenuItem("Cut"));
    aMenuItem.addActionListener(this);
    editMenu.add(aMenuItem = new MenuItem("Copy"));
    aMenuItem.addActionListener(this);
    editMenu.add(aMenuItem = new MenuItem("Paste"));
    aMenuItem.addActionListener(this);
    editMenu.add(aMenuItem = new MenuItem("Clear")); 
    aMenuItem.addActionListener(this);
    editMenu.getItem(0).setEnabled(false);
    editMenu.getItem(1).setEnabled(false);
    editMenu.getItem(2).setEnabled(false);
    editMenu.getItem(3).setEnabled(false);
    return editMenu;
  }

  private boolean IsInEditMenu(String theName) {
    return(	theName.equals("Cut") || theName.equals("Copy") || theName.equals("Paste") 
		|| theName.equals("Clear"));
  }
  
  private Menu CreateProjectMenu() {
    MenuItem aMenuItem;
    Menu ProjectMenu = new Menu("Project");
    ProjectMenu.add(aMenuItem = new MenuItem("Add Window"));
    aMenuItem.addActionListener(this);
    ProjectMenu.add(aMenuItem = new MenuItem("Add files..."));
    aMenuItem.addActionListener(this);
    ProjectMenu.add(aMenuItem = new MenuItem("Remove files"));
    aMenuItem.addActionListener(this);
    ProjectMenu.getItem(0).setEnabled(false);
    ProjectMenu.getItem(2).setEnabled(false);
    return ProjectMenu;
  }

  private boolean IsInProjectMenu(String theName) {
    return(theName.equals("Add Window") || theName.equals("Add files...") || theName.equals("Remove files"));
  }

  private Menu CreateWindowsMenu() {
    MenuItem aMenuItem;
    Menu windowsMenu = new Menu("Windows");
    windowsMenu.add(aMenuItem = new MenuItem("Stack"));
    aMenuItem.addActionListener(this);
    windowsMenu.add(aMenuItem = new MenuItem("Tile"));
    aMenuItem.addActionListener(this);
    windowsMenu.add(aMenuItem = new MenuItem("Tile Vertical"));
    aMenuItem.addActionListener(this);
    windowsMenu.add(new MenuItem("-"));
    windowsMenu.add(aMenuItem = new MenuItem("jMax Console"));
    aMenuItem.addActionListener(this);
    return windowsMenu;
  }
	
  public void AddWindowToMenu(String theName){
    MenuItem aMenuItem;
    if(itsWindowsMenu.getItemCount()==5) itsWindowsMenu.add(new MenuItem("-"));
    itsWindowsMenu.add(aMenuItem = new MenuItem(theName));
    aMenuItem.addActionListener(this);
  }

  public void AddToSubWindowsMenu(String theTopWindowName, String theSubWindowName, boolean theFirstItem){
    MenuItem aItem;
    Menu aMenu;
    int aIndex = 0;
    if(theFirstItem){
      for(int i=0; i<itsWindowsMenu.getItemCount();i++){
	aItem = itsWindowsMenu.getItem(i);
	if(aItem.getLabel().equals(theTopWindowName)){
	  itsWindowsMenu.remove(aItem);
	  aIndex = i;
	  break;
	}
      }
      aMenu = new Menu(theTopWindowName);
      aMenu.add(aItem = new MenuItem(theTopWindowName));
      aItem.addActionListener(this);
      aMenu.add(aItem = new MenuItem(theSubWindowName));
      aItem.addActionListener(this);
      itsWindowsMenu.insert(aMenu, aIndex);
      itsWindowMenuList.addElement(aMenu);
    }
    else{
      for(Enumeration e = itsWindowMenuList.elements(); e.hasMoreElements();) {
	aMenu = (Menu) e.nextElement();
	if(theTopWindowName.equals(aMenu.getLabel())){
	  aMenu.add(aItem = new MenuItem(theSubWindowName));
	  aItem.addActionListener(this);
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
      for(int i=0; i<itsWindowsMenu.getItemCount();i++){
	aItem = itsWindowsMenu.getItem(i);
	if(aItem.getLabel().equals(theTopWindowName)){
	  itsWindowsMenu.remove(aItem);
	  itsWindowMenuList.removeElement(aItem);
	  aIndex = i;
	  break;
	}
      }
      aItem = new MenuItem(theTopWindowName);
      aItem.addActionListener(this);
      itsWindowsMenu.insert(aItem, aIndex);
    }
    else{
      for(Enumeration e = itsWindowMenuList.elements(); e.hasMoreElements();) {
	aMenu = (Menu) e.nextElement();
	if(theTopWindowName.equals(aMenu.getLabel())){
	  for(int j=0; j<aMenu.getItemCount();j++){
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
    for(int i=0; i<itsWindowsMenu.getItemCount();i++){
      aItem = itsWindowsMenu.getItem(i);
      if(aItem.getLabel().equals(theName)){
	itsWindowsMenu.remove(aItem);
	if(itsWindowsMenu.getItemCount()==6)itsWindowsMenu.remove(5);
	return;
      }
    }
  }
  
  public void ChangeWinNameMenu(String theOldName, String theNewName){
    MenuItem aItem;
    for(int i=0; i<itsWindowsMenu.getItemCount();i++){
      aItem = itsWindowsMenu.getItem(i);
      if(aItem.getLabel().equals(theOldName)){
	aItem.setLabel(theNewName);
	return;
      }
    }
  }

  private boolean IsInWindowsMenu(String theName) {
    return(theName.equals("Stack") || theName.equals("Tile") || theName.equals("Tile Vertical")||theName.equals("jMax Console")||IsAWindowName(theName) || IsAnEditorFrameName(theName));
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

  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// itemListener --inizio
  public void itemStateChanged(ItemEvent e){
    if(e.getItemSelectable() instanceof CheckboxMenuItem ){
      CheckboxMenuItem aCheckItem = (CheckboxMenuItem)e.getItemSelectable();
      String itemName = aCheckItem.getLabel();
      if (IsInFileMenu(itemName)) FileMenuAction(aCheckItem, itemName);
    }
  }
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// itemListener --fine

  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// actionListener --inizio

  public void actionPerformed(ActionEvent e){
    if(e.getSource() instanceof MenuItem ){
      MenuItem aMenuItem = (MenuItem)e.getSource();
      String itemName = aMenuItem.getLabel();
    
      if (IsInFileMenu(itemName)) FileMenuAction(aMenuItem, itemName);
      if (IsInEditMenu(itemName)) EditMenuAction(aMenuItem, itemName);
      if (IsInProjectMenu(itemName)) ProjectMenuAction(aMenuItem, itemName);
      if (IsInWindowsMenu(itemName)) WindowsMenuAction(aMenuItem, itemName);
    }
  }

  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// actionListener --fine
  
  public void New(){
     String aNewFileType;
     ProjectNewDialog aNewDialog = new ProjectNewDialog(this);
     Point aPoint = getLocation();
     aNewDialog.setLocation(aPoint.x+100, aPoint.y+100);
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
	      MaxApplication.getTclInterp().evalFile(theDirectory+theFileName);
	}
	catch (Exception e) {
	System.out.println("error while opening .tpa"+theDirectory+theFileName+" "+e.toString());
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
	System.out.println("editor not found:" + aResId.preferred_resource_handler); 
	return false;
      }
      catch(IllegalAccessException k) {
	System.out.println("cannot execute " + aResId.preferred_resource_handler + " illegal access - is this \"public\"?");
	return false;
      }
      catch(InstantiationException k) {
	System.out.println("cannot execute " + aResId.preferred_resource_handler + ": instantiation error");
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
	aDialog.setLocation(100, 100);
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
    else if (theString.equals("jMax Console")) {
      MaxApplication.getApplication().GetConsoleWindow().ToFront();
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

  public void focusGained(FocusEvent e){
  }

  public void focusLost(FocusEvent e){
  }
  /////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////keyListener --inizio


  public void keyTyped(KeyEvent e){}
  public void keyReleased(KeyEvent e){}

  public void keyPressed(KeyEvent e){
    int aInt = e.getKeyCode();
    if (e.isControlDown()){
      if(aInt == 78) New();//n
      else if(aInt == 79) Open();//o
      else if(aInt == 80) MaxApplication.getApplication().ObeyCommand(MaxApplication.PRINT_WINDOW);//p
      else if(aInt == 81) MaxApplication.getApplication().ObeyCommand(MaxApplication.QUIT_APPLICATION);//q
      else if(aInt == 83){//s
	if(MaxApplication.getApplication().itsWindow != null) 
	  MaxApplication.getApplication().itsSketchWindow.GetDocument().Save();
      }
      else if(aInt == 87) MaxApplication.getApplication().ObeyCommand(MaxApplication.CLOSE_WINDOW);//w
    }
    else{
      if(aInt == ircam.jmax.utils.Platform.RETURN_KEY) {
	if(itsProject.itsCurrentEntry.itsDocument==null)
	  if(itsProject.itsCurrentEntry.OpenEntryDocument()) return;
      }
      else if(aInt == Event.UP) itsProject.SelectPreviousEntry();
      else if(aInt == Event.DOWN) itsProject.SelectNextEntry();
    }
  }
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// keyListener --fine

  ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////WindowListener --inizio  
  public void windowClosing(WindowEvent e){
    if (inAnApplet) {
      dispose();
    } 
    else {
      dispose();
      Runtime.getRuntime().exit(0);
    }
  }

  public void windowOpened(WindowEvent e){}
  public void windowClosed(WindowEvent e){}
  public void windowIconified(WindowEvent e){}       
  public void windowDeiconified(WindowEvent e){}
  public void windowActivated(WindowEvent e){
    requestFocus();}
  public void windowDeactivated(WindowEvent e){}  

  ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////WindowListener --fine

  ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////MouseListener --inizio
  public void mousePressed(MouseEvent e){
    requestFocus();
  }
  public void mouseClicked(MouseEvent e){}
  public void mouseReleased(MouseEvent e){}
  public void mouseEntered(MouseEvent e){}
  public void mouseExited(MouseEvent e){}
  

  ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////MouseListener --fine
  //--------------------------------------------------------
  //	paint
  //--------------------------------------------------------
  public void paint(Graphics g) {
    Color bg = getBackground();
    g.setColor(bg);
    Dimension d = itsProjectBar.getSize();
    g.draw3DRect(0, 0, d.width+5, d.height+5, true);
  }
    
	
  //--------------------------------------------------------
  // minimumSize()
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
    return new Dimension(300,450); //(depending on the layout manager).
  }

  //--------------------------------------------------------
  // preferredSize()
  //--------------------------------------------------------
  public Dimension getPreferredSize() {
    return getMinimumSize();
  }
}








