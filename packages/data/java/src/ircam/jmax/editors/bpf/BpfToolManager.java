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

package ircam.jmax.editors.bpf;

import javax.swing.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;

import ircam.jmax.toolkit.*;

/**
 * A specific component used as a container of JToggleButtons 
 * corresponding to Tools. 
 * The EditorToolbar class keeps a JPopupMenu with all the tools inserted;
 * this Popup can be accessed and used as an alternative for choosing 
 * a tool (example, right-mouse click). 
 */
//--------------------------------
// implementation notes:
// starting from 2.4.8, this class have been reduced to a simple 
// graphic component that is able to select tools. The model for this
// UI element is represented by the ToolManager class
//--------------------------------

public class BpfToolManager extends ToolManager implements ActionListener{
  
  public BpfToolManager()
  {
    super();
  }

  public BpfToolManager(ToolProvider theProvider) 
  {
    super(theProvider);    
    if(itsMenu==null) initBpfToolManager();
  }
    
  /**
   * add a new tool in the popupMenu
   */ 
  public void addTool(Tool theTool) 
  {
    super.addTool(theTool);

    if(itsMenu==null) initBpfToolManager();

    JMenuItem aMenuItem = new JMenuItem (theTool.getIcon());
    
    aMenuItem.addActionListener(this);
    
    itsBpfTools.put(aMenuItem, theTool);
    
    itsMenu.add(aMenuItem);
  }  
    
  void initBpfToolManager()
  {
     itsMenu = new JMenu("Tools");
     itsBpfTools = new Hashtable();    
  }
    
  /**
   * From the ActionListener interface, not for public use.
   * called when a user selects a new tool via the toolbar's user interface.
   * The new tool will work on the same graphic context then the old.
   */
  public void actionPerformed(ActionEvent e) 
  {    
    Object aSource =  e.getSource();
    Tool aTool = (Tool) itsBpfTools.get(aSource);	
    changeTool(aTool);    
  }
  
    public JMenu getMenu()
    {
	return itsMenu;
    }
  
  //---- Fields and accessors    
  private JMenu itsMenu;
  Hashtable itsBpfTools;
}











