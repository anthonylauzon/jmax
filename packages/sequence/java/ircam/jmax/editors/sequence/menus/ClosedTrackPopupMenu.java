//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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

public class ClosedTrackPopupMenu extends JPopupMenu 
{
  static private ClosedTrackPopupMenu popup = new ClosedTrackPopupMenu();

  int x;
  int y;
  TrackEditor target = null;    
  private boolean added = false;
  JMenuItem displayLabelItem;
  JMenu moveMenu;
  int trackCount = 0;

  public ClosedTrackPopupMenu()
  {
    super();
    JMenuItem item;

    moveMenu = new JMenu("Move to Position");
    item = new JMenuItem(""+trackCount);
    item.addActionListener(Actions.moveClosedTrackToAction);
    moveMenu.add(item);
    
    add(moveMenu);

    addSeparator();
    
    item = new JMenuItem("Remove Track");
    item.addActionListener(new ActionListener(){
      public void actionPerformed(ActionEvent e)
      {
        ((FtsSequenceObject)ClosedTrackPopupMenu.getPopupTarget().getGraphicContext().getFtsObject()).
        removeTrack(ClosedTrackPopupMenu.getPopupTarget().getTrack());
      }
    });
    add(item);

    pack();
  }

  static public ClosedTrackPopupMenu getInstance()
  {
    return popup;
  }

  static public TrackEditor getPopupTarget(){
    return popup.target;
  }

  static public void update(TrackEditor editor)
  {
      popup.target = editor;
      popup.updateMoveToMenu();
  }

  void updateMoveToMenu()
  {
    JMenuItem item;
    int count = ((FtsSequenceObject)target.getGraphicContext().getFtsObject()).trackCount()-1;
    if(trackCount==count)
      return;
    else
      {
	int dif = count-trackCount;
	
	if(dif>0)
	  for(int i=1; i<=dif; i++)
	    {
	      item = new JMenuItem(""+(trackCount+i));
	      item.addActionListener(Actions.moveClosedTrackToAction);
	      moveMenu.add(item);			
	    }		
	else
	  for(int i=0; i<-dif; i++)
	    moveMenu.remove(moveMenu.getItemCount()-1);
	trackCount = count;
      }
  }
}











