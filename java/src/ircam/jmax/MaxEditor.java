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
import java.util.*;
import java.io.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;
import ircam.jmax.mda.*;
import ircam.jmax.dialogs.*;
import javax.swing.*;

// ^^^^ This class, nested class New something, is not compatible
// ^^^^ With the multiserver; anyway, this class should just go away.

/**
 * The abstract base class for all the Ermes editors. It provides utility methods
 * such as the Window menu handling, initialisation, and others.
 */
public abstract class MaxEditor extends JFrame implements KeyListener, FocusListener, WindowListener
{

  // Subsystem to register things to dispose at editor destroy time

  private MaxVector toDisposeAtDestroy = new MaxVector();

  /** Interface used to handle cleanup */

  public interface Disposable
  {
    public void dispose();
  }

  public void disposeAtDestroy(Disposable d)
  {
    toDisposeAtDestroy.addElement(d);
  }

  private void doDestroyDispose()
  {
    for (int i = 0; i < toDisposeAtDestroy.size(); i++)
      {
	((Disposable) toDisposeAtDestroy.elementAt(i)).dispose();
      }

    toDisposeAtDestroy.removeAllElements();
  }

  /** The MaxActionListener class is used to deal with the
   *  Menu item GC bug; MaxActionListener are version of ActionListener
   * that use the disposeAtDestroy mechanism to cleanup at destroy time.
   * Note that no support for removing
   * this action listener is implemented (but it could, of course).
   */


  public abstract class MaxActionListener implements  ActionListener, Disposable
  {
    MenuItem item;

    public MaxActionListener(MenuItem item)
    {
      this.item = item;
      disposeAtDestroy(this);
    }

    public abstract void actionPerformed(ActionEvent e);

    public void dispose()
    {
      item.removeActionListener(this);
    }
  }


  /** As MaxActionListener, for item listeners */

  /** The MaxItemListener class is used to deal with the
   *  Menu item GC bug; MaxItemListener are version of ItembListener
   * that use the disposeAtDestroy mechanism to cleanup at destroy time.
   * Note that no support for removing
   * this action listener is implemented (but it could, of course).
   */

  public abstract class MaxItemListener implements  ItemListener, Disposable
  {
    CheckboxMenuItem item;

    public MaxItemListener(CheckboxMenuItem item)
    {
      this.item = item;
      disposeAtDestroy(this);
    }

    public abstract void itemStateChanged(ItemEvent e);

    public void dispose()
    {
      item.removeItemListener(this);
    }
  }

  private Menu itsFileMenu;
  private Menu itsEditMenu;	
  private Menu itsWindowsMenu;
  private Menu itsToolsMenu;

  // File Menu Items

  private MenuItem itsOpenMenuItem;
  private MenuItem itsCloseMenuItem;

  private MenuItem itsSaveMenuItem;
  private MenuItem itsSaveAsMenuItem;
  private MenuItem itsSaveToMenuItem;
  private MenuItem itsPrintMenuItem;
  private MenuItem itsStatisticsMenuItem;
  private MenuItem itsQuitMenuItem;

  // Edit Menu Items

  private MenuItem itsCutMenuItem;
  private MenuItem itsCopyMenuItem;
  private MenuItem itsPasteMenuItem;  
  private MenuItem itsDuplicateMenuItem;
  
  private MenuItem itsUndoMenuItem;
  private MenuItem itsRedoMenuItem;

  public MaxEditor(String title, boolean register)
  {
    super(title);

    if (register) 
      MaxWindowManager.getWindowManager().addWindow(this);
  }

  public MaxEditor(String title)
  {
    this(title, true);
  }

  
  public MaxEditor()
  {
    this("", true);
  }

  public final void Init()
  {
    MenuBar mb = new MenuBar();

    itsFileMenu = CreateFileMenu(); 
    mb.add(itsFileMenu); 

    itsEditMenu = CreateEditMenu(); 
    mb.add(itsEditMenu); 

    setMenuBar(mb);
    
    addKeyListener(this);
    addWindowListener(this);
    SetupMenu();

    // New Tool menu 

    itsToolsMenu = new ircam.jmax.toolkit.MaxToolsMenu("Tools"); 
    mb.add(itsToolsMenu);

    // New Window Manager based Menu

    itsWindowsMenu = new ircam.jmax.toolkit.MaxWindowMenu("Windows", this); 
    mb.add(itsWindowsMenu);

    // The script menu
    Menu scriptMenu = MaxApplication.getScriptMenu("all");
    if (scriptMenu != null) {
	mb.add(scriptMenu);
    }
  }

  public void Destroy()
  {
    getMenuBar().remove(itsFileMenu);
    getMenuBar().remove(itsEditMenu);	
    getMenuBar().remove(itsWindowsMenu);
    getMenuBar().remove(itsToolsMenu);

    removeKeyListener(this);
    removeWindowListener(this);
    doDestroyDispose();
    dispose();
  }

