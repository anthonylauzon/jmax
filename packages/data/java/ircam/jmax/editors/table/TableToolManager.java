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

import javax.swing.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;

import ircam.jmax.toolkit.*;

public class TableToolManager extends ToolManager implements ActionListener{
  
  public TableToolManager()
  {
    super();
  }

  public TableToolManager( ToolProvider theProvider) 
  {
    super( theProvider);    
    if( itsMenu==null) initTableToolManager();
  }
    
  /**
   * add a new tool in the popupMenu
   */ 
  public void addTool(Tool theTool) 
  {
    super.addTool(theTool);

    if( itsMenu==null) initTableToolManager();
    
    JMenuItem aMenuItem = new JMenuItem( theTool.getName(), theTool.getIcon());
    
    aMenuItem.addActionListener(this);
      
    itsTableTools.put( theTool.getName(), theTool);
    
    itsMenu.add(aMenuItem);
  }  
    
  void initTableToolManager()
  {
    itsMenu = new JMenu("Tools");
    itsTableTools = new Hashtable();    
  }
  
  /**
   * From the ActionListener interface, not for public use.
   * called when a user selects a new tool via the toolbar's user interface.
   * The new tool will work on the same graphic context then the old.
   */
  public void actionPerformed(ActionEvent e) 
  {    
    Object aSource =  e.getSource();
    Tool aTool = (Tool) itsTableTools.get(((JMenuItem)aSource).getText());	
    changeTool(aTool);    
  }
  
  public JMenu getMenu()
  {
    return itsMenu;
  }  
  //---- Fields and accessors    
  private JMenu itsMenu;
  Hashtable itsTableTools;
}











