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
import java.awt.datatransfer.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.actions.*;
import ircam.jmax.editors.sequence.track.*;


public class TrackBasePopupMenu extends JPopupMenu 
{
  int x;
  int y;
  TrackBaseEditor target = null;    
  boolean isInSequence = false;
  JMenuItem removeItem, nameItem;
  JMenu moveMenu;
	JCheckBoxMenuItem saveItem;
  int trackCount = 0;
	
  JLabel maxLabel, minLabel;
  JSlider maxSlider, minSlider;
  Box maxBox, minBox;
  MoveTrackToAction moveToAction;
	
  public TrackBasePopupMenu( TrackBaseEditor editor, boolean isInSequence)
  {
    super();
    JMenuItem item;
		
    target = editor;
    this.isInSequence = isInSequence;
		
    addInspectorMenu();
		
    item = new JMenuItem("View as list");
    item.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent e)
		  {
				target.showListDialog();
		  }
		});
    add(item);
				
    addSeparator(); 
    
    item = new JMenuItem("Export Track");
    item.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent e)
		  {
				target.getTrack().getFtsTrack().export();
		  }
    });
    add(item);
    item = new JMenuItem("Import...");
    item.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent e)
		  {
				target.getTrack().getFtsTrack().importMidiFile();
		  }
    });
    add(item);
		
    if(isInSequence)
		{          
			addSeparator();
						
			removeItem = new JMenuItem("Remove Track");
			removeItem.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e)
	    {
					((FtsSequenceObject)target.getGraphicContext().getFtsObject()).
					removeTrack( target.getTrack());
	    }
			});
			add(removeItem);      
		}    
    pack();
  }
    
  void addInspectorMenu(){}
	
  public TrackBaseEditor getPopupTarget(){
    return target;
  }
	
  public void update(){}
		
  public void show(Component invoker, int x, int y)
  {
    this.x = x;
    this.y = y;
    
    super.show(invoker, x, y);
  }
}











