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

public class MidiTrackPopupMenu extends TrackBasePopupMenu 
{	
  public MidiTrackPopupMenu( MidiTrackEditor editor, boolean isInSequence)
  {
    super(editor, isInSequence);
		JMenuItem item;
    
    addSeparator();
    
    item = new JMenuItem("Create Bars");
    item.addActionListener( new ActionListener(){
			public void actionPerformed(ActionEvent e)
		  {
				((MidiTrackEditor)target).getTrack().getFtsTrack().createMarkers();
		  }
		});	
    add(item);
    
    item = new JMenuItem("Make Trill");
    item.addActionListener( new ActionListener(){
			public void actionPerformed(ActionEvent e)
		  {
        SwingUtilities.invokeLater(new Runnable(){
          public void run()
          {
            ((MidiTrackEditor)target).getTrack().getFtsTrack().makeTrillFromSelection( target.getSelection().getSelected());
          }
        });
      }
		});	
    add(item);
    
    item = new JMenuItem("Append Bar");
    item.addActionListener( new ActionListener(){
			public void actionPerformed(ActionEvent e)
		  {
				((MidiTrackEditor)target).getTrack().getFtsTrack().appendBar( null);
		  }
		});	
    add(item);
		
		item = new JMenuItem("Collapse markers");
    item.addActionListener( new ActionListener(){
			public void actionPerformed(ActionEvent e)
		  {
				FtsTrackObject track = ((MidiTrackEditor)target).getTrack().getFtsTrack();
				SequenceSelection sel = target.getGraphicContext().getMarkersSelection();
				TrackEvent bar = null;
				if(sel != null && sel.size() > 1)
					track.collapseMarkers( sel.getSelected());
		  }
		});	
    add(item);
    pack();
  }
	
  void addInspectorMenu()
  {
    JMenuItem item = new JMenuItem("Inspect ... ");
    item.addActionListener(new ActionListener(){
      public void actionPerformed(ActionEvent e)
      {
        MidiTrackInspector.inspect(target, target.getGraphicContext().getFrame(), isInSequence,
                                   SwingUtilities.convertPoint(target, x, y, target.getGraphicContext().getFrame()));
      }
    });
    add(item);
  }    
  	
  public void update()
  {
    super.update();
  }	
}











