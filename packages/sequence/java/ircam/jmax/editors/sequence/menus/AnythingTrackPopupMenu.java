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

public class AnythingTrackPopupMenu extends JPopupMenu 
{
  static private AnythingTrackPopupMenu popup = new AnythingTrackPopupMenu();

  int x;
  int y;
  AnythingTrackEditor target = null;    
  private boolean added = false;
  JMenuItem displayLabelItem, removeItem, nameItem;
  int trackCount = 0;

  public AnythingTrackPopupMenu()
  {
    super();
    JMenuItem item;

    addSeparator();

    nameItem = new JMenuItem("Track Name");
    nameItem.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  ChangeTrackNameDialog.changeName(AnythingTrackPopupMenu.getPopupTarget().getTrack(),  
					   AnythingTrackPopupMenu.getPopupTarget().getGraphicContext().getFrame(),
					   SwingUtilities.convertPoint(AnythingTrackPopupMenu.getPopupTarget(), 
								       AnythingTrackPopupMenu.getPopupX(),
								       AnythingTrackPopupMenu.getPopupY(),
								       AnythingTrackPopupMenu.getPopupTarget().getGraphicContext().getFrame()));
	}
      });
    
    add( nameItem);

    item = new JMenuItem("View as list");
    item.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  AnythingTrackPopupMenu.getPopupTarget().showListDialog();
	}
      });
    add(item);

    item = new JMenuItem("Select All");
    item.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	    AnythingTrackPopupMenu.getPopupTarget().getSelection().selectAll();
	    AnythingTrackPopupMenu.getPopupTarget().getGraphicContext().getGraphicDestination().requestFocus();
	}
    });
    add(item);

    addSeparator();
     ////////////////////////////////////////////////////////////
    removeItem = new JMenuItem("Remove Track");
    removeItem.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  ((FtsSequenceObject)AnythingTrackPopupMenu.getPopupTarget().getGraphicContext().getFtsObject()).
	    removeTrack( AnythingTrackPopupMenu.getPopupTarget().getTrack());
	}
      });
    add(removeItem);
    
    pack();
  }

  static public AnythingTrackPopupMenu getInstance()
  {
    return popup;
  }

  static public AnythingTrackEditor getPopupTarget(){
    return popup.target;
  }

  static public void update( AnythingTrackEditor editor)
  {
    if( !popup.added) 
      {
	popup.insert( editor.getToolsMenu(), 0);
        popup.target = editor;
	popup.added=true;
	popup.pack();
      }
    else 
      {
	popup.remove( popup.target.getToolsMenu());
	MidiTrackPopupMenu.getInstance().remove( popup.target.getToolsMenu());
	MonoTrackPopupMenu.getInstance().remove( popup.target.getToolsMenu());
	  
	popup.insert(editor.getToolsMenu(), 0);
	popup.target = editor;
	popup.pack();
      }
    
    if( popup.target.getGraphicContext().getFtsObject() instanceof FtsSequenceObject)
      {
	popup.removeItem.setEnabled( true);
	popup.nameItem.setEnabled( true);
      }    
    else
      {
	popup.removeItem.setEnabled( false);
	popup.nameItem.setEnabled( false);
      }
  }

  public void show(Component invoker, int x, int y)
  {
    this.x = x;
    this.y = y;
    
    super.show(invoker, x, y);
  }
  static public int getPopupX()
  {
    return popup.x;
  }
  static public int getPopupY()
  {
    return popup.y;
  }
}