  /** Intercept the setTitle method to tell the window manager,
   *  and to automatically generate a unique title.
   */

  public synchronized void setTitle(String title)
  {
    super.setTitle(MaxWindowManager.getWindowManager().makeUniqueWindowTitle(title));

    MaxWindowManager.getWindowManager().windowChanged(this);
  }

  /** Use an Action object to do this stuff */

  class NewDocumentCreator implements ActionListener, Disposable
  {
    MenuItem item;
    MaxDocumentType type;

    NewDocumentCreator(MenuItem item, MaxDocumentType type)
    {
      this.item = item;
      this.type = type;

      disposeAtDestroy(this);
    }

    public void actionPerformed(ActionEvent e)
    {
      MaxDocument document;
      Cursor temp = getCursor();

      try
	{
	  setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
	  document = type.newDocument(MaxApplication.getFts());
	  document.edit();
	  setCursor(temp);
	}
      catch (MaxDocumentException ex)
	{
	  setCursor(temp);
	  new ErrorDialog(MaxEditor.this, ex.toString());
	}
    }

    public void dispose()
    {
      item.removeActionListener(this);
    }
  }

  private void  CreateNewMenuItems(Menu menu)
  {
    MenuItem aMenuItem;
    String aString;
    
    for (Enumeration e = Mda.getDocumentTypes().elements(); e.hasMoreElements();)
      {
	final MaxDocumentType aDocumentType = (MaxDocumentType) e.nextElement();

	if (aDocumentType.canMakeNewDocument())
	  {
	    aMenuItem = new MenuItem("New " + aDocumentType.getPrettyName());
	    menu.add(aMenuItem); 

	    aMenuItem.addActionListener(new NewDocumentCreator(aMenuItem, aDocumentType));
	  }
      }
  }
  

  private Menu CreateFileMenu()
  {
    CheckboxMenuItem aCheckItem;
    Menu fileMenu = new Menu("File");

    CreateNewMenuItems(fileMenu);

    itsOpenMenuItem = new MenuItem("Open...",  new MenuShortcut(KeyEvent.VK_O));
    fileMenu.add(itsOpenMenuItem);

    fileMenu.addSeparator();

    itsOpenMenuItem.addActionListener(new MaxActionListener(itsOpenMenuItem)
				{
				  public  void actionPerformed(ActionEvent e)
				    { Open();}});


    itsSaveMenuItem = new MenuItem("Save", new MenuShortcut(KeyEvent.VK_S));
    fileMenu.add(itsSaveMenuItem);
    itsSaveMenuItem.addActionListener(new MaxActionListener(itsSaveMenuItem)
				{
				  public  void actionPerformed(ActionEvent e)
				    { Save();}});


    itsSaveAsMenuItem = new MenuItem("Save As ...");
    fileMenu.add(itsSaveAsMenuItem);
    itsSaveAsMenuItem.addActionListener(new MaxActionListener(itsSaveAsMenuItem)
				{
				  public  void actionPerformed(ActionEvent e)
				    { SaveAs();}});

    itsSaveToMenuItem = new MenuItem("Save To ...");
    fileMenu.add(itsSaveToMenuItem);
    itsSaveToMenuItem.addActionListener(new MaxActionListener(itsSaveToMenuItem)
				{
				  public  void actionPerformed(ActionEvent e)
				    { SaveTo();}});


    itsCloseMenuItem = new MenuItem("Close", new MenuShortcut(KeyEvent.VK_W));
    fileMenu.add(itsCloseMenuItem);
    itsCloseMenuItem.addActionListener(new MaxActionListener(itsCloseMenuItem)
				{
				  public  void actionPerformed(ActionEvent e)
				    { 
				      Close();}});

    fileMenu.addSeparator();

    itsPrintMenuItem = new MenuItem("Print...", new MenuShortcut(KeyEvent.VK_P));
    fileMenu.add(itsPrintMenuItem);
    itsPrintMenuItem.addActionListener(new MaxActionListener(itsPrintMenuItem)
				{
				  public  void actionPerformed(ActionEvent e)
				    { Print();}});

    itsStatisticsMenuItem = new MenuItem("Statistics...");
    fileMenu.add(itsStatisticsMenuItem);
    itsStatisticsMenuItem.addActionListener(new MaxActionListener(itsStatisticsMenuItem)
						     {
						       public  void actionPerformed(ActionEvent e)
							 {      new StatisticsDialog(MaxEditor.this);}});

    itsQuitMenuItem = new MenuItem("Quit", new MenuShortcut(KeyEvent.VK_Q));
    fileMenu.add(itsQuitMenuItem);
    itsQuitMenuItem.addActionListener(new MaxActionListener(itsQuitMenuItem)
				{
				  public  void actionPerformed(ActionEvent e)
				    { MaxApplication.Quit();}});

    return fileMenu;
  }


