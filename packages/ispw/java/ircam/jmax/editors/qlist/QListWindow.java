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

package ircam.jmax.editors.qlist;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.ispw.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.editors.qlist.menus.*;

/** 
 * The qlist editor. It uses a QListPanel to show its data and
 * a couple of buttons (set, get) to synchronize with FTS.
 */
public class QListWindow extends JFrame implements EditorContainer {
  
  QListPanel itsQListPanel;
  Dimension preferred = new Dimension(512,412);

  private FileMenu itsFileMenu;
  private EditMenu itsEditMenu;	
  private JMenu itsWindowsMenu;
  
  /**
   * Constructor with a FtsAtomList. It prepares the graphic components
   * of the window.*/
  public QListWindow(FtsQListObject obj) 
  {
    super();

    MaxWindowManager.getWindowManager().addWindow(this);
    makeTitle(obj.getAtomList());
    getContentPane().setLayout( new BorderLayout());

    // Build The Menus and Menu Bar
    makeMenuBar();

    addWindowListener(new WindowListener(){
       public void windowOpened(WindowEvent e){}
       public void windowClosed(WindowEvent e){}
       public void windowClosing(WindowEvent e)
	 {
	     MaxWindowManager.getWindowManager().removeWindow(getFrame());
	 }
       public void windowDeiconified(WindowEvent e){}
       public void windowIconified(WindowEvent e){}
       public void windowActivated(WindowEvent e){}
       public void windowDeactivated(WindowEvent e){}
    });
    // prepare the QListPanel
    itsQListPanel = new QListPanel(this, obj);
    
    getContentPane().add( BorderLayout.CENTER, itsQListPanel);

    validate();
    setBounds(100, 100, getPreferredSize().width,getPreferredSize().height);
    setVisible(true);
  }

  private final void makeTitle(FtsAtomList list){
    setTitle(MaxWindowManager.getWindowManager().makeUniqueWindowTitle("Qlist "+list.getName()));
    MaxWindowManager.getWindowManager().windowChanged(this);
  } 

  private final void makeMenuBar(){
    JMenuBar mb = new JMenuBar();

    // Build the file menu
    itsFileMenu = new FileMenu();
    mb.add(itsFileMenu); 
    
    // Build the edit menu
    itsEditMenu = new EditMenu(this); 
    mb.add( itsEditMenu); 
    
    // New Window Manager based Menu
    itsWindowsMenu = new ircam.jmax.toolkit.menus.MaxWindowJMenu("Windows", this); 
    mb.add(itsWindowsMenu);

    setJMenuBar(mb);
  }

  public Dimension getPreferredSize() {
    return preferred;
  }

  public Dimension getMinimumSize() {
    return preferred;
  }

  // ------ editorContainer interface ---------------
  public Editor getEditor(){
    return itsQListPanel;
  }
  public Frame getFrame(){
    return this;
  }
  public Point getContainerLocation(){
    return getLocation();
  }
  public Rectangle getViewRectangle(){
    return getContentPane().getBounds();
  }
}








