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

package ircam.jmax.editors.sequence.track;

import java.awt.*;
import javax.swing.*;
import java.util.*;
import java.awt.event.*;
import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.menus.*;
/**
* */

public class MidiTrackInspector extends JDialog 
{
  JTextField maximumField, minimumField;
  Track track;
  TrackBaseEditor trackEditor;
  JCheckBox saveStateCheckBox, showMarkersCheckBox;
  JRadioButton pianorollButton, stavesButton, usedRangeButton, wholeRangeButton;
  JComboBox moveToCombo, labelsCombo;
  boolean isInSequence;
  boolean updating = false;
  
  public MidiTrackInspector(TrackBaseEditor editor, Frame frame, boolean isInSequence)
  {
    super(frame, "Midi Track Inspector", true);
    setResizable(false);
    
    trackEditor = editor;
    track = editor.getTrack();
    this.isInSequence = isInSequence;
    
    /* root panel */
    JPanel rootPanel = new JPanel();
    rootPanel.setLayout(new BoxLayout(rootPanel, BoxLayout.Y_AXIS));
    rootPanel.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createEmptyBorder( 2, 2, 2, 2), BorderFactory.createEtchedBorder()));
    
    /* save state panel */
    JPanel saveStatePanel = new JPanel();
    saveStatePanel.setLayout(new BoxLayout(saveStatePanel, BoxLayout.X_AXIS));
    
    saveStateCheckBox = new JCheckBox("Save Editor State");
    saveStateCheckBox.addItemListener( new ItemListener(){
      public void itemStateChanged(ItemEvent e)
      {
        if(!updating)
          track.getFtsTrack().requestSetSaveEditor(e.getStateChange() == ItemEvent.SELECTED);
      };
    });
    saveStatePanel.add(saveStateCheckBox);
    saveStatePanel.add(Box.createHorizontalGlue());
    rootPanel.add(saveStatePanel);
    
    /* show markers panel */
    JPanel showMarkersPanel = new JPanel();
    showMarkersPanel.setLayout(new BoxLayout(showMarkersPanel, BoxLayout.X_AXIS));
    
    showMarkersCheckBox = new JCheckBox("Show Markers");
    showMarkersCheckBox.addItemListener( new ItemListener(){
      public void itemStateChanged(ItemEvent e)
      {
        if(!updating)
        {
          if(e.getStateChange() == ItemEvent.SELECTED)
            ((MidiTrackEditor)trackEditor).setGridMode( TrackEditor.MEASURES_GRID);
          else
            ((MidiTrackEditor)trackEditor).setGridMode( TrackEditor.TIME_GRID);
        }
      };
    });
    showMarkersPanel.add(showMarkersCheckBox);
    showMarkersPanel.add(Box.createHorizontalGlue());
    rootPanel.add(showMarkersPanel);    
    
    /* centrer panel*/
    JPanel centerPanel = new JPanel();
    centerPanel.setLayout(new BoxLayout(centerPanel, BoxLayout.X_AXIS));
    
    /* viewMode panel */
    JPanel viewModePanel = new JPanel();
    viewModePanel.setLayout(new BoxLayout(viewModePanel, BoxLayout.Y_AXIS));
    viewModePanel.setBorder(BorderFactory.createTitledBorder(BorderFactory.createEtchedBorder(), "View Mode"));
    ButtonGroup viewModeGroup = new ButtonGroup();
    
    pianorollButton = new JRadioButton("pianoroll   ");
    viewModeGroup.add(pianorollButton);
    pianorollButton.addItemListener( new ItemListener(){
      public void itemStateChanged(ItemEvent e)
      {
        if(!updating)
        {
          if(e.getStateChange() == ItemEvent.SELECTED)
          {
            trackEditor.setViewMode( MidiTrackEditor.PIANOROLL_VIEW);
            if(((Integer)track.getProperty("rangeMode")).intValue() == MidiTrackEditor.USED_RANGE)
            {
              track.setProperty("maximumPitch", new Integer(((MidiTrackEditor)trackEditor).getMaximumPitchInTrack()));
              track.setProperty("minimumPitch", new Integer(((MidiTrackEditor)trackEditor).getMinimumPitchInTrack()));
            }
          }
        }
      }
    });
    viewModePanel.add(pianorollButton);
    
    stavesButton = new JRadioButton("staves   ");
    viewModeGroup.add(stavesButton);
    stavesButton.addItemListener( new ItemListener(){
      public void itemStateChanged(ItemEvent e)
      {
        if(!updating)
        {
          if(e.getStateChange() == ItemEvent.SELECTED)
          {
            trackEditor.setViewMode( MidiTrackEditor.NMS_VIEW);
            if(((Integer)track.getProperty("rangeMode")).intValue() == MidiTrackEditor.USED_RANGE)
            {
              track.setProperty("maximumPitch", new Integer(((MidiTrackEditor)trackEditor).getMaximumPitchInTrack()));
              track.setProperty("minimumPitch", new Integer(((MidiTrackEditor)trackEditor).getMinimumPitchInTrack()));
            }
          }
        }
      }
    });
    viewModePanel.add(stavesButton);
    
    centerPanel.add(viewModePanel);
    
    /* rangeMode panel */
    JPanel rangeModePanel = new JPanel();
    rangeModePanel.setLayout(new BoxLayout(rangeModePanel, BoxLayout.Y_AXIS));
    rangeModePanel.setBorder(BorderFactory.createTitledBorder(BorderFactory.createEtchedBorder(), "Range Mode"));
    ButtonGroup rangeModeGroup = new ButtonGroup();
    
    usedRangeButton = new JRadioButton("used        ");
    rangeModeGroup.add(usedRangeButton);
    usedRangeButton.addItemListener( new ItemListener(){
      public void itemStateChanged(ItemEvent e)
      {
        if(!updating)
        {
          if(e.getStateChange() == ItemEvent.SELECTED)
            ((MidiTrackEditor)trackEditor).setRangeMode( MidiTrackEditor.USED_RANGE, true);
        }
      }
    });
    rangeModePanel.add(usedRangeButton);
    
    wholeRangeButton = new JRadioButton("whole       ");
    rangeModeGroup.add(wholeRangeButton);
    wholeRangeButton.addItemListener( new ItemListener(){
      public void itemStateChanged(ItemEvent e)
      {
        if(!updating)
        {
          if(e.getStateChange() == ItemEvent.SELECTED)
            ((MidiTrackEditor)trackEditor).setRangeMode( MidiTrackEditor.WHOLE_RANGE, true);
        }
      }
    });
    rangeModePanel.add(wholeRangeButton);
    
    centerPanel.add(rangeModePanel);
    rootPanel.add(centerPanel);
    
    /* labels panel */
    JPanel labelsPanel = new JPanel();
    labelsPanel.setLayout(new BoxLayout(labelsPanel, BoxLayout.X_AXIS));
    labelsPanel.add(new JLabel(" Show as label  ", JLabel.LEFT));
    
    labelsCombo = new JComboBox( getLabelsVector());
    labelsCombo.addItemListener( new ItemListener(){
      public void itemStateChanged(ItemEvent e)
      {   
        if(!updating)
          trackEditor.setLabelType((String)labelsCombo.getSelectedItem());
      };
    });
    labelsPanel.add(labelsCombo);
    labelsPanel.add(Box.createHorizontalGlue());
    rootPanel.add(labelsPanel);
    
    /* moveToPosition panel */
    if(isInSequence)
    {
      JPanel moveToPanel = new JPanel();
      moveToPanel.setLayout(new BoxLayout(moveToPanel, BoxLayout.X_AXIS));
      moveToPanel.add(new JLabel(" MoveTo Position  ", JLabel.LEFT));
      
      moveToCombo = new JComboBox( getMoveToVector());
      moveToCombo.addItemListener( new ItemListener(){
        public void itemStateChanged(ItemEvent e)
        {
          if(!updating)
          {
            int index = moveToCombo.getSelectedIndex();
            ((FtsSequenceObject)trackEditor.getGraphicContext().getFtsObject()).requestTrackMove( track, index); 
          }
        };
      });
      moveToPanel.add(moveToCombo);
      moveToPanel.add(Box.createHorizontalGlue());
      rootPanel.add(moveToPanel);
    }
    
    getContentPane().add(rootPanel);
    getContentPane().validate();
    
    validate();
    pack();
  }
  
  void update()
  {
    updating = true;
    /* update saveState checkbox */
    saveStateCheckBox.setSelected(track.getFtsTrack().saveEditor);			
    
    /* update showMarkers checkbox */
    showMarkersCheckBox.setSelected(((MidiTrackEditor)trackEditor).getGridMode() == TrackEditor.MEASURES_GRID);			
    
    /* update viewMode */
    switch( trackEditor.getViewMode())
    {
      case MidiTrackEditor.NMS_VIEW:
        stavesButton.setSelected(true);
        break;
      case MidiTrackEditor.PIANOROLL_VIEW:
      default:
        pianorollButton.setSelected(true);
        break;
    }    
    
    /* update rangeMode */
    switch(((Integer)track.getProperty("rangeMode")).intValue())
    {
      case MidiTrackEditor.USED_RANGE:
        usedRangeButton.setSelected(true);
        break;
      case MidiTrackEditor.WHOLE_RANGE:
      default:
        wholeRangeButton.setSelected(true);
        break;
    }    
  
    /* update labels combo */
    labelsCombo.setSelectedItem(((MidiTrackEditor)trackEditor).getLabelType());
    
    /* update moveTo combo */
    if(isInSequence)
    {
      int idx = ((FtsSequenceObject)trackEditor.getGraphicContext().getFtsObject()).getTrackIndex(track);
      moveToCombo.setSelectedIndex(idx);
    }
    
    updating = false;
  }
  
  Vector getMoveToVector()
  {
    Vector vec = new Vector();
    for(int i=0; i < trackEditor.trackCount(); i++)
      vec.addElement(""+i);
    return vec;
  }
  
  Vector getLabelsVector()
  {
    Vector vec = new Vector();
    
    vec.addElement("none");
    for (Enumeration e = track.getFtsTrack().getPropertyNames() ; e.hasMoreElements();)
      vec.addElement((String)e.nextElement());
    return vec;
  }  
  
  public static void inspect(TrackBaseEditor trackEditor, Frame frame, boolean isInSequence, Point position)
  {
    MidiTrackInspector inspector = new MidiTrackInspector(trackEditor, frame, isInSequence);
    inspector.update();
    inspector.setLocation(frame.getLocation().x + position.x, frame.getLocation().y + position.y);
    inspector.requestFocus();
    inspector.setVisible(true);
  }
}





