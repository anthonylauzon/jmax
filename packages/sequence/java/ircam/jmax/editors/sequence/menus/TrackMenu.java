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

import java.awt.*;
import java.awt.event.*;

import java.util.*;
import javax.swing.*;

import ircam.jmax.*;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.actions.*;
import ircam.jmax.editors.sequence.track.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;


public class TrackMenu extends EditorMenu
{
  public TrackMenu(EditorContainer container, SequenceDataModel sequenceModel)
  {
    super("Track");
    setMnemonic(KeyEvent.VK_T);

    this.sequenceModel = sequenceModel;

    setHorizontalTextPosition(AbstractButton.LEFT);

    JMenu addTrackMenu = new JMenu("Add Track");
    FillAddTrackMenu(addTrackMenu);

    add(addTrackMenu);
    add(Actions.removeTrackAction);
    
    addSeparator();
    add(Actions.exportAction);
    add(Actions.mergeAction);

    addSeparator();
    JMenuItem item;
    item = new JMenuItem("Close All Tracks");
    item.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  ((FtsSequenceObject)TrackMenu.this.sequenceModel).setOpenedAllTracks(false);
	}
      });
    add(item);
    item = new JMenuItem("Open All Tracks");
    item.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  ((FtsSequenceObject)TrackMenu.this.sequenceModel).setOpenedAllTracks(true);
	}
      });
    add(item);
  }
    
  private void FillAddTrackMenu(JMenu menu)
  {
    for(Enumeration e = TrackEditorFactoryTable.getTypes(); e.hasMoreElements();)
	menu.add( new AddTrackAction((ValueInfo)e.nextElement(), (FtsSequenceObject)sequenceModel));
  }
  //---
  SequenceDataModel sequenceModel;
}










