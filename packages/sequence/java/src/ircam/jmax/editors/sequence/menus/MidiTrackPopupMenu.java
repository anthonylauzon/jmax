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

public class MidiTrackPopupMenu extends JPopupMenu 
{
  static private MidiTrackPopupMenu popup = new MidiTrackPopupMenu();

  int x;
  int y;
  MidiTrackEditor target = null;    
  private boolean added = false;
  JMenuItem displayLabelItem;
  JMenu moveMenu;
  int trackCount = 1;

  public MidiTrackPopupMenu()
  {
    super();
    JMenuItem item;

    addSeparator();
    moveMenu = new JMenu("Move to Position");
    item = new JMenuItem("1");
    item.addActionListener(Actions.moveMidiTrackToAction);
    moveMenu.add(item);
    
    add(moveMenu);

    JMenu viewMenu = new JMenu("Change View");
    item = new JMenuItem("Pianoroll view");
    item.addActionListener(new SetViewAction(MidiTrackEditor.PIANOROLL_VIEW));
    viewMenu.add(item);    
    item = new JMenuItem("NMS view");
    item.addActionListener(new SetViewAction(MidiTrackEditor.NMS_VIEW));
    viewMenu.add(item);

    addSeparator();
    add(viewMenu);

    item = new JMenuItem("View as list");
    item.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	    MidiTrackPopupMenu.getPopupTarget().showListDialog();
	}
    });
    add(item);
    addSeparator();
    displayLabelItem = new JMenuItem("Hide label");
    displayLabelItem.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  MidiTrackPopupMenu.getPopupTarget().setDisplayLabels(!MidiTrackPopupMenu.getPopupTarget().isDisplayLabels());
	}
    });
    add(displayLabelItem);

    pack();
  }

  static public MidiTrackPopupMenu getInstance()
  {
    return popup;
  }

  static public MidiTrackEditor getPopupTarget(){
    return popup.target;
  }

  static public void update(MidiTrackEditor editor)
  {
    if(!popup.added) 
      {
	popup.insert(editor.getToolsMenu(), 0);
        popup.target = editor;
	popup.added=true;
	popup.pack();
      }
    else
	{
	  popup.remove(popup.target.getToolsMenu());
	  MonodimensionalTrackPopupMenu.getInstance().remove(popup.target.getToolsMenu());

	  popup.insert(editor.getToolsMenu(), 0);
	  popup.target = editor;
	  popup.pack();
	}
    popup.updateMoveToMenu();

    if(!popup.target.isDisplayLabels())
	popup.displayLabelItem.setText("Display Labels");
    else
	popup.displayLabelItem.setText("Hide Labels");
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
			item.addActionListener(Actions.moveMidiTrackToAction);
			moveMenu.add(item);			
		    }		
		else
		    for(int i=0; i<-dif; i++)
			moveMenu.remove(moveMenu.getItemCount()-1);
		trackCount = count;
	    }
    }

  class SetViewAction extends AbstractAction {
    SetViewAction(int viewType)
    {
      super("Set View");
      this.viewType = viewType;
    }
    
    public void actionPerformed(ActionEvent e)
    {
      MidiTrackPopupMenu.getPopupTarget().setViewMode(viewType);
    }
	
    int viewType;    
  }  
}











