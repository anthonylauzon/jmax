package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;
import java.util.*;
import java.io.*;
import java.text.*;//DateFormat...

import tcl.lang.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import ircam.jmax.dialogs.*;
import com.sun.java.swing.*;


/**
 * The window that contains the sketchpad. It knows the ftspatcher it is editing.
 * It handles all the sketch menus, it knows how to load from a ftspatcher.
 */

public class ErmesSketchWindow extends MaxEditor implements MaxDataEditor, FtsPropertyHandler, ComponentListener{

  public void propertyChanged(FtsObject object, String name, Object value)
  {
    //    System.err.println("cambiata la proprieta' "+name);
    if (name.equals("ins"))
      {
	// A patcher has been redefined
	
	itsSketchPad.RedefineInChoice();
      }
    else if (name.equals("outs"))
      {
	// A patcher has been redefined
	
	itsSketchPad.RedefineOutChoice();
      }
    else if (name.equals("newObject"))
      ftsObjectsPasted.addElement(value);
    else if (name.equals("newConnection"))
      ftsConnectionsPasted.addElement(value);
    else if (name.equals("deletedObject")) {
      // just an hack: remove the watch temporarly, add it just after
      // to avoid recursion
      itsPatcher.removeWatch("deletedObject", this);
      itsSketchPad.itsHelper.DeleteGraphicObject((ErmesObject)(((FtsObject)value).getRepresentation()), false);
      itsPatcher.watch("deletedObject", this);
    }
    else if (name.equals("deleteConnection")) {
      itsPatcher.removeWatch("deleteConnection", this);
      ErmesObject objFrom = (ErmesObject) (((FtsConnection)value).getFrom()).getRepresentation();
      int outletFrom = ((FtsConnection)value).getFromOutlet();
      ErmesObject objTo = (ErmesObject) (((FtsConnection)value).getTo()).getRepresentation();
      int inletTo = ((FtsConnection)value).getToInlet();
      itsSketchPad.itsHelper.DeleteConnectionByInOut(objFrom, outletFrom, objTo, inletTo, false);
      itsPatcher.watch("deleteConnection", this);
    }
    if (!pasting) itsSketchPad.paintDirtyList();
  }

  public void componentResized(ComponentEvent e) {
    if (itsPatcher == null) System.err.println("internal warning: patcher resized while FtsPatcher is null");     
    else {
      itsPatcher.put("ww", getSize().width-horizontalOffset());
      itsPatcher.put("wh", getSize().height-verticalOffset());
    }
  }
  public void componentMoved(ComponentEvent e) {
    if (itsPatcher == null) System.err.println("internal warning: patcher moved while FtsPatcher is null");    
    else {
      itsPatcher.put("wx", getLocation().x);
      itsPatcher.put("wy", getLocation().y);
    }
  }
  public void componentShown(ComponentEvent e) {}
  public void componentHidden(ComponentEvent e){}  

  
  FtsSelection itsSelection;
  Vector ftsObjectsPasted = new Vector();
  Vector ftsConnectionsPasted = new Vector();
  boolean pasting = false;
  public static ErmesClipboardProvider itsClipboardProvider = new ErmesClipboardProvider();
  //  public ErmesObject itsOwner;//in case this is a subpatcher
  public boolean isAbstraction = false;
  final String FILEDIALOGMENUITEM = "File dialog...";
  public static int preferredWidth = 490;
  public static int preferredHeight = 450;
  Dimension preferredsize = new Dimension(preferredWidth,preferredHeight);
  public ErmesSketchPad itsSketchPad = new ErmesSketchPad(this);
  ErmesScrollerView itsScrollerView = new ErmesScrollerView(this, itsSketchPad);
  ErmesSwToolbar itsToolBar = new ErmesSwToolbar(itsSketchPad);
  static String[] itsFontList = Toolkit.getDefaultToolkit().getFontList();
  ErmesSwVarEdit itsVarEdit;//created when we need a variable editor (abstractions)!
  public FtsContainerObject itsPatcher;
  private Menu itsJustificationMenu;
  private Menu itsResizeObjectMenu;
  private Menu itsAlignObjectMenu;
  private Menu itsTextMenu;	
  private Menu itsSizesMenu;	
  private Menu itsFontsMenu;
  private Menu itsExecutionMenu;
  private Menu itsGraphicsMenu;
  CheckboxMenuItem itsSelectedSizeMenu;//the Selected objects size MenuItem
  CheckboxMenuItem itsSketchSizeMenu;//the SketchPad size MenuItem
  CheckboxMenuItem itsSketchFontMenu;//the SketchPad font MenuItem
  CheckboxMenuItem itsSelectedFontMenu;//the Selected objects font MenuItem
  CheckboxMenuItem itsSelectedJustificationMenu;
  CheckboxMenuItem itsSketchJustificationMenu;
  CheckboxMenuItem itsCurrentResizeMenu;
  CheckboxMenuItem itsAutoroutingMenu;
  MenuItem itsRunModeMenuItem;
  MenuItem itsSelectAllMenuItem;
  boolean itsClosing = false;
  boolean itsChangingRunEditMode = false;
  //public String itsTitle;
  public MaxDocument itsDocument;

