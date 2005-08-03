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

public class MonoTrackInspector extends JDialog 
{
  JTextField maximumField, minimumField;
  Track track;
  TrackBaseEditor trackEditor;
  JCheckBox saveStateCheckBox;
  JRadioButton peaksViewButton, stepsViewButton, breakpointsViewButton;
  JComboBox moveToCombo;
  boolean isInSequence;
  boolean updating = false;
  
  public MonoTrackInspector(TrackBaseEditor editor, Frame frame, boolean isInSequence)
  {
    super(frame, "Track Inspector", true);
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
    
    /* centrer panel*/
    JPanel centerPanel = new JPanel();
    centerPanel.setLayout(new BoxLayout(centerPanel, BoxLayout.X_AXIS));
    
    /* viewMode panel */
    JPanel viewModePanel = new JPanel();
    viewModePanel.setLayout(new BoxLayout(viewModePanel, BoxLayout.Y_AXIS));
    viewModePanel.setBorder(BorderFactory.createTitledBorder(BorderFactory.createEtchedBorder(), "View Mode"));
    ButtonGroup viewModeGroup = new ButtonGroup();
    
    peaksViewButton = new JRadioButton("peaks");
    viewModeGroup.add(peaksViewButton);
    peaksViewButton.addItemListener( new ItemListener(){
      public void itemStateChanged(ItemEvent e)
      {
        if(!updating)
          if(e.getStateChange() == ItemEvent.SELECTED)
            trackEditor.setViewMode(MonoTrackEditor.PEAKS_VIEW);
      }
    });
    viewModePanel.add(peaksViewButton);
    
    stepsViewButton = new JRadioButton("steps");
    viewModeGroup.add(stepsViewButton);
    stepsViewButton.addItemListener( new ItemListener(){
      public void itemStateChanged(ItemEvent e)
      {
        if(!updating)
          if(e.getStateChange() == ItemEvent.SELECTED)
            trackEditor.setViewMode(MonoTrackEditor.STEPS_VIEW);
      }
    });
    viewModePanel.add(stepsViewButton);
    
    breakpointsViewButton = new JRadioButton("breakpoints");
    viewModeGroup.add(breakpointsViewButton);
    breakpointsViewButton.addItemListener( new ItemListener(){
      public void itemStateChanged(ItemEvent e)
      {
        if(!updating)
          if(e.getStateChange() == ItemEvent.SELECTED)
            trackEditor.setViewMode(MonoTrackEditor.BREAK_POINTS_VIEW);
      }
    });
    viewModePanel.add(breakpointsViewButton); 
    
    centerPanel.add(viewModePanel);
    
    /* rangeMode panel */
    JPanel rangePanel = new JPanel();
    rangePanel.setLayout(new BoxLayout(rangePanel, BoxLayout.Y_AXIS));
    rangePanel.setBorder(BorderFactory.createTitledBorder(BorderFactory.createEtchedBorder(), "Range"));
  
    /* maximum panel */
    JPanel maximumPanel = new JPanel();
    maximumPanel.setLayout(new BoxLayout(maximumPanel, BoxLayout.X_AXIS));
    maximumPanel.add(new JLabel("max ", JLabel.LEFT));
    
    maximumField = new JTextField();
    maximumField.setBorder(BorderFactory.createEtchedBorder());
    maximumField.setEditable(true);     
    maximumField.setPreferredSize(new Dimension(120, 28));  
    maximumField.setMaximumSize(new Dimension(120, 28));
    maximumField.addKeyListener(new KeyListener(){
      public void keyPressed(KeyEvent e){
	      float value;
	      if(e.getKeyCode()==KeyEvent.VK_ENTER)
        {
		      try { 
            value = Float.valueOf(maximumField.getText()).floatValue(); // parse int
		      } catch (NumberFormatException exc) {
            System.err.println("Error:  invalid number format!");
            return;
		      }
          trackEditor.setMaximumValue(value);
        }  
      }
      public void keyReleased(KeyEvent e){}
      public void keyTyped(KeyEvent e){}
    });
    
    maximumPanel.add(maximumField);
    rangePanel.add(maximumPanel);
    rangePanel.add(Box.createVerticalGlue());
    
    /* minimum panel */
    JPanel minimumPanel = new JPanel();
    minimumPanel.setLayout(new BoxLayout(minimumPanel, BoxLayout.X_AXIS));
    minimumPanel.add(new JLabel("min  ", JLabel.LEFT));
    
    minimumField = new JTextField();
    minimumField.setBorder(BorderFactory.createEtchedBorder());
    minimumField.setEditable(true);
    minimumField.setPreferredSize(new Dimension(120, 28));  
    minimumField.setMaximumSize(new Dimension(120, 28));
    minimumField.addKeyListener(new KeyListener(){
      public void keyPressed(KeyEvent e){
	      float value;
	      if(e.getKeyCode()==KeyEvent.VK_ENTER)
        {
		      try { 
            value = Float.valueOf(minimumField.getText()).floatValue(); // parse float
		      } catch (NumberFormatException exc) {
            System.err.println("Error:  invalid number format!");
            return;
		      }
          trackEditor.setMinimunValue(value);
        }  
      }
      public void keyReleased(KeyEvent e){}
      public void keyTyped(KeyEvent e){}
    });
    
    minimumPanel.add(minimumField);
    rangePanel.add(minimumPanel);    
    
    centerPanel.add(rangePanel);
    rootPanel.add(centerPanel);
    
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
    
    /* update viewMode */
    switch( trackEditor.getViewMode())
    {
      case MonoTrackEditor.STEPS_VIEW:
        stepsViewButton.setSelected(true);
        break;
      case MonoTrackEditor.BREAK_POINTS_VIEW:
        breakpointsViewButton.setSelected(true);
        break;
      case MonoTrackEditor.PEAKS_VIEW:
      default:
        peaksViewButton.setSelected(true);
        break;
    }    
    
    /* update range */
    maximumField.setText(""+((Float)track.getProperty("maximumValue")).floatValue());
    minimumField.setText(""+((Float)track.getProperty("minimumValue")).floatValue());    
  
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
  
  public static void inspect(TrackBaseEditor trackEditor, Frame frame, boolean isInSequence, Point position)
  {
    MonoTrackInspector inspector = new MonoTrackInspector(trackEditor, frame, isInSequence);
    inspector.update();
    inspector.setLocation(frame.getLocation().x + position.x, frame.getLocation().y + position.y);
    inspector.requestFocus();
    inspector.setVisible(true);
  }
}





