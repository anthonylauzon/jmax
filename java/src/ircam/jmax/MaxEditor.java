
package ircam.jmax;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.editors.ermes.*; // @@@ !!
import ircam.jmax.editors.console.*; // @@@ !!
import com.sun.java.swing.*;

/**
 * The abstract base class for all the Ermes editors. It provides utility methods
 * such as the Window menu handling, initialisation, and others.
 */
public abstract class MaxEditor extends JFrame implements KeyListener, FocusListener, WindowListener, ActionListener {
  MaxDataType editedType;

  public Menu itsFileMenu;
  public Menu itsNewFileMenu;
  public Menu itsEditMenu;	
  public Menu itsWindowsMenu;

  Vector itsWindowMenuList;
	
  public MaxEditor(String title, MaxDataType type)
  {
    super(title);
    
    editedType = type;
    MaxWindowManager.getWindowManager().addWindow(this);
  }

  public MaxEditor(String title)
  {
    super(title);
    
    editedType = null;
    MaxWindowManager.getWindowManager().addWindow(this);
  }

  
  public MaxEditor(MaxDataType type)
  {
    super("");

    editedType = type;
    MaxWindowManager.getWindowManager().addWindow(this);
  }

  public MaxEditor()
  {
    super("");

    editedType = null;
    MaxWindowManager.getWindowManager().addWindow(this);
  }

  public final void Init()
  {
    itsWindowMenuList = new Vector();
    
    MenuBar mb = new MenuBar();
    itsFileMenu = CreateFileMenu();
    itsEditMenu = CreateEditMenu();
    itsWindowsMenu = CreateWindowsMenu();
    mb.add(itsFileMenu);
    mb.add(itsEditMenu);

    setMenuBar(mb);
    
    addKeyListener(this);
    addWindowListener(this);
    SetupMenu();

    mb.add(itsWindowsMenu);
  }


  /** Use an Action object to do this stuff */

  class NewDataCreator implements ActionListener
  {
    MaxDataType type;

    NewDataCreator(MaxDataType type)
    {
      this.type = type;
    }

    public void actionPerformed(ActionEvent e)
    {
      MaxData data;

      try
	{
	  data = type.newInstance();
	  data.edit();
	}
      catch (MaxDataException ex)
	{
	  new ErrorDialog(MaxEditor.this, "Error " + ex + "while creating new "+ type.getName());
	}
    }
  }

  private MenuItem CreateNewTypeMenu()
  {
    MenuItem newMenu = new MenuItem("New " + editedType.getPrettyName() + " Ctrl+N");

    newMenu.addActionListener(new NewDataCreator(editedType));

    return newMenu;
  }

  private Menu CreateNewFileMenu()
  {
    MenuItem aMenuItem;
    String aString;
    Menu newFileMenu = new Menu("New...");
    
    for (Enumeration e = MaxDataType.getTypes().elements(); e.hasMoreElements();)
      {
	final MaxDataType aDataType = (MaxDataType) e.nextElement();

	if (aDataType.canMakeNewInstance() && aDataType.haveEditorFactory())
	  {
	    aMenuItem = new MenuItem(aDataType.getPrettyName());
	    newFileMenu.add(aMenuItem); 

	    aMenuItem.addActionListener(new NewDataCreator(aDataType));
	  }
      }

    return newFileMenu;
  }
  