  // the MaxDataEditor interface: Please implement me :->


  public void reEdit() {
    setVisible(true);
    toFront();
    //ErmesSketchPad.RequestOffScreen(itsSketchPad);
  }


  public void quitEdit() {
    /* No confirmation should be asked here !!! */

    itsPatcher.close();
    Destroy();
  }

  /** Tell the editor to syncronize, i.e. to store in the
   * data all the information possibly cached in the editor
   * and still not passed to the data instance; this usually
   * happen before saving an instance.
   */

  public void syncData()
  {
    //2705 CreateFtsGraphics(this);
  }

  /** Tell the editor the data has changed; it pass a sigle Java
   * Object that may code what is changed and where; if the argument
   * is null, means usually that all the data is changed
   */
  public void dataChanged(Object reason){}

  //end of the MaxDataEditor interface


  //----------alternative contructors:
  /**
   * constructor 
   * This is the only constructor actually called
   */
  public ErmesSketchWindow(FtsContainerObject patcher) {
    super(Mda.getDocumentTypeByName("patcher"));

    itsDocument = patcher.getDocument();
    itsPatcher = patcher;

    if (itsDocument.getRootData() == getData())
      setTitle(itsDocument.getName());
    else
      setTitle(MaxWindowManager.getWindowManager().makeUniqueWindowTitle(chooseWindowName(patcher)));

    itsSketchPad.setFont(new Font(ircam.jmax.utils.Platform.FONT_NAME, Font.PLAIN, ircam.jmax.utils.Platform.FONT_SIZE));						// communicate with
    Init(); //MaxEditor base class init (standard menu handling)
    isAbstraction = false;
    itsSketchPad.SetToolBar(itsToolBar);	// inform the Sketch of the ToolBar to 

    InitSketchWin();
    validate();
    itsPatcher.open();
    InitFromContainer(itsPatcher);
    setVisible(true);

    itsSketchPad.PrepareInChoice();//???????
    itsSketchPad.PrepareOutChoice();///?????????

    itsPatcher.watch("ins", this);
    itsPatcher.watch("outs", this);
    addComponentListener(this);
  }

  // For the MaxDataEditor interface
  /**
   * constructor from a MaxDocument AND a ftsContainer AND a father window (subpatchers editors)
   * added better window titles for patchers.
   */

  public MaxData getData()
  {
    return (MaxData) itsPatcher;
  }

  static String chooseWindowName(FtsContainerObject theFtsPatcher)
  {
    if (theFtsPatcher instanceof FtsPatcherObject)
      return "patcher " + theFtsPatcher.getObjectName();
    else
      return theFtsPatcher.getClassName();
  }

    //--------------------------------------------------------
    //	CONSTRUCTOR
    //
    //--------------------------------------------------------    
  public ErmesSketchWindow(boolean theIsSubPatcher, ErmesSketchWindow theTopWindow, boolean theIsAbstraction)
  {
    super(Mda.getDocumentTypeByName("patcher"));
    isAbstraction = theIsAbstraction;
    itsSketchPad.SetToolBar(itsToolBar);	// inform the Sketch of the ToolBar to 
    itsSketchPad.setFont(new Font(ircam.jmax.utils.Platform.FONT_NAME, Font.PLAIN, ircam.jmax.utils.Platform.FONT_SIZE));						// communicate with
      
    InitSketchWin();
    validate();
  }

  int horizontalOffset() {
    //sketchPad is not there yet, and we have no time for the release.
    //    return 20+itsSketchPad.getLocation().x; //the size of the sketch
    return 40;
  }
  
  int verticalOffset() {
    //    return itsSketchPad.getLocation().y;//the size of the toolbar + menus
    return 130;
  }
  
  public void InitFromContainer(FtsContainerObject patcher) {
    
    Object aObject;
    int x=0;
    int y=0;
    int width=500;
    int height=480;
    Integer x1, y1, width1, height1;
    //String autorouting;
    //double check the existence of the window properties. If there aren't, use defaults
      
      x1 = (Integer) patcher.get("wx");
      if (x1 == null) patcher.put("wx", new Integer(x));
      else  x = x1.intValue();
      y1 = (Integer) patcher.get("wy");
      if (y1 == null) patcher.put("wy", new Integer(y));
      else  y = y1.intValue();
      width1  = (Integer) patcher.get("ww");
      if (width1 == null) patcher.put("ww", new Integer (width));
      else  width = width1.intValue();
      height1 = (Integer) patcher.get("wh");
      if (height1 == null) patcher.put("wh", new Integer(height));
      else  height = height1.intValue();
      
      setBounds(x, y, width+horizontalOffset(), height+verticalOffset());
      validate();
      itsSketchPad.InitFromFtsContainer(patcher);

      validate();
    }

  //--------------------------------------------------------
  // InitSketchWin
  //--------------------------------------------------------
  protected void InitSketchWin(){ 
    
    getContentPane().setLayout(new ErmesToolBarLayout(ErmesToolBarLayout.VERTICAL));
    
    itsSelection = Fts.getSelection();
    itsSelection.clean();
    setSize(new Dimension(600, 300));
    itsToolBar.setSize(600, 30);    
    getContentPane().add(itsToolBar);
    if (isAbstraction ) {
      ErmesSwVarEdit itsVarEdit = new ErmesSwVarEdit(itsSketchPad, 1);///***era 3
      getContentPane().add(itsVarEdit, "stick_both");
    }
    getContentPane().add(itsScrollerView, "fill");
    validate();
  }
  
