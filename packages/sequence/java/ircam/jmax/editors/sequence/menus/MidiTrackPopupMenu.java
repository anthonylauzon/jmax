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
	
  JLabel maxLabel, minLabel;
  JSlider maxSlider, minSlider;
  Box maxBox, minBox;
  LabelTypesAction labelAction;
  JRadioButtonMenuItem pianoItem, stavesItem;
	
  public MidiTrackPopupMenu( MidiTrackEditor editor, boolean isInSequence)
  {
    super(editor, isInSequence);
		
    addSeparator();
		
    labelTypesMenu = new JMenu("Labels");
    add( labelTypesMenu);
		
    labelAction = new LabelTypesAction((MidiTrackEditor)target);
		
    pack();
  }
	
  boolean addRangeMenu()
  {
    JMenu rangeMenu = new JMenu("Range");
    JMenu maxRangeMenu = new JMenu("Maximum");
    JMenu minRangeMenu = new JMenu("Minimum");
		
    maxLabel = new JLabel(" 127 ", JLabel.CENTER);
    maxLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
		
    maxSlider = new JSlider(JSlider.VERTICAL, 0, 127, 127);
    maxSlider.setMajorTickSpacing(12);
    maxSlider.setMinorTickSpacing(6);
    maxSlider.setBorder(BorderFactory.createEmptyBorder(0,0,0,10));
    maxSlider.setPaintTicks(true);
    maxSlider.addChangeListener(new ChangeListener(){
			public void stateChanged(ChangeEvent e) {
				JSlider source = (JSlider)e.getSource();
				int max = (int)source.getValue();
				int min = ((Integer)target.getTrack().getProperty("minimumPitch")).intValue();	    
				if(max<min) max = min+1;
				
				if(!source.getValueIsAdjusting())
					target.getTrack().setProperty("maximumPitch", new Integer(max));
				
				maxLabel.setText(""+max);
			}
		});
    maxBox = new Box(BoxLayout.Y_AXIS);
    maxBox.add(maxSlider);
    maxBox.add(maxLabel);
    maxBox.validate();
    
    minLabel = new JLabel("  0  ", JLabel.CENTER);
    minLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
    
    minSlider = new JSlider(JSlider.VERTICAL, 0, 127, 0);
    minSlider.setMajorTickSpacing(12);
    minSlider.setMinorTickSpacing(6);
    minSlider.setBorder(BorderFactory.createEmptyBorder(0,0,0,10));
    minSlider.setPaintTicks(true);
    minSlider.addChangeListener(new ChangeListener(){
			public void stateChanged(ChangeEvent e) {
				JSlider source = (JSlider)e.getSource();
				int min = (int)source.getValue();
				int max = ((Integer)target.getTrack().getProperty("maximumPitch")).intValue();
				if(min>max) min = max-1;
				
				if (!source.getValueIsAdjusting())
					target.getTrack().setProperty("minimumPitch", new Integer(min));
				
				minLabel.setText(""+min);
			}
		});
    minBox = new Box(BoxLayout.Y_AXIS);
    minBox.add(minSlider);
    minBox.add(minLabel);
    minBox.validate();
    
    maxRangeMenu.add(maxBox);
    minRangeMenu.add(minBox);
    
    rangeMenu.add(maxRangeMenu);
    rangeMenu.add(minRangeMenu);
    
    add(rangeMenu);
    
    return true;
  }
	
  boolean addViewMenu()
  {
    ButtonGroup viewsMenuGroup = new ButtonGroup();
    
    pianoItem = new JRadioButtonMenuItem("Pianoroll");
    pianoItem.addActionListener(new SetViewAction(MidiTrackEditor.PIANOROLL_VIEW, target));
    viewsMenuGroup.add(pianoItem);
    add(pianoItem);    
    stavesItem = new JRadioButtonMenuItem("Staves");
    stavesItem.addActionListener(new SetViewAction(MidiTrackEditor.NMS_VIEW, target));
    viewsMenuGroup.add(stavesItem);
    add(stavesItem);
    pianoItem.setSelected(true);
		
    return true;
  }
	
  public void update()
  {
    updateChangeRangeMenu();
		
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
	
  void updateChangeRangeMenu()
  {
    int max =  ((Integer)target.getTrack().getProperty("maximumPitch")).intValue();
    int min =  ((Integer)target.getTrack().getProperty("minimumPitch")).intValue();
    
    if(maxSlider.getValue()!=max) maxSlider.setValue(max);
    if(minSlider.getValue()!=min) minSlider.setValue(min);
  }
}