  private Menu CreateFileMenu()
  {
    MenuItem aMenuItem;
    CheckboxMenuItem aCheckItem;
    Menu fileMenu = new Menu("File");

    if (editedType != null)
      fileMenu.add(CreateNewTypeMenu());

    itsNewFileMenu = CreateNewFileMenu();

    fileMenu.add(itsNewFileMenu);

    aMenuItem = new MenuItem("Open... Ctrl+O");
    fileMenu.add(aMenuItem);
    aMenuItem.addActionListener(new ActionListener()
				{
				  public  void actionPerformed(ActionEvent e)
				    { Open();}});

    aMenuItem = new MenuItem("Close   Ctrl+W");
    fileMenu.add(aMenuItem);
    aMenuItem.addActionListener(new ActionListener()
				{
				  public  void actionPerformed(ActionEvent e)
				    { Close();}});

    fileMenu.add(new MenuItem("-"));

    aCheckItem = new CheckboxMenuItem("Open with Autorouting");
    aCheckItem.setState(true);
    fileMenu.add(aCheckItem);
    aCheckItem.addItemListener(new ItemListener()
			       {
				 public  void itemStateChanged(ItemEvent e)
				   { MaxApplication.doAutorouting = !MaxApplication.doAutorouting;}});

    fileMenu.add(new MenuItem("-"));

    aMenuItem = new MenuItem("Save  Ctrl+S");
    fileMenu.add(aMenuItem);
    aMenuItem.addActionListener(new ActionListener()
				{
				  public  void actionPerformed(ActionEvent e)
				    { Save();}});


    aMenuItem = new MenuItem("Save As...");
    fileMenu.add(aMenuItem);
    aMenuItem.addActionListener(new ActionListener()
				{
				  public  void actionPerformed(ActionEvent e)
				    { SaveAs();}});

    fileMenu.add(new MenuItem("-"));

    aMenuItem = new MenuItem("Print... Ctrl+P");
    fileMenu.add(aMenuItem);
    aMenuItem.addActionListener(new ActionListener()
				{
				  public  void actionPerformed(ActionEvent e)
				    { Print();}});

    aMenuItem = new MenuItem("System statistics...");
    fileMenu.add(aMenuItem);
    aMenuItem.addActionListener(new ActionListener()
				{
				  public  void actionPerformed(ActionEvent e)
				    {      new StatisticsDialog(MaxEditor.this);}});

    aMenuItem = new MenuItem("Quit    Ctrl+Q");
    fileMenu.add(aMenuItem);
    aMenuItem.addActionListener(new ActionListener()
				{
				  public  void actionPerformed(ActionEvent e)
				    { MaxApplication.Quit();}});

    return fileMenu;
  }

  private Menu CreateWindowsMenu()
  {
    MenuItem aMenuItem;
    Menu windowsMenu = new Menu("Windows");

    aMenuItem = new MenuItem("Stack");
    windowsMenu.add(aMenuItem);
    aMenuItem.addActionListener(new ActionListener()
				{
				  public  void actionPerformed(ActionEvent e)
				    { MaxWindowManager.getWindowManager().StackWindows();}});

    aMenuItem = new MenuItem("Tile");
    windowsMenu.add(aMenuItem);
    aMenuItem.addActionListener(new ActionListener()
				{
				  public  void actionPerformed(ActionEvent e)
				    { MaxWindowManager.getWindowManager().TileWindows();}});

    aMenuItem = new MenuItem("Tile Vertical");
    windowsMenu.add(aMenuItem);
    aMenuItem.addActionListener(new ActionListener()
				{
				  public  void actionPerformed(ActionEvent e)
				    { MaxWindowManager.getWindowManager().TileVerticalWindows();}});

    windowsMenu.add(new MenuItem("-"));

    aMenuItem = new MenuItem("jMax Console  Ctrl+J");
    windowsMenu.add(aMenuItem);
    aMenuItem.addActionListener(new ActionListener()
				{
				  public  void actionPerformed(ActionEvent e)
				    { ConsoleWindow.getConsoleWindow().toFront();}});

    AddWindowItems(windowsMenu);

    return windowsMenu;
  }

  public String GetTitle()
  {
    return getTitle();
  }
  
  private void AddWindowItems(Menu theWindowMenu)
  {
    ErmesSketchWindow aSketchWindow;
    ErmesSketchWindow aSubWindow;
    MaxEditor aWindow;
    MenuItem aMenuItem;
    Menu aMenu;

    if (MaxApplication.itsSketchWindowList != null)
      {
	for (int i=0; i< MaxApplication.itsSketchWindowList.size(); i++)
	  {
	    aSketchWindow= (ErmesSketchWindow) MaxApplication.itsSketchWindowList.elementAt(i);
	    if (!aSketchWindow.isSubPatcher)
	      {
		if (aSketchWindow.itsSubWindowList.size()==0)
		  {
		    theWindowMenu.add(aMenuItem = new MenuItem(aSketchWindow.getTitle()));
		    aMenuItem.addActionListener(this);
		  }
		else
		  {
		    aMenu = new Menu(aSketchWindow.getTitle());
		    aMenu.add(aMenuItem = new MenuItem(aSketchWindow.getTitle()));
		    aMenuItem.addActionListener(this);
		    for (int k=0; k<aSketchWindow.itsSubWindowList.size(); k++ )
		      {
			aSubWindow = (ErmesSketchWindow)aSketchWindow.itsSubWindowList.elementAt(k);
			aMenu.add(aMenuItem = new MenuItem(aSubWindow.getTitle()));
			aMenuItem.addActionListener(this);
		      }
		    theWindowMenu.add(aMenu);
		    itsWindowMenuList.addElement(aMenu);
		  }
	      }
	  }
      }

    if (MaxApplication.itsEditorsFrameList != null)
      {
	for (int j=0; j< MaxApplication.itsEditorsFrameList.size(); j++)
	  {
	    aWindow = (MaxEditor) MaxApplication.itsEditorsFrameList.elementAt(j);
	    if (aWindow!=this)
	      {
		theWindowMenu.add(aMenuItem = new MenuItem(aWindow.GetTitle()));
		aMenuItem.addActionListener(this);
	      }
	  }
      }
  }