  public void SetupMenu(){
    
    itsGraphicsMenu = new Menu("Graphics");
    getMenuBar().add(itsGraphicsMenu);
    FillGraphicsMenu(itsGraphicsMenu);
    
    itsTextMenu = new Menu("Text");
    getMenuBar().add(itsTextMenu);
    FillTextMenu(itsTextMenu);
    CheckDefaultSizeFontMenuItem();
    CheckDefaultFontItem();
    
    itsSelectAllMenuItem = new MenuItem("Select All  Ctrl+A");
    GetEditMenu().add(itsSelectAllMenuItem);
    itsSelectAllMenuItem.addActionListener(new ActionListener() {
    public  void actionPerformed(ActionEvent e)
      { GetSketchPad().SelectAll();}});

    GetEditMenu().add(new MenuItem("-"));

    itsResizeObjectMenu =  new Menu("Resize Object");
    GetEditMenu().add(itsResizeObjectMenu);
    FillResizeObjectMenu(itsResizeObjectMenu);

    GetEditMenu().add(new MenuItem("-"));

    itsAlignObjectMenu =  new Menu("Align Objects");
    GetEditMenu().add(itsAlignObjectMenu);
    FillAlignObjectsMenu(itsAlignObjectMenu);
    
    itsExecutionMenu = new Menu("Execution");
    getMenuBar().add(itsExecutionMenu);
    FillExecutionMenu(itsExecutionMenu);

    GetCutMenu().setEnabled(true);
    GetCopyMenu().setEnabled(true);
    GetPasteMenu().setEnabled(true);
    GetClearMenu().setEnabled(false);
  }

  protected void Cut(){
    Cursor temp = getCursor();

    setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
    //2705 CreateFtsGraphics(this);
    itsSketchPad.ftsClipboard.copy(Fts.getSelection());
    MaxApplication.systemClipboard.setContents(itsClipboardProvider, itsClipboardProvider);
    itsSketchPad.itsHelper.DeleteSelected();
    setCursor(temp);
  }

  // clipboard handling
  protected void Copy() {
    Cursor temp = getCursor();

    setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
    //2705 CreateFtsGraphics(this);
    itsSketchPad.ftsClipboard.copy(Fts.getSelection());
    MaxApplication.systemClipboard.setContents(itsClipboardProvider, itsClipboardProvider);
    setCursor(temp);
  }



