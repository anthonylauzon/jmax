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

package ircam.jmax.toolkit.menus;

import java.io.*;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.console.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;

/** Implement the patcher editor File Menu */

public class DefaultFileMenu extends EditorMenu
{
  JMenuItem dspMenuItem;
  public DefaultFileMenu()
  {
    super("File");

    setHorizontalTextPosition(AbstractButton.LEFT);
    setDefaultNumEntries(7);

    add(DefaultActions.newAction, "New", Toolkit.getDefaultToolkit().getMenuShortcutKeyMask(), KeyEvent.VK_N);
    add(DefaultActions.openAction, "Open", Toolkit.getDefaultToolkit().getMenuShortcutKeyMask(), KeyEvent.VK_O);

    addSeparator();

    add(DefaultActions.closeAction, "Close", Toolkit.getDefaultToolkit().getMenuShortcutKeyMask(), KeyEvent.VK_W);

    addSeparator();
    
    dspMenuItem = add(DefaultActions.dspAction, "Activate DSP", Toolkit.getDefaultToolkit().getMenuShortcutKeyMask(), KeyEvent.VK_ENTER);
    add(DefaultActions.quitAction, "Quit", Toolkit.getDefaultToolkit().getMenuShortcutKeyMask(), KeyEvent.VK_Q);
  
    JMaxApplication.getRecentFileHistory().addListDataListener(new ListDataListener(){
	public void contentsChanged(ListDataEvent e)
	{
	  buildRecentFiles();
	}
	public void intervalAdded(ListDataEvent e)
	{
	  buildRecentFiles();
	}    
	public void intervalRemoved(ListDataEvent e) 
	{
	  buildRecentFiles();
	}
      });

    buildRecentFiles();
  }

  public void updateMenu()
  {
    if(JMaxApplication.getFtsServer() != null)
      {
	dspMenuItem.setEnabled(true);

	boolean dspOn = JMaxApplication.getDspControl().getDspOn();
      
	if(dspOn) 
	  dspMenuItem.setText("Desactivate DSP");
	else
	  dspMenuItem.setText("Activate DSP");
      }
    else
      dspMenuItem.setEnabled(false);
  }

  void buildRecentFiles()
  {
    RecentFileHistory recentFileHistory = JMaxApplication.getRecentFileHistory();
	
    //remove all recent Files
    int num = getItemCount() - getDefaultNumEntries();
    while(num>0)
      {
	remove(getItemCount()-1); 
	num--;
      }

    if(recentFileHistory.size() > 0)          
      {  
	File file;
	JMenuItem jMenuItem;

	addSeparator();

	for (int i = 0; i < recentFileHistory.size(); ++i)
	  {
	    file = (File)recentFileHistory.get(i);
		  
	    jMenuItem = add(new OpenAction(file), (i+1)+":  "+file.getName());
	    jMenuItem.setMnemonic(Character.forDigit(i+1, 10));
	  }
      }
  }
}