  public void AddWindowToMenu(String theName)
  {
    MenuItem aMenuItem;
    itsWindowsMenu.add(aMenuItem = new MenuItem(theName));
    aMenuItem.addActionListener(this);
  }

  public void AddToSubWindowsMenu(String theTopWindowName, String theSubWindowName, boolean theFirstItem)
  {
    MenuItem aItem, aMenuItem;
    Menu aMenu;
    int aIndex = 0;
    if (theFirstItem)
      {
	for (int i=0; i<itsWindowsMenu.getItemCount();i++)
	  {
	    aItem = itsWindowsMenu.getItem(i);
	    if (aItem.getLabel().equals(theTopWindowName))
	      {
		itsWindowsMenu.remove(aItem);
		aIndex = i;
		break;
	      }
	  }
	aMenu = new Menu(theTopWindowName);
	aMenu.add(aMenuItem = new MenuItem(theTopWindowName));
	aMenuItem.addActionListener(this);
	aMenu.add(aMenuItem = new MenuItem(theSubWindowName));
	aMenuItem.addActionListener(this);
	itsWindowsMenu.insert(aMenu, aIndex);
	itsWindowMenuList.addElement(aMenu);
      }
    else
      {
	for (Enumeration e = itsWindowMenuList.elements(); e.hasMoreElements();)
	  {
	    aMenu = (Menu) e.nextElement();
	    if (theTopWindowName.equals(aMenu.getLabel()))
	      {
		aMenu.add(aMenuItem = new MenuItem(theSubWindowName));
		aMenuItem.addActionListener(this);
		return;
	      }
	  }
      }
  }

  public void RemoveFromSubWindowsMenu(String theTopWindowName,String theSubWindowName,boolean theLastItem)
  {
    MenuItem aItem;
    MenuItem aItem1;
    Menu aMenu;
    int aIndex = 0;
    if (theLastItem)
      {
	for (int i=0; i<itsWindowsMenu.getItemCount();i++)
	  {
	    aItem = itsWindowsMenu.getItem(i);
	    if (aItem.getLabel().equals(theTopWindowName))
	      {
		itsWindowsMenu.remove(aItem);
		itsWindowMenuList.removeElement(aItem);
		aIndex = i;
		break;
	      }
	  }
	aItem = new MenuItem(theTopWindowName);
	itsWindowsMenu.insert(aItem, aIndex);
	aItem.addActionListener(this);
      }
    else
      {
	for (Enumeration e = itsWindowMenuList.elements(); e.hasMoreElements();)
	  {
	    aMenu = (Menu) e.nextElement();
	    if (theTopWindowName.equals(aMenu.getLabel()))
	      {
		for (int j=0; j<aMenu.getItemCount();j++)
		  {
		    aItem1 = aMenu.getItem(j);
		    if (aItem1.getLabel().equals(theSubWindowName))
		      {
			aMenu.remove(aItem1);
			return;
		      }
		  }
		return;
	      }
	  }
      }
  }

  public void RemoveWindowFromMenu(String theName)
  {
    MenuItem aItem;
    for (int i=0; i<itsWindowsMenu.getItemCount();i++)
      {
	aItem = itsWindowsMenu.getItem(i);
	if (aItem.getLabel().equals(theName))
	  {
	    itsWindowsMenu.remove(aItem);
	    return;
	  }
      }
  }

  public void ChangeWinNameMenu(String theOldName, String theNewName)
  {
    MenuItem aItem;
    for (int i=0; i<itsWindowsMenu.getItemCount();i++)
      {
	aItem = itsWindowsMenu.getItem(i);
	if (aItem.getLabel().equals(theOldName))
	  {
	    aItem.setLabel(theNewName);
	    return;
	  }
      }
  }


