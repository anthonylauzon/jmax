package ircam.jmax;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.dialogs.*;
import com.sun.java.swing.*;

/**
 * The abstract base class for all the Ermes editors. It provides utility methods
 * such as the Window menu handling, initialisation, and others.
 */
public abstract class MaxEditor extends JFrame implements KeyListener, FocusListener, WindowListener
{
  MaxDocumentType editedType;

  public Menu itsFileMenu;
  public Menu itsNewFileMenu;
  public Menu itsEditMenu;	
  public Menu itsWindowsMenu;

  public MaxEditor(String title, MaxDocumentType type)
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

  
  public MaxEditor(MaxDocumentType type)
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
    MenuBar mb = new MenuBar();
    itsFileMenu = CreateFileMenu();
    itsEditMenu = CreateEditMenu();

    mb.add(itsFileMenu);
    mb.add(itsEditMenu);

    setMenuBar(mb);
    
    addKeyListener(this);
    addWindowListener(this);
    SetupMenu();

    // New Window Manager based Menu

    itsWindowsMenu = new ircam.jmax.toolkit.MaxWindowMenu("Windows", this);
    mb.add(itsWindowsMenu);
  }

  /** Intercept the setTitle method to tell the window manager */

  public synchronized void setTitle(String title)
  {
    super.setTitle(title);

    MaxWindowManager.getWindowManager().windowChanged(this);
  }

  /** Use an Action object to do this stuff */

  class NewDocumentCreator implements ActionListener
  {
    MaxDocumentType type;

    NewDocumentCreator(MaxDocumentType type)
    {
      this.type = type;
    }

    public void actionPerformed(ActionEvent e)
    {
      MaxDocument document;
      Cursor temp = getCursor();

      try
	{
	  setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
	  document = type.newDocument();
	  document.edit();
	  setCursor(temp);
	}
      catch (MaxDocumentException ex)
	{
	  setCursor(temp);
	  new ErrorDialog(MaxEditor.this, ex.toString());
	}
    }
  }

  private MenuItem CreateNewTypeMenu()
  {
    MenuItem newMenu = new MenuItem("New " + editedType.getPrettyName() + " Ctrl+N");

    newMenu.addActionListener(new NewDocumentCreator(editedType));

    return newMenu;
  }

  private Menu CreateNewFileMenu()
  {
    MenuItem aMenuItem;
    String aString;
    Menu newFileMenu = new Menu("New...");
    
    for (Enumeration e = Mda.getDocumentTypes().elements(); e.hasMoreElements();)
      {
	final MaxDocumentType aDocumentType = (MaxDocumentType) e.nextElement();

	if (aDocumentType.canMakeNewDocument())
	  {
	    aMenuItem = new MenuItem(aDocumentType.getPrettyName());
	    newFileMenu.add(aMenuItem); 

	    aMenuItem.addActionListener(new NewDocumentCreator(aDocumentType));
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
				    { 
				      Close();}});

    fileMenu.add(new MenuItem("-"));

    aMenuItem = new MenuItem("Save  Ctrl+S");
    fileMenu.add(aMenuItem);
    aMenuItem.addActionListener(new ActionListener()
				{
				  public  void actionPerformed(ActionEvent e)
				    { Save();}});


    aMenuItem = new MenuItem("Save As ...");
    fileMenu.add(aMenuItem);
    aMenuItem.addActionListener(new ActionListener()
				{
				  public  void actionPerformed(ActionEvent e)
				    { SaveAs();}});

    aMenuItem = new MenuItem("Save To ...");
    fileMenu.add(aMenuItem);
    aMenuItem.addActionListener(new ActionListener()
				{
				  public  void actionPerformed(ActionEvent e)
				    { SaveTo();}});

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


  public void Print()
  {}

  public void Save()
  {}//override this function if you want to save your content

  public void SaveAs()
  {}//override this function if you want to save your content 

  public void SaveTo(){}//override this function if you want to save your content to a file

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
    setVisible(false);
  }

  protected void Cut(){};
  protected void Copy(){};
  protected void Paste(){};
  protected void Clear(){};

  protected void Find()
  {
  }

  protected void FindAgain()
  {
  }
  
  ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////focusListener --inizio
  public void focusGained(FocusEvent e)
  {
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
    File file = MaxFileChooser.chooseFileToOpen(this);

    if (file != null)
      {
	Cursor temp = getCursor();

	try
	  {
	    MaxDocument document;

	    setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
	    document = Mda.loadDocument(file);
	
	    try
	      {
		if (document.getDocumentType().isEditable())
		  document.edit();
	      }
	    catch (MaxDocumentException e)
	      {
		// Ignore MaxDocumentException exception in running the editor
		// May be an hack, may be is ok; move this stuff to an action
		// handler !!
	      }

	    setCursor(temp);
	  }
	catch (MaxDocumentException e)
	  {
	    setCursor(temp);
	    new ErrorDialog(this, e.toString());
	  }
      }
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
	if (aInt == 67) Copy();//c
	else if (aInt == 78)
	  {
	    // Ctrl-N always create a new patcher

	    if (editedType != null)
	      {
		Cursor temp = getCursor();

		setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));

		try
		  {
		    editedType.newDocument().edit();
		  }
		catch (MaxDocumentException ex)
		  {
		    // Ingnore exceptions here
		  }

		setCursor(temp);
	      }
	  }
	else if (aInt == 79) Open();//o
	else if (aInt == 80) Print();//p
	else if (aInt == 81) MaxApplication.Quit(); //q
	else if (aInt == 83) Save();//s
	else if (aInt == 86) Paste();//v
	else if (aInt == 87) Close();//w
	else if (aInt == 88) Cut();//x
	else if (aInt == 70) Find(); //F
	else if (aInt == 71) FindAgain(); //G
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