  protected void Paste() {
    //it does no more use ErmesClipboardProvider

    if(itsSketchPad.itsRunMode) return;

    Cursor temp = getCursor();

    setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));

    ftsObjectsPasted.removeAllElements();
    ftsConnectionsPasted.removeAllElements();
    pasting = true;
    //evaluate the script
    itsPatcher.watch("newObject", this);
    itsPatcher.watch("newConnection", this);

    itsSketchPad.ftsClipboard.paste(itsPatcher);

    itsPatcher.removeWatch("newObject", this);    
    itsPatcher.removeWatch("newConnection", this);    
    pasting = false;
    // make the sketch do the graphic job
    if (!ftsObjectsPasted.isEmpty() || ! ftsConnectionsPasted.isEmpty()) {
      itsSketchPad.PasteObjects(ftsObjectsPasted, ftsConnectionsPasted);
      ErmesSketchPad.RequestOffScreen(itsSketchPad);
      itsSketchPad.repaint();
    }
    setCursor(temp);
  }

  
  private void FillGraphicsMenu(Menu theGraphicsMenu){
    itsAutoroutingMenu = new CheckboxMenuItem("Autorouting", true);
    theGraphicsMenu.add(itsAutoroutingMenu);
    itsAutoroutingMenu.setEnabled(false);//still there, but inactive
    itsAutoroutingMenu.addItemListener(new ItemListener() {
      public  void itemStateChanged(ItemEvent e)
	{ /*SetAutorouting(itsAutoroutingMenu.getState());*/}});
  }


  class ResizeMenuAdapter implements ItemListener
  {
    CheckboxMenuItem item;
    String resize;

    ResizeMenuAdapter(CheckboxMenuItem item,  String resize)
    {
      this.item = item;
      this.resize = resize;
    }

    public  void itemStateChanged(ItemEvent e)
    {
      ResizeObjectMenuAction(item, resize);
    }
  }

  private void FillResizeObjectMenu(Menu theResizeObjectMenu)
  {
    CheckboxMenuItem aCheckItem;

    aCheckItem = new CheckboxMenuItem("Both");
    theResizeObjectMenu.add(aCheckItem);
    aCheckItem.setState(true);
    aCheckItem.addItemListener(new ResizeMenuAdapter(aCheckItem, "Both"));
    itsCurrentResizeMenu = aCheckItem;

    aCheckItem = new CheckboxMenuItem("Horizontal");
    theResizeObjectMenu.add(aCheckItem);
    aCheckItem.addItemListener(new ResizeMenuAdapter(aCheckItem, "Both"));

    aCheckItem = new CheckboxMenuItem("Vertical");
    theResizeObjectMenu.add(aCheckItem);
    aCheckItem.addItemListener(new ResizeMenuAdapter(aCheckItem, "Both"));
  }
  
  class AlignMenuAdapter implements ActionListener
  {
    String align;

    AlignMenuAdapter(String align)
    {
      this.align = align;
    }

    public  void actionPerformed(ActionEvent e)
    {
      itsSketchPad.AlignSelectedObjects(align);
    }
  }

  private void FillAlignObjectsMenu(Menu theAlignObjectMenu)
  {
    MenuItem aMenuItem;

    aMenuItem = new MenuItem("Align Top");
    theAlignObjectMenu.add(aMenuItem);
    aMenuItem.addActionListener(new AlignMenuAdapter("Top"));

    aMenuItem = new MenuItem("Align Left");
    theAlignObjectMenu.add(aMenuItem);
    aMenuItem.addActionListener(new AlignMenuAdapter("Left"));

    aMenuItem = new MenuItem("Align Bottom");
    theAlignObjectMenu.add(aMenuItem);
    aMenuItem.addActionListener(new AlignMenuAdapter("Bottom"));

    aMenuItem = new MenuItem("Align Right");
    theAlignObjectMenu.add(aMenuItem);
    aMenuItem.addActionListener(new AlignMenuAdapter("Right"));
  }

  private void FillExecutionMenu(Menu theExecutionMenu){
    itsRunModeMenuItem = new MenuItem("Run Mode Ctrl+E");
    theExecutionMenu.add(itsRunModeMenuItem);

    itsRunModeMenuItem.addActionListener(new ActionListener() {
      public  void actionPerformed(ActionEvent e)
	{
	  setRunMode(! itsSketchPad.itsRunMode);
	}
    });
  }

  private void FillTextMenu(Menu theTextMenu) {
    String aString;
    CheckboxMenuItem aCheckItem;

    //-- fonts
    itsFontsMenu =  new Menu("Fonts");
    FillFontMenu(itsFontsMenu);
    theTextMenu.add(itsFontsMenu);

    theTextMenu.add(new MenuItem("-"));

    //-- sizes
    itsSizesMenu = new Menu("Sizes");
    FillSizesMenu(itsSizesMenu);
    theTextMenu.add(itsSizesMenu);

    theTextMenu.add(new MenuItem("-"));

    //-- justification
    itsJustificationMenu = new Menu("Justification");
    FillJustificationMenu(itsJustificationMenu);
    theTextMenu.add(itsJustificationMenu);
  }

  class SizesMenuAdapter implements ItemListener
  {
    CheckboxMenuItem item;
    int size;

    SizesMenuAdapter(CheckboxMenuItem item, int size)
    {
      this.item = item;
      this.size = size;
    }

    public  void itemStateChanged(ItemEvent e)
    {
      SizesMenuAction(item, size);
    }
  }

  private void FillSizesMenu(Menu theSizesMenu)
  {
    CheckboxMenuItem aCheckItem;

    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("8"));
    aCheckItem.addItemListener(new SizesMenuAdapter(aCheckItem, 8));
    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("9"));
    aCheckItem.addItemListener(new SizesMenuAdapter(aCheckItem, 9));
    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("10"));
    aCheckItem.addItemListener(new SizesMenuAdapter(aCheckItem, 10));
    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("12"));
    aCheckItem.addItemListener(new SizesMenuAdapter(aCheckItem, 12));
    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("14"));
    aCheckItem.addItemListener(new SizesMenuAdapter(aCheckItem, 14));
    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("18"));
    aCheckItem.addItemListener(new SizesMenuAdapter(aCheckItem, 18));
    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("24"));
    aCheckItem.addItemListener(new SizesMenuAdapter(aCheckItem, 24));
    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("36"));
    aCheckItem.addItemListener(new SizesMenuAdapter(aCheckItem, 36));
    theSizesMenu.add(aCheckItem = new CheckboxMenuItem("48"));
    aCheckItem.addItemListener(new SizesMenuAdapter(aCheckItem, 48));
  }

  class FontMenuAdapter implements ItemListener
  {
    CheckboxMenuItem item;
    String font;

    FontMenuAdapter(CheckboxMenuItem item, String font)
    {
      this.item = item;
      this.font = font;
    }

    public  void itemStateChanged(ItemEvent e)
    {
      FontsMenuAction(item, font);
    }
  }

  private void FillFontMenu(Menu theFontMenu)
  {
    CheckboxMenuItem aCheckItem;
    String aString;

    for(int i = 0;i<itsFontList.length;i++){
      aString = (String) itsFontList[i];
      theFontMenu.add(aCheckItem = new CheckboxMenuItem(aString));
      aCheckItem.addItemListener(new FontMenuAdapter(aCheckItem, aString));
    }
  }


  class JustificationMenuAdapter implements ItemListener
  {
    CheckboxMenuItem item;
    String justification;

    JustificationMenuAdapter(CheckboxMenuItem item,  String justification)
    {
      this.item = item;
      this.justification = justification;
    }

    public  void itemStateChanged(ItemEvent e)
    {
      JustificationMenuAction(item, justification);
    }
  }

  private void FillJustificationMenu(Menu theJustificationMenu)
  {
    CheckboxMenuItem aCheckItem;

    aCheckItem = new CheckboxMenuItem("Left");
    theJustificationMenu.add(aCheckItem);
    aCheckItem.addItemListener(new JustificationMenuAdapter(aCheckItem, "Left"));

    aCheckItem = new CheckboxMenuItem("Center");
    theJustificationMenu.add(aCheckItem);
    aCheckItem.addItemListener(new JustificationMenuAdapter(aCheckItem, "Center"));
    aCheckItem.setState(true);
    itsSelectedJustificationMenu = aCheckItem;
    itsSketchJustificationMenu = itsSelectedJustificationMenu;

    aCheckItem = new CheckboxMenuItem("Right");
    theJustificationMenu.add(aCheckItem);
    aCheckItem.addItemListener(new JustificationMenuAdapter(aCheckItem, "Right"));
  }

  private void CheckDefaultSizeFontMenuItem(){
    CheckboxMenuItem aCheckboxMenuItem;
    String aFontSize = String.valueOf(itsSketchPad.getSketchFontSize());
    for(int i=0; i<9;i++){
      aCheckboxMenuItem = (CheckboxMenuItem)itsSizesMenu.getItem(i);
      if(aCheckboxMenuItem.getLabel().compareTo(aFontSize) == 0){
	itsSketchSizeMenu = aCheckboxMenuItem;
	itsSelectedSizeMenu = itsSketchSizeMenu;
	itsSelectedSizeMenu.setState(true);
	return;
      }
    }
  }
	
  private void CheckDefaultFontItem(){
    CheckboxMenuItem aCheckboxMenuItem;
    String aFont = itsSketchPad.getFont().getName();
    for(int i=0; i<itsFontList.length; i++){
      aCheckboxMenuItem = (CheckboxMenuItem)itsFontsMenu.getItem(i);
      if(aCheckboxMenuItem.getLabel().toLowerCase().compareTo(aFont.toLowerCase()) == 0){
	itsSketchFontMenu = aCheckboxMenuItem;
	itsSelectedFontMenu = aCheckboxMenuItem;
	itsSketchFontMenu.setState(true);
	return;
      }
    }
  }
	
  //--------------------------------------------------------
  //	GetSketchPad
  //	returns the associated ErmesSketchPad
  //--------------------------------------------------------
  public ErmesSketchPad GetSketchPad(){
    return itsSketchPad;
  }
  
  /////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////keyListener --inizio  
  public void keyTyped(KeyEvent e){}

  public void keyReleased(KeyEvent e){
    
  }

  // Modified to use inheritance and call the MaxEditor method
  // for all the standard key bindings
  public void keyPressed(KeyEvent e){
    int aInt = e.getKeyCode();
    
      //arrows first:
      if (isAnArrow(aInt)) {

	//***
	if (e.isShiftDown()) {
	  if (e.isControlDown()) {
	    itsSketchPad.resizeSelection(10, aInt);
	  }
	  else {
	    itsSketchPad.moveSelection(10, aInt);
	  }
	}
	else {
	  if (e.isControlDown()) {
	    if (e.isMetaDown()) {
	      itsSketchPad.alignSizeSelection(aInt);
	    }
	    else {
	      itsSketchPad.resizeSelection(1, aInt);
	    }
	  }
	  else {
	    if (e.isMetaDown()) {
	      //align
	       String where;
	       if (aInt == Platform.LEFT_KEY) where = "Left";
	       else if (aInt == Platform.RIGHT_KEY) where = "Right";
	       else if (aInt == Platform.UP_KEY) where = "Top";
	       else where = "Bottom";
	       itsSketchPad.AlignSelectedObjects(where);
	    }
	    else {
	      itsSketchPad.moveSelection(1, aInt);
	    }
	  }
	}
      }

      /////// end of arrows

      else if(e.isControlDown()){
	
	if(aInt == 65) itsSketchPad.SelectAll();//a
	else if(aInt == 69){//e
	  if (itsSketchPad.GetRunMode()) setRunMode(false);
	  else setRunMode(true);
	  return;
	}
	else if (aInt == 47){//?
	  //ask help for the reference Manual for the selected element...
	  // open one url *only*, because we open only one browser.
	  
	  ErmesObject aObject;
	  String urlToOpen;
	  Interp interp  = MaxApplication.getTclInterp();
	  
	  if (ErmesSketchPad.currentSelection.itsObjects.size() > 0){
	    aObject = (ErmesObject) ErmesSketchPad.currentSelection.itsObjects.elementAt(0);
	    
	    urlToOpen = FtsReferenceURLTable.getReferenceURL(aObject.itsFtsObject);
	    
	    if (urlToOpen != null){
	      try
		{
		  // Call the tcl browse function, with the URL as argument
		  // By default, the tcl browse function do nothing.
		  // if a user installed a browser package, this will
		// show the documentation.
		  
		  
		  
		  interp.eval("browse " + urlToOpen);
		}
	      catch (tcl.lang.TclException et)
		{
		  System.out.println("TCL error executing browse " + urlToOpen + " : " + interp.getResult());
		}
	    }
	  }   
	}
	else super.keyPressed(e);
      } 
      else if((aInt==ircam.jmax.utils.Platform.DELETE_KEY)||(aInt==ircam.jmax.utils.Platform.BACKSPACE_KEY)){
      if(itsSketchPad.GetEditField()!=null){
	if(!itsSketchPad.GetEditField().HasFocus())
	  itsSketchPad.itsHelper.DeleteSelected();
      }
    }
      else if(aInt == 47){//this is a patch to trap the '?'
      //ask help for the selected element...
      ErmesObject aObject = null;
      
      for (Enumeration en = ErmesSketchPad.currentSelection.itsObjects.elements(); en.hasMoreElements();) {
	aObject = (ErmesObject) en.nextElement();
	
	FtsHelpPatchTable.openHelpPatch(aObject.itsFtsObject);
      }
    } 
      else {
      // Finally, if we don't redefine the key, call the superclass method
      // that define the standard things.
      super.keyPressed(e);
    }
  }
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// keyListener --fine

  public static boolean isAnArrow(int code) {
    return (code == Platform.LEFT_KEY ||
	     code == Platform.RIGHT_KEY ||
	     code == Platform.UP_KEY ||
	     code == Platform.DOWN_KEY 
	     );
  }

  public void Close(){
    if (itsDocument.getRootData() != getData()) {
      setVisible(false);
    }
    else {
      if (ShouldSave()) {
	FileNotSavedDialog aDialog = new FileNotSavedDialog(this, itsDocument);
	aDialog.setLocation(300, 300);
	aDialog.setVisible(true);
	if(aDialog.GetNothingToDoFlag()) return;
	if(aDialog.GetToSaveFlag()){
	  Save();
	}
	aDialog.dispose();
      }

      // Just call dispose on the document
      // Mda will indirectly call Destroy,
      // and will close all the other editors

      itsDocument.dispose();
    }
  }

  /** Method to close the editor (do not touch the patcher) */
  void Destroy()
  {
    itsClosing = true;
    itsClosing = false;
    setVisible(false);
    dispose();
    return;
  }

  public boolean ShouldSave() {
    return (itsDocument.getRootData() == getData()) && (! itsDocument.isSaved());
  }

  private boolean SaveBody(){
    setVisible(false);
    setTitle(itsDocument.getDocumentFile().toString()); 
    setVisible(true);
    //2705 CreateFtsGraphics(this);

    try
      {
	itsDocument.setInfo("Saved " + DateFormat.getDateInstance(DateFormat.FULL).format(new Date()));
	itsDocument.save();
      }
    catch (MaxDocumentException e)
      {
	new ErrorDialog(this, e.getMessage());
	return false;
      }

    return true;
  }

  public void Save() {
    // first, tentative implementation:
    // the FILE is constructed now, and the ErmesSketchPad SaveTo method is invoked.
    // we should RECEIVE this FILE, or contruct it when we load this document
		
    // The "canSave" method of a data tell if it can be saved
    // i.e. if it have a document , and if we can write to its document file

    if (itsDocument.canSave())
      SaveBody();
    else
      SaveAs();
  }

  public void SaveAs() {
    String oldTitle = getTitle();
    File file;

    file = MaxFileChooser.chooseFileToSave(this, "Save As", itsDocument.getDocumentFile());

    if (file == null)
      return;
    else
      itsDocument.bindToDocumentFile(file);
    
    SaveBody();
  }


  public void Print(){
    PrintJob aPrintjob = getToolkit().getPrintJob(this, "Printing Patcher", MaxApplication.getProperties());
    if(aPrintjob != null){
      Graphics aPrintGraphics = aPrintjob.getGraphics();
      if(aPrintGraphics != null){
	//aPrintGraphics.setClip(0, 0, 400, 400);
	itsSketchPad.printAll(aPrintGraphics);
	aPrintGraphics.dispose();
      }
      aPrintjob.end();
    }
  }

  private void FontsMenuAction(MenuItem theMenuItem, String theString) {
    
    if(itsSelectedFontMenu!=null) itsSelectedFontMenu.setState(false);
    
    if(ErmesSketchPad.currentSelection.itsObjects.size()==0) itsSketchFontMenu=(CheckboxMenuItem) theMenuItem;
    
    itsSelectedFontMenu = (CheckboxMenuItem) theMenuItem;
    
    if(ErmesSketchPad.currentSelection.itsObjects.size()==0)
      itsSketchPad.ChangeFont(new Font(theString, Font.PLAIN, itsSketchPad.sketchFontSize));
    else itsSketchPad.ChangeNameFont(theString);
    
    itsSelectedFontMenu.setState(true);
  }

  public void DeselectionUpdateMenu(){
    if(itsSelectedFontMenu!=null) itsSelectedFontMenu.setState(false);
    itsSelectedFontMenu = itsSketchFontMenu;
    itsSelectedFontMenu.setState(true);
    if(itsSelectedSizeMenu!=null) itsSelectedSizeMenu.setState(false);
    itsSelectedSizeMenu = itsSketchSizeMenu;
    itsSelectedSizeMenu.setState(true);
    if(itsSelectedJustificationMenu!=null) itsSelectedJustificationMenu.setState(false);
    itsSelectedJustificationMenu = itsSketchJustificationMenu;
    itsSelectedJustificationMenu.setState(true);
    //itsAutoroutingMenu.setState(itsSketchPad.doAutorouting);
    //itsSketchPad.itsSelectionRouting = itsSketchPad.doAutorouting;
  }

  public void SelectionUpdateMenu(String theFont, Integer theSize, Integer theJustification){
    CheckboxMenuItem aCheckItem = null;
    int i;
    if(itsSelectedFontMenu!=null) itsSelectedFontMenu.setState(false);
    if(theFont!=null){
      for(i=0; i<itsFontsMenu.getItemCount(); i++){
	aCheckItem = (CheckboxMenuItem)itsFontsMenu.getItem(i);
	if(aCheckItem.getLabel().toLowerCase().equals(theFont)){
	  itsSelectedFontMenu = aCheckItem;
	  itsSelectedFontMenu.setState(true);
	  break;
	}
      }
    }
    else itsSelectedFontMenu = null;
    
    if(itsSelectedSizeMenu!=null) itsSelectedSizeMenu.setState(false);
    if(theSize!=null){
      for(i=0; i<itsSizesMenu.getItemCount(); i++){
	aCheckItem = (CheckboxMenuItem)itsSizesMenu.getItem(i);
	if(aCheckItem.getLabel().equals(theSize.toString())){
	  itsSelectedSizeMenu = aCheckItem;
	  itsSelectedSizeMenu.setState(true);
	  break;
	}
      }
    }
    else itsSelectedSizeMenu = null;

    if(itsSelectedJustificationMenu!=null) itsSelectedJustificationMenu.setState(false);
    if(theJustification!=null){
      int aJust = theJustification.intValue();
      if(aJust == ErmesSketchPad.CENTER_JUSTIFICATION){
	for(i=0; i<itsJustificationMenu.getItemCount(); i++){
	  aCheckItem = (CheckboxMenuItem)itsJustificationMenu.getItem(i);
	  if(aCheckItem.getLabel().equals("Center")){
	    itsSelectedJustificationMenu = aCheckItem;
	    itsSelectedJustificationMenu.setState(true);
	    break;
	  }
	}
      }
      else if(aJust == ErmesSketchPad.LEFT_JUSTIFICATION){
	for(i=0; i<itsJustificationMenu.getItemCount(); i++){
	  aCheckItem = (CheckboxMenuItem)itsJustificationMenu.getItem(i);
	  if(aCheckItem.getLabel().equals("Left")){
	    itsSelectedJustificationMenu = aCheckItem;
	    itsSelectedJustificationMenu.setState(true);
	    break;
	  }
	}
      }
      else if(aJust == ErmesSketchPad.RIGHT_JUSTIFICATION){
	for(i=0; i<itsJustificationMenu.getItemCount(); i++){
	  aCheckItem = (CheckboxMenuItem)itsJustificationMenu.getItem(i);
	  if(aCheckItem.getLabel().equals("Right")){
	    itsSelectedJustificationMenu = aCheckItem;
	    itsSelectedJustificationMenu.setState(true);
	    break;
	  }
	}
      }
    }
    else itsSelectedJustificationMenu = null;
    
  }


  private void ExecutionMenuAction(MenuItem theMenuItem, String theString) {
    ErmesObject aObject;
    if (theString.equals("Run Mode Ctrl+E")) {
      setRunMode(true);
    }
    else if (theString.equals("Edit Mode Ctrl+E")) {
      setRunMode(false);
    }
  }
  
  private boolean SizesMenuAction(CheckboxMenuItem theMenuItem, int theFontSize) {
    // UI action

    if(itsSelectedSizeMenu != null)
      itsSelectedSizeMenu.setState(false);

    itsSelectedSizeMenu = theMenuItem;
    itsSelectedSizeMenu.setState(true);

    //if we are here, a font size have been choosen from the FONT menu

    if(ErmesSketchPad.currentSelection.itsObjects.size()==0) itsSketchSizeMenu = (CheckboxMenuItem)theMenuItem;
    
    if(ErmesSketchPad.currentSelection.itsObjects.size()==0) {
      itsSketchPad.sketchFontSize = theFontSize;
      itsSketchPad.ChangeFont(new Font(itsSketchPad.sketchFont.getName(), itsSketchPad.sketchFont.getStyle(), theFontSize));
    }
    else itsSketchPad.ChangeSizeFont(theFontSize);
    return true;
  }

  private boolean JustificationMenuAction(MenuItem theMenuItem, String theString) {
    if(itsSelectedJustificationMenu!=null) itsSelectedJustificationMenu.setState(false);
    itsSketchPad.ChangeJustification(theString);
    itsSelectedJustificationMenu = (CheckboxMenuItem)theMenuItem;
    itsSelectedJustificationMenu.setState(true);
    if(ErmesSketchPad.currentSelection.itsObjects.size()==0) itsSketchJustificationMenu = itsSelectedJustificationMenu;
    return true;
  }


  private boolean ResizeObjectMenuAction(MenuItem theMenuItem, String  theString){
    itsCurrentResizeMenu.setState(false);
    itsSketchPad.ChangeResizeMode(theString);
    itsCurrentResizeMenu = (CheckboxMenuItem)theMenuItem;
    itsCurrentResizeMenu.setState(true);
    return true;
  }

	
  ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////focusListener --inizio
  public void focusGained(FocusEvent e){
    if(!itsClosing){
      MaxApplication.setCurrentWindow(this);
      ErmesSketchPad.RequestOffScreen(itsSketchPad);
      if(itsSketchPad.getGraphics()!= null)
	itsSketchPad.update(itsSketchPad.getGraphics());
    }
  } 

  public void focusLost(FocusEvent e){
    
    //?ErmesSketchPad.RequestOffScreen(itsSketchPad);
    itsSketchPad.itsScrolled = false;
  }

  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////focusListener --fine
  
  ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////WindowListener --inizio  

  public void windowActivated(WindowEvent e){
    requestFocus();
    if(!itsClosing){
      MaxApplication.setCurrentWindow(this);
    }
  }

  ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////WindowListener --fine

  //--------------------------------------------------------
  //	paint
  //--------------------------------------------------------
  public void paint(Graphics g) {
    super.paint(g);
    //2203getContentPane().paintComponents(g);
  }

  //--------------------------------------------------------
  //	SetSnapToGrid
  //--------------------------------------------------------
  public void SetSnapToGrid(){
    itsSketchPad.SetSnapToGrid();
  }
  
  private MenuItem getRunModeMenuItem() {
    return itsRunModeMenuItem;
  }

  private MenuItem getSelectAllMenuItem() {
    return itsSelectAllMenuItem;
  }


  public void setRunMode(boolean theRunMode) {
    ErmesObject aObject;

    /** Store the mode in a non persistent, property of 
      the patch, so that subpatcher can use it as their initial mode */
    
    itsPatcher.put("editMode", (theRunMode ? "run" : "edit"));

    itsChangingRunEditMode = true;
    MenuItem aRunEditItem = getRunModeMenuItem();
    MenuItem aSelectAllItem = getSelectAllMenuItem();
    if(theRunMode)  
      setBackground(Color.white);
    else setBackground(ErmesSketchPad.sketchColor);
    
    itsSketchPad.SetRunMode(theRunMode);
    for(Enumeration en1 = itsSketchPad.itsElements.elements(); en1.hasMoreElements();) {
      aObject = (ErmesObject)en1.nextElement();
      aObject.RunModeSetted();
    }
    itsToolBar.setRunMode(theRunMode);
    aSelectAllItem.setEnabled(!theRunMode);

    itsRunModeMenuItem.setLabel(theRunMode ? "Edit Mode Ctrl+E" : "Run Mode Ctrl+E");
    requestFocus();
  }

  //--------------------------------------------------------
  // minimumSize()
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
      return getPreferredSize();//(depending on the layout manager).
    }
  
  //--------------------------------------------------------
  // preferredSize()
  //--------------------------------------------------------
  public Dimension getPreferredSize() {
      return preferredsize;
  }

  /*2705 public void CreateFtsGraphics(ErmesSketchWindow theSketchWindow)
    {
    //create the graphic descriptions for the FtsObjects, before saving them
    ErmesObject aErmesObject = null;
    FtsObject aFObject = null;
    Rectangle aRect = theSketchWindow.getBounds();
    Rectangle aRect1 = theSketchWindow.getContentPane().getBounds();//e.m.1103
    //String ermesInfo = new String();
    theSketchWindow.itsPatcher.put("wx", aRect.x);
    theSketchWindow.itsPatcher.put("wy", aRect.y);
    theSketchWindow.itsPatcher.put("ww", aRect.width-horizontalOffset());//e.m.1103
    theSketchWindow.itsPatcher.put("wh", aRect.height-verticalOffset());//e.m.1103
      
    for (Enumeration e=theSketchWindow.itsSketchPad.itsElements.elements(); e.hasMoreElements();) {
    aErmesObject = (ErmesObject) e.nextElement();
    aFObject = aErmesObject.itsFtsObject;
    if (aFObject == null) continue; //security check!           
    // Set geometrical properties
    
    aFObject.put("x", aErmesObject.getItsX());
    aFObject.put("y", aErmesObject.getItsY());
    aFObject.put("w", aErmesObject.getItsWidth());
    aFObject.put("h", aErmesObject.getItsHeight());
    
    // Set the font properties
    if (!aErmesObject.getFont().getName().equals(theSketchWindow.itsSketchPad.sketchFont.getName()))
    aFObject.put("font", aErmesObject.getFont().getName());
    
    if (aErmesObject.getFont().getSize() != theSketchWindow.itsSketchPad.sketchFont.getSize())
      aFObject.put("fs", aErmesObject.getFont().getSize());
      
      
      // if (aErmesObject.itsJustification != itsSketchPad.itsJustificationMode)
      //aFObject.put("jsf", aErmesObject.itsJustification);
      //moved to putOtherProperties
      
      aErmesObject.putOtherProperties(aFObject);
      }*/
}