  protected Menu CreateEditMenu()
  {
    MenuItem aMenuItem;
    Menu editMenu = new Menu("Edit");

    aMenuItem = new MenuItem("Cut  Ctrl+X");
    editMenu.add(aMenuItem);
    aMenuItem.addActionListener(new ActionListener()
				{
				  public  void actionPerformed(ActionEvent e)
				    { Cut();}});

    aMenuItem = new MenuItem("Copy  Ctrl+C");
    editMenu.add(aMenuItem);
    aMenuItem.addActionListener(new ActionListener()
				{
				  public  void actionPerformed(ActionEvent e)
				    { Copy();}});

    aMenuItem = new MenuItem("Paste  Ctrl+V");
    editMenu.add(aMenuItem);
    aMenuItem.addActionListener(new ActionListener()
				{
				  public  void actionPerformed(ActionEvent e)
				    { Paste();}});

    aMenuItem = new MenuItem("Clear"); 
    editMenu.add(aMenuItem);
    aMenuItem.addActionListener(new ActionListener()
				{
				  public  void actionPerformed(ActionEvent e)
				    { Clear();}});

    return editMenu;
  }


  private boolean IsInWindowsMenu(String theName)
  {
    return(IsAWindowName(theName)|| IsAnEditorFrameName(theName));
  }
  
  private boolean IsAWindowName(String theName)
  {
    ErmesSketchWindow aSketchWindow; 
    for (int i=0; i< MaxApplication.itsSketchWindowList.size(); i++)
      {
	aSketchWindow = (ErmesSketchWindow) MaxApplication.itsSketchWindowList.elementAt(i);
	if (aSketchWindow.getTitle().equals(theName)) return true;
      }
    return false;
  }
  
  private boolean IsAnEditorFrameName(String theName)
  {
    MaxEditor aWindow; 
    for (int i=0; i< MaxApplication.itsEditorsFrameList.size(); i++)
      {
	aWindow = (MaxEditor)MaxApplication.itsEditorsFrameList.elementAt(i);
	if (aWindow.GetTitle().equals(theName)) return true;
      }
    return false;
  } 
  
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// actionListener --inizio
  public void actionPerformed(ActionEvent e)
  {
    if (e.getSource() instanceof MenuItem )
      {
	MenuItem aMenuItem = (MenuItem)e.getSource();
	String itemName = aMenuItem.getLabel();
    
	if (IsInWindowsMenu(itemName)) WindowsMenuAction(aMenuItem, itemName);
      }
  }

  public void Print()
  {}
  public void Save()
  {}//override this function if you want to save your content
  public void SaveAs(){}//override this function if you want to save your content
  public boolean ShouldSave(){return false;}//override this function if your data changed
  public void Close()
  {
    // Removed the code that save the file, because at the 
    // MaxEditor level we *don't* know if we are editing 
    // a file or other stuff; the correct thing to do 
    // is to overwrite the method in the subclass and
    // inside the redefined close do something like:
    //  
    //    < ask to save the file if we have a file ...>
    //    super.Close()
    //
    
    MaxApplication.RemoveThisWindowFromMenus(this);
    MaxApplication.itsEditorsFrameList.removeElement(this);
    setVisible(false);
    dispose();
  }

  protected void Cut(){};
  protected void Copy(){};
  protected void Paste(){};
  protected void Clear(){};
  
  private void WindowsMenuAction(MenuItem theMenuItem, String theName)
  {
    ErmesSketchWindow aSketchWindow;
    MaxEditor aWindow;
    for (int i=0; i< MaxApplication.itsSketchWindowList.size(); i++)
      {
	aSketchWindow = (ErmesSketchWindow) MaxApplication.itsSketchWindowList.elementAt(i);
	if (aSketchWindow.getTitle().equals(theName))
	  {
	    aSketchWindow.toFront();
	    return;
	  }
      }
    for (int j=0; j< MaxApplication.itsEditorsFrameList.size(); j++)
      {
	aWindow = (MaxEditor) MaxApplication.itsEditorsFrameList.elementAt(j);
	if (aWindow.GetTitle().equals(theName))
	  {
	    aWindow.toFront();
	    return;
	  }
      }
  }

  ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////focusListener --inizio
  public void focusGained(FocusEvent e)
  {
    MaxApplication.SetCurrentWindow(this);
  }
  public void focusLost(FocusEvent e){}

  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////focusListener --fine
  ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////WindowListener --inizio  
  public void windowClosing(WindowEvent e)
  {
    Close();
  }
  public void windowOpened(WindowEvent e){}
  public void windowClosed(WindowEvent e){}
  public void windowIconified(WindowEvent e){}       
  public void windowDeiconified(WindowEvent e){}
  public void windowActivated(WindowEvent e)
  {
    requestFocus();
  }
  public void windowDeactivated(WindowEvent e){}  

  ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////WindowListener --fine

