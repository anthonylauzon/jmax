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

public class RulerPopupMenu extends JPopupMenu 
{
  public RulerPopupMenu(SequenceRuler rul)
  {
    super();
    JCheckBoxMenuItem item;
    ruler = rul;
    rulerMenuGroup = new ButtonGroup();
    ActionListener listener = new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	    JCheckBoxMenuItem itm = (JCheckBoxMenuItem)e.getSource();
	    if(itm.getState())
	    {
		ruler.setUnityName(itm.getText());
	    } 
	}
    };
    item = new JCheckBoxMenuItem("Seconds");
    item.addActionListener(listener);
	
    add(item);
    rulerMenuGroup.add(item);

    item = new JCheckBoxMenuItem("Milliseconds");
    item.addActionListener(listener);
    add(item);
    rulerMenuGroup.add(item);

    item = new JCheckBoxMenuItem("Samples");
    item.addActionListener(listener);
    add(item);
    rulerMenuGroup.add(item);

    pack();
  }

    public void update()
    {
	JCheckBoxMenuItem item;
	String unitName = ruler.getUnityName();

	for( int i = 0; i < getComponentCount(); i++)
	    {
		item = (JCheckBoxMenuItem)getComponent(i);
		
		if (item.getText().equals(unitName))
		    {
			item.setSelected(true);
			break;
		    }
	    }
    }
 
  SequenceRuler ruler;
  private ButtonGroup rulerMenuGroup;
}