  protected Menu CreateEditMenu()
  {
    Menu editMenu = new Menu("Edit");

    itsUndoMenuItem = new MenuItem("Undo", new MenuShortcut(KeyEvent.VK_Z));
    editMenu.add(itsUndoMenuItem);
    
    itsUndoMenuItem.addActionListener(new MaxActionListener(itsUndoMenuItem)
      {
	public void actionPerformed(ActionEvent e)
	  { Undo();}
      });

    itsRedoMenuItem = new MenuItem("Redo", new MenuShortcut(KeyEvent.VK_R));
    editMenu.add(itsRedoMenuItem);

    itsRedoMenuItem.addActionListener(new MaxActionListener(itsRedoMenuItem)
				{
				  public void actionPerformed(ActionEvent e)
				    { Redo();}});

    itsCutMenuItem = new MenuItem("Cut", new MenuShortcut(KeyEvent.VK_X));
    editMenu.add(itsCutMenuItem);
    itsCutMenuItem.addActionListener(new MaxActionListener(itsCutMenuItem)
				{
				  public  void actionPerformed(ActionEvent e)
				    { Cut();}});

    itsCopyMenuItem = new MenuItem("Copy", new MenuShortcut(KeyEvent.VK_C));
    editMenu.add(itsCopyMenuItem);
    itsCopyMenuItem.addActionListener(new MaxActionListener(itsCopyMenuItem)
				{
				  public  void actionPerformed(ActionEvent e)
				    { Copy();}});

    itsPasteMenuItem = new MenuItem("Paste", new MenuShortcut(KeyEvent.VK_V));
    editMenu.add(itsPasteMenuItem);
    itsPasteMenuItem.addActionListener(new MaxActionListener(itsPasteMenuItem)
				{
				  public  void actionPerformed(ActionEvent e)
				    { Paste();}});

    itsDuplicateMenuItem = new MenuItem("Duplicate", new MenuShortcut(KeyEvent.VK_D));
    editMenu.add(itsDuplicateMenuItem);
    itsDuplicateMenuItem.addActionListener( new MaxActionListener(itsDuplicateMenuItem)	{
      public  void actionPerformed(ActionEvent e)
	{ 
	  Duplicate();
	}
    });

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
    Close(true);
  }

  public void Close(boolean doCancel)
  {
    setVisible(false);
  }

  protected void Undo() {};
  protected void Redo() {};
  protected void Cut(){};
  protected void Copy(){};
  protected void Paste(){};
  protected void Duplicate(){};

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
    Close(false);
  }
  public void windowOpened(WindowEvent e)
  {
  }
  public void windowClosed(WindowEvent e)
  {
  }
  public void windowIconified(WindowEvent e)
  {
  }
  public void windowDeiconified(WindowEvent e)
  {
  }
  public void windowActivated(WindowEvent e)
  {
    requestFocus();
  }
  public void windowDeactivated(WindowEvent e)
  {
  }
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
	    document = Mda.loadDocument(MaxApplication.getFts(), file);
	
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
  }

  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// keyListener --fine

  public Menu getFileMenu()
  {
    return itsFileMenu;
  }
  
  public Menu getEditMenu()
  {
    return itsEditMenu;
  }

  public MenuItem getOpenMenu()
  {
    return itsOpenMenuItem;
  }

  public MenuItem getCloseMenu()
  {
    return itsCloseMenuItem;
  }

  public MenuItem getSaveMenu()
  {
    return itsSaveMenuItem;
  }

  public MenuItem getSaveAsMenu()
  {
    return itsSaveAsMenuItem;
  }

  public MenuItem getSaveToMenu()
  {
    return itsSaveToMenuItem;
  }
  
  public MenuItem getPrintMenu()
  {
    return itsPrintMenuItem;
  }

  public MenuItem getStatisticsMenu()
  {
    return itsStatisticsMenuItem;
  }

  public MenuItem getQuitMenu()
  {
    return itsQuitMenuItem;
  }

  public MenuItem getCutMenu()
  {
    return itsCutMenuItem;
  }
  
  public MenuItem getCopyMenu()
  {
    return itsCopyMenuItem;
  }
  
  public MenuItem getPasteMenu()
  {
    return itsPasteMenuItem;
  }  

  public MenuItem getDuplicateMenu()
  {
    return itsDuplicateMenuItem;
  }

  public MenuItem getUndoMenu()
  {
    return itsUndoMenuItem;
  }  

  public MenuItem getRedoMenu()
  {
    return itsRedoMenuItem;
  }
}