  /** Use an Action object to do this stuff */

  public void Open()
  {
    MaxDataSource source = MaxFileChooser.chooseFileToOpen(this, "Open File");

    if (source != null)
      {
	try
	  {
	    MaxData data;

	    data = MaxDataHandler.loadDataInstance(source);
	
	    try
	      {
		data.edit();
	      }
	    catch (MaxDataException e)
	      {
		// Ignore MaxDataException exception in running the editor
		// May be an hack, may be is ok; move this stuff to an action
		// handler !!
	      }
	  }
	catch (MaxDataException e)
	  {
	    new ErrorDialog(this, "Error " + e + "while opening "+ source);
	  }
      }
  }


  public Frame GetFrame()
  {
    return this;
  }
  
  
  public abstract void SetupMenu();

  /////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////keyListener --inizio  
  public void keyTyped(KeyEvent e)
  {}
  public void keyReleased(KeyEvent e)
  {}

  public void keyPressed(KeyEvent e)
  {
    int aInt = e.getKeyCode();
    if (e.isControlDown())
      {
	if (aInt == 74) ConsoleWindow.getConsoleWindow().toFront();//j
	else if (aInt == 67) Copy();//c
	else if (aInt == 78)
	  {
	    // Ctrl-N always create a new patcher

	    if (editedType != null)
	      {
		try
		  {
		    editedType.newInstance().edit();
		  }
		catch (MaxDataException ex)
		  {
		    // Ingnore exceptions here
		  }
	      }
	  }
	else if (aInt == 79) Open();//o
	else if (aInt == 80) Print();//p
	else if (aInt == 81) MaxApplication.Quit(); //q
	else if (aInt == 83) Save();//s
	else if (aInt == 86) Paste();//v
	else if (aInt == 87) Close();//w
	else if (aInt == 88) Cut();//x
      }
  }
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// keyListener --fine

  public Menu GetFileMenu()
  {
    return itsFileMenu;
  }
  
  public Menu GetEditMenu()
  {
    return itsEditMenu;
  }

  public MenuItem GetNewMenu()
  {
    if (editedType != null)
      return itsFileMenu.getItem(1);
    else
      return itsFileMenu.getItem(0);
  }

  public MenuItem GetOpenMenu()
  {
    if (editedType != null)
      return itsFileMenu.getItem(2);
    else
      return itsFileMenu.getItem(1);
  }

  public MenuItem GetCloseMenu()
  {
    if (editedType != null)
      return itsFileMenu.getItem(3);
    else
      return itsFileMenu.getItem(2);
  }

  public MenuItem GetOpenWithAutoroutingMenu()
  {
    if (editedType != null)
      return itsFileMenu.getItem(4);
    else
      return itsFileMenu.getItem(3);
  }

  public MenuItem GetSaveMenu()
  {
    if (editedType != null)
      return itsFileMenu.getItem(6);
    else
      return itsFileMenu.getItem(5);
  }

  public MenuItem GetSaveAsMenu()
  {
    if (editedType != null)
      return itsFileMenu.getItem(7);
    else
      return itsFileMenu.getItem(6);
  }
  
  public MenuItem GetPrintMenu()
  {
    if (editedType != null)
      return itsFileMenu.getItem(9);
    else
      return itsFileMenu.getItem(8);
  }

  public MenuItem GetSystemStatisticsMenu()
  {
    if (editedType != null)
      return itsFileMenu.getItem(10);
    else
      return itsFileMenu.getItem(9);
  }

  public MenuItem GetQuitMenu()
  {
    if (editedType != null)
      return itsFileMenu.getItem(11);
    else
      return itsFileMenu.getItem(10);
  }

  public MenuItem GetCutMenu()
  {
    return itsEditMenu.getItem(0);
  }
  
  public MenuItem GetCopyMenu()
  {
    return itsEditMenu.getItem(1);
  }
  
  public MenuItem GetPasteMenu()
  {
    return itsEditMenu.getItem(2);
  }
  
  public MenuItem GetClearMenu()
  {
    return itsEditMenu.getItem(3);
  }
}










