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
import ircam.jmax.editors.sequence.renderers.*;

//
// The graphic pop-up menu used to change the number of an inlet or an outlet in a subpatcher.
//

public class MidiEventPopupMenu extends JPopupMenu
{
  static private MidiEventPopupMenu popup = new MidiEventPopupMenu();
  TrackEvent target = null;    
  SequenceGraphicContext sgc;

  public MidiEventPopupMenu()
  {
    super();

    JMenu altMenu = new JMenu("Set Alteration");
    JMenuItem item;
    item = new JMenuItem("Diesis #");
    item.addActionListener(new SetAlterationAction(PartitionAdapter.ALTERATION_DIESIS));
    altMenu.add(item);    
    item = new JMenuItem("Bemolle b");
    item.addActionListener(new SetAlterationAction(PartitionAdapter.ALTERATION_BEMOLLE));
    altMenu.add(item);
    item = new JMenuItem("Nothing");
    item.addActionListener(new SetAlterationAction(PartitionAdapter.ALTERATION_NOTHING));
    altMenu.add(item);

    add(altMenu);

    pack();
  }

  static public MidiEventPopupMenu getInstance()
  {
    return popup;
  }

  static public TrackEvent getPopupTarget(){
    return popup.target;
  }

  static public void update(TrackEvent target, SequenceGraphicContext gc)
  {
    popup.target = target;
    popup.sgc = gc;
  }

  class SetAlterationAction extends AbstractAction {
    SetAlterationAction(int alt)
    {
      super("Set View");
      this.alteration = alt;
    }
    
    public void actionPerformed(ActionEvent e)
    {
	int alt, value;
	TrackEvent evt = MidiEventPopupMenu.getPopupTarget();
	SequenceGraphicContext gc = MidiEventPopupMenu.getInstance().sgc;

	alt = ((PartitionAdapter)gc.getAdapter()).getAlteration(evt);
	value = ((Integer)evt.getProperty("pitch")).intValue();

	if((alt!=PartitionAdapter.ALTERATION_NOTHING)&&
	   (alteration==PartitionAdapter.ALTERATION_NOTHING))
	    value = value-alt;
	else
	    value = value+alteration;

	/*switch(alt)
	  {
	  case PartitionAdapter.ALTERATION_NOTHING:
	  value = value+alteration;
	  break;
	  case PartitionAdapter.ALTERATION_DIESIS:
	  case PartitionAdapter.ALTERATION_BEMOLLE:
	  if(alteration==PartitionAdapter.ALTERATION_NOTHING)
	  value = value-alt;
	  else
	  value = value+alteration;
	  }*/

	evt.setProperty("pitch", new Integer(value));
	evt.getRenderer().render(evt, gc.getGraphicDestination().getGraphics(), true, gc);
    }
	
    int alteration;    
  }  
}











