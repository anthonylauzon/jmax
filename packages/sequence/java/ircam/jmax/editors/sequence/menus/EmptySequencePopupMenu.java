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

public class EmptySequencePopupMenu extends JPopupMenu 
{
  public EmptySequencePopupMenu(SequencePanel sp)
  {  
    super();
    seqPanel = sp;
		
		initMenu();				
    pack();
	}

	public void initMenu()
	{
		add( new JLabel("Add New Track"));
    FtsSequenceObject seq = seqPanel.getFtsSequenceObject();
    for(Enumeration e = TrackEditorFactoryTable.getTypes(); e.hasMoreElements();)
      add( new AddTrackAction((ValueInfo)e.nextElement(), seq));
	}

	public void update()
  {
	}

	SequencePanel seqPanel = null;
}











