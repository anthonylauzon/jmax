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

package ircam.jmax.editors.table;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.toolkit.*;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

import ircam.jmax.editors.table.menus.*;

/** 
 * The table editor
 */
public class Tabler extends JFrame implements EditorContainer {
  
  //--- Fields 
  TablePanel itsPanel;
  static EditorToolbar toolbar; 

  private JMenu itsFileMenu;
  private EditMenu itsEditMenu;	
  private ViewMenu itsViewMenu;
  private JMenu itsWindowsMenu;
  private JMenu itsToolsMenu;

  /** Constructor */
  public Tabler(FtsIntegerVector theData) {
    super();

    MaxWindowManager.getWindowManager().addWindow(this);
    
    // Make the title
    makeTitle(theData);

    itsPanel = new TablePanel(this, new TableRemoteData(theData));
    
    // Build The Menus and Menu Bar
    makeMenuBar();

    getContentPane().add(itsPanel);

    itsPanel.frameAvailable(); 
    //--
    validate();
    pack();
    setVisible(true);

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
  }

  private final void makeTitle(FtsIntegerVector data)
  { 
    if (data.getName() != null)
      setTitle(MaxWindowManager.getWindowManager().makeUniqueWindowTitle("Table "+data.getName()));
    else
    setTitle(MaxWindowManager.getWindowManager().makeUniqueWindowTitle("Table"));
    
    MaxWindowManager.getWindowManager().windowChanged(this);
  }

  private final void makeMenuBar(){
    JMenuBar mb = new JMenuBar();

    // Build the file menu
    itsFileMenu = new ircam.jmax.toolkit.menus.DefaultFileMenu();
    mb.add( itsFileMenu); 
    
    // Build the edit menu
    itsEditMenu = new EditMenu(this); 
    mb.add( itsEditMenu); 
    
    // Build the view menu
    itsViewMenu = new ViewMenu(this);
    mb.add( itsViewMenu);

    // New Tool menu 
    itsToolsMenu = new ircam.jmax.toolkit.menus.MaxToolsJMenu("Tools"); 
    mb.add(itsToolsMenu);

    // New Window Manager based Menu
    itsWindowsMenu = new ircam.jmax.toolkit.menus.MaxWindowJMenu("Windows", this); 
    mb.add(itsWindowsMenu);

    setJMenuBar(mb);
  }

  // ------ editorContainer interface ---------------
  public Editor getEditor(){
    return itsPanel;
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







