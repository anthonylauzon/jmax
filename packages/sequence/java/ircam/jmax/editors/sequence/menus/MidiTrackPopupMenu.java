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
  JMenu labelTypesMenu;
  private ButtonGroup labelTypesMenuGroup;
	JRadioButtonMenuItem usedRangeItem, wholeRangeItem;
  LabelTypesAction labelAction;
  JRadioButtonMenuItem pianoItem, stavesItem;
	JRadioButtonMenuItem timeItem, measuresItem;
	
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
				((MidiTrackEditor)target).getTrack().getFtsTrack().makeTrillFromSelection( target.getSelection().getSelected());
		  }
		});	
    add(item);
    
    addSeparator();
		
    labelTypesMenu = new JMenu("Labels");
    add( labelTypesMenu);
		
    labelAction = new LabelTypesAction((MidiTrackEditor)target);
		
    pack();
  }
	
  boolean addRangeMenu()
  {
		ButtonGroup rangeMenuGroup = new ButtonGroup();
    
    usedRangeItem = new JRadioButtonMenuItem("Used Range");
    usedRangeItem.addActionListener( new ActionListener(){
			public void actionPerformed(ActionEvent e)
		  {
				((MidiTrackEditor)target).setRangeMode( MidiTrackEditor.USED_RANGE, true);
		  }
		});
    rangeMenuGroup.add(usedRangeItem);
    add(usedRangeItem);    
    wholeRangeItem = new JRadioButtonMenuItem("Whole Range");
    wholeRangeItem.addActionListener( new ActionListener(){
			public void actionPerformed(ActionEvent e)
		  {
				((MidiTrackEditor)target).setRangeMode( MidiTrackEditor.WHOLE_RANGE, true);
		  }
		});		
    rangeMenuGroup.add(wholeRangeItem);
    add(wholeRangeItem);
    wholeRangeItem.setSelected(true);
		
    return true;				
  }
	
  boolean addViewMenu()
  {
    ButtonGroup viewsMenuGroup = new ButtonGroup();
    
    pianoItem = new JRadioButtonMenuItem("Pianoroll");
		pianoItem.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent e)
		  {
				target.setViewMode( MidiTrackEditor.PIANOROLL_VIEW);
				if(((Integer)target.getTrack().getProperty("rangeMode")).intValue() == MidiTrackEditor.USED_RANGE)
				{
					target.getTrack().setProperty("maximumPitch", new Integer(((MidiTrackEditor)target).getMaximumPitchInTrack()));
					target.getTrack().setProperty("minimumPitch", new Integer(((MidiTrackEditor)target).getMinimumPitchInTrack()));
				}
			}		
		});		
    viewsMenuGroup.add(pianoItem);
    add(pianoItem);    
    stavesItem = new JRadioButtonMenuItem("Staves");
		stavesItem.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent e)
		  {
				target.setViewMode( MidiTrackEditor.NMS_VIEW);
				if(((Integer)target.getTrack().getProperty("rangeMode")).intValue() == MidiTrackEditor.USED_RANGE)
				{
					target.getTrack().setProperty("maximumPitch", new Integer(((MidiTrackEditor)target).getMaximumPitchInTrack()));
					target.getTrack().setProperty("minimumPitch", new Integer(((MidiTrackEditor)target).getMinimumPitchInTrack()));
				}
			}		
		});		
    viewsMenuGroup.add(stavesItem);
    add(stavesItem);
    pianoItem.setSelected(true);
		
		/************** Time or Measures Grid *******************/
    ButtonGroup gridMenuGroup = new ButtonGroup();
    
    timeItem = new JRadioButtonMenuItem("Show Time Grid");
		timeItem.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent e)
		  {
				((MidiTrackEditor)target).setGridMode( TrackEditor.TIME_GRID);
			}		
		});		
    gridMenuGroup.add(timeItem);
    add(timeItem);    
    measuresItem = new JRadioButtonMenuItem("Show Markers");
		measuresItem.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent e)
		  {
				((MidiTrackEditor)target).setGridMode( TrackEditor.MEASURES_GRID);
			}		
		});		
    gridMenuGroup.add(measuresItem);
    add(measuresItem);
    timeItem.setSelected(true);		

    return true;
  }
	
  public void update()
  {
    updateRangeMenu();
		
    updateLabelTypesMenu();
		
    super.update();
  }
	
  void updateViewMenu()
  {
    switch( target.getViewMode())
		{
      case MidiTrackEditor.NMS_VIEW:
				stavesItem.setSelected(true);
				break;
      case MidiTrackEditor.PIANOROLL_VIEW:
      default:
				pianoItem.setSelected(true);
				break;
		}
  }
	
  void updateLabelTypesMenu()
  {
    labelTypesMenu.removeAll();
    labelTypesMenuGroup = new ButtonGroup();
    
    String currentType = ((MidiTrackEditor)target).getLabelType();
    String type;
		
    JRadioButtonMenuItem selectItem = null;
    JRadioButtonMenuItem item =  new JRadioButtonMenuItem( "none"); 
    item.addActionListener( labelAction);
    labelTypesMenu.add( item);
    labelTypesMenuGroup.add(item);
    if( currentType.equals("none"))
      selectItem = item;
		
    for (Enumeration e = target.getTrack().getFtsTrack().getPropertyNames() ; e.hasMoreElements();)
		{
			type = (String)e.nextElement();
			item = new JRadioButtonMenuItem( type);
			item.addActionListener( labelAction);
			labelTypesMenu.add(item);
			labelTypesMenuGroup.add(item);
			if( currentType.equals(type)) selectItem = item;
		}
		
    if( selectItem != null) selectItem.setSelected( true);
  }
	
  void updateRangeMenu()
  {
		int rangeMode = ((Integer)target.getTrack().getProperty("rangeMode")).intValue();
		if(rangeMode == MidiTrackEditor.USED_RANGE && !usedRangeItem.isSelected())
			usedRangeItem.setSelected(true);
		else
			if(rangeMode == MidiTrackEditor.WHOLE_RANGE && !wholeRangeItem.isSelected()) 
				wholeRangeItem.setSelected(true);
  }
}











