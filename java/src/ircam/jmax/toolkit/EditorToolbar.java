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

package ircam.jmax.toolkit;

import javax.swing.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;

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

public class EditorToolbar extends JToolBar implements ActionListener{
  
  /**
   * constructor. It inserts the tools provided by the given
   * ToolManager. The direction parameter specify if the
   * toolbar should be HORIZONTAL or VERTICAL. 
   */
  public EditorToolbar(ToolManager manager, int direction) 
  {
    super();

    this.manager = manager;

    setDoubleBuffered(false);
    
    if (direction == VERTICAL)
      setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));

    /** prepare the popup */
    itsPopupMenu = new JPopupMenu();
    itsMenu = new JMenu("Tools");

    //install the Tools in the Toolbar
    Tool aTool;

    for (Enumeration e = manager.getTools(); e.hasMoreElements();) 
	{
	    aTool = (Tool) e.nextElement();
	    addTool(aTool);
	    
	}
    
  }
    
    /**
     * add a new tool in the toolbar (and in the popupMenu)
     */ 
    private void addTool(Tool theTool) 
    {
	JToggleButton aButton = new JToggleButton(theTool.getIcon());
	JMenuItem aMenuItem;
	
	aButton.setToolTipText(theTool.getName());
	aButton.setMargin(new Insets(0,0,0,0));
	aButton.addActionListener(this);
	aButton.addMouseListener(new MouseListener(){
	    public void mousePressed(MouseEvent e){
		if (e.isPopupTrigger()) 
		{
		    Object aSource =  e.getSource();
		    Tool aTool = (Tool) itsTools.get(aSource);
		    
		    if (aTool.getMenu() != null)
			aTool.getMenu().show(e.getComponent(), e.getX()-10, e.getY()-10);
		}
	    }
	    public void mouseClicked(MouseEvent e){}
	    public void mouseReleased(MouseEvent e){}
	    public void mouseEntered(MouseEvent e){}
	    public void mouseExited(MouseEvent e){}
	});
	itsTools.put(aButton, theTool);
	add(aButton);
	itsButtonGroup.add(aButton);
	
	aMenuItem = new JMenuItem (theTool.getIcon());
	aMenuItem.addActionListener(this);
	itsTools.put(aMenuItem, theTool);
	itsPopupMenu.add (aMenuItem);

	aMenuItem = new JMenuItem (theTool.getIcon());
	aMenuItem.addActionListener(this);
	itsTools.put(aMenuItem, theTool);
	itsMenu.add (aMenuItem);

	validate();
    }  
    
    
    /**
     * From the ActionListener interface, not for public use.
     * called when a user selects a new tool via the toolbar's user interface.
     * The new tool will work on the same graphic context then the old.
     */
    public void actionPerformed(ActionEvent e) 
    {    
	Object aSource =  e.getSource();
	Tool aTool = (Tool) itsTools.get(aSource);
	
	manager.changeTool(aTool);    
    }
    
    //---- Fields and accessors
    
    public JPopupMenu itsPopupMenu;
    public JMenu itsMenu;

    Hashtable itsTools = new Hashtable();
    
    private ButtonGroup itsButtonGroup = new ButtonGroup();
    public static final int VERTICAL = 0;
    public static final int HORIZONTAL = 1;
    
    private ToolManager manager;

}











