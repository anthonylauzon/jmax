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

package ircam.jmax.editors.explode;

import java.io.*;
import java.lang.*;
import java.awt.event.*;
import java.awt.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.mda.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;

import javax.swing.*;
import javax.swing.table.*;

import ircam.jmax.editors.explode.menus.*;

/**
 * The main class of the explode package.
 * It inherits from MaxEditor, and implements the MaxDataEditor Interface.
 * This implementation builds a ScrPanel and a ExplodeTablePanel to represent the data.
 */
public class Explode extends JFrame implements EditorContainer, AAAReadme {

  //------------------- fields
    ExplodeRemoteData explodeData;
  ExplodePanel itsExplodePanel;
  JMenu itsFileMenu;
  EditMenu itsEditMenu;
  OptionsMenu itsOptionsMenu;
  JMenu itsToolsMenu;
  JMenu itsWindowsMenu;

  /**
   * constructor.
   * It creates the panels that will display the datas in maxData
   */
  public Explode(ExplodeRemoteData maxData)
  { 
    super();

    explodeData = (ExplodeRemoteData) maxData;
    MaxWindowManager.getWindowManager().addWindow(this);

    makeTitle(maxData);


    // Build The Menus and Menu Bar
    makeMenuBar();

    addWindowListener(new WindowListener(){
        public void windowOpened(WindowEvent e){}
	public void windowClosed(WindowEvent e){}
	public void windowClosing(WindowEvent e)
	{
	    MaxWindowManager.getWindowManager().removeWindow(getFrame());
	    System.err.println("meo "+getFrame());
	}
	public void windowDeiconified(WindowEvent e){}
	public void windowIconified(WindowEvent e){}
	public void windowActivated(WindowEvent e){}
	public void windowDeactivated(WindowEvent e){}
    });

    itsExplodePanel = new ExplodePanel(this, maxData);
    getContentPane().add(itsExplodePanel);
    pack();
    validate();
    setVisible(true);
  }

    /** This function is an example on how to translate an explode DB into an agrep form.*/
    void translateToAgrep(Writer w)
    {
	PrintWriter pw = new PrintWriter(w);
	
	try {
	    for (int i = 0; i<explodeData.length(); i++)
		{
		    pw.print((char) (128+explodeData.getEventAt(i).getPitch()));
		}
	}
	catch(Exception e){}
	pw.println();
    }
    
    
    // Build The Menus and Menu Bar

  private final void makeTitle(ExplodeRemoteData maxData){
    setTitle(MaxWindowManager.getWindowManager().makeUniqueWindowTitle("Explode " + maxData.getName()));
    MaxWindowManager.getWindowManager().windowChanged(this);
  } 

  private final void makeMenuBar(){
    JMenuBar mb = new JMenuBar();

    // Build the file menu
    itsFileMenu = new DefaultFileMenu();
    mb.add(itsFileMenu); 
    
    // Build the edit menu
    itsEditMenu = new EditMenu(this); 
    mb.add( itsEditMenu); 
    
    // Build the options menu
    itsOptionsMenu = new OptionsMenu(this); 
    mb.add( itsOptionsMenu); 

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
    return itsExplodePanel;
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

