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

package ircam.jmax.editors.sequence.menus;

import java.util.*;
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.actions.*;
import ircam.jmax.editors.sequence.track.*;

//
// The graphic pop-up menu used to change the number of an inlet or an outlet in a subpatcher.
//

public class MonoTrackPopupMenu extends JPopupMenu 
{
  static private MonoTrackPopupMenu popup = new MonoTrackPopupMenu();

  int x;
  int y;
  MonoTrackEditor target = null;    
  private boolean added = false;
  JMenuItem displayLabelItem;
  JMenu moveMenu;
  int trackCount = 1;

  public MonoTrackPopupMenu()
  {
    super();
    JMenuItem item;

    addSeparator();
    moveMenu = new JMenu("Move to Position");
    item = new JMenuItem("1");
    item.addActionListener(Actions.moveMonodimensionalTrackToAction);
    moveMenu.add(item);
    
    add(moveMenu);

    pack();
  }

  static public MonoTrackPopupMenu getInstance()
  {
    return popup;
  }

  static public MonoTrackEditor getPopupTarget(){
    return popup.target;
  }

  static public void update(MonoTrackEditor editor)
  {
    if(!popup.added) 
      {
	popup.insert(editor.getToolsMenu(), 0);
        popup.target = editor;
	popup.added=true;
	popup.pack();
      }
    else /*if(popup.target!= editor)*/
	{
	  popup.remove(popup.target.getToolsMenu());
	  MidiTrackPopupMenu.getInstance().remove(popup.target.getToolsMenu());
	  
	  popup.insert(editor.getToolsMenu(), 0);
	  popup.target = editor;
	  popup.pack();
	}
    popup.updateMoveToMenu();
  }

    void updateMoveToMenu()
    {
	JMenuItem item;
	int count =  target.trackCount();
	if(trackCount==count)
	    return;
	else
	    {
		int dif = count-trackCount;
		
		if(dif>0)
		    for(int i=1; i<=dif; i++)
		    {
			item = new JMenuItem(""+(trackCount+i));
			item.addActionListener(Actions.moveMonodimensionalTrackToAction);
			moveMenu.add(item);			
		    }		
		else
		    for(int i=0; i<-dif; i++)
			moveMenu.remove(moveMenu.getItemCount()-1);
		trackCount = count;
	    }
    }
}











