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

package ircam.jmax.editors.table;

import java.awt.*;
import javax.swing.*;
import java.util.*;
import java.awt.event.*;
import ircam.jmax.editors.table.renderers.*;
/**
* */

public class TableInspector extends JDialog 
{
  JTextField maximumField, minimumField;
  TableDisplay tableEditor;
  FtsTableObject ftsTableObject;
  JRadioButton filledViewButton, pointsViewButton, linesViewButton;
  JButton foregroundButton, backgroundButton;
  JComboBox typeRefCombo, indexRefCombo;
  JTextField onsetRefField, sizeRefField;
  String type_ref;
  int idx_ref, onset_ref, size_ref;
  //JButton applyRefButton;
  boolean reference_changed = false;
  TableDataListener listener;
  
  boolean updating = false;
  
  public TableInspector(TableDisplay editor, Frame frame)
  {
    super(frame, "Table Inspector", /*true*/false);
    setResizable(false);
    
    tableEditor = editor;
    ftsTableObject = tableEditor.getGraphicContext().getFtsObject();
    
    listener = new TableDataListener(){
      public void valueChanged(int index1, int index2, boolean fromScroll){}
      public void pixelsChanged(int index1, int index2){}
      public void tableSetted(){}
      public void tableCleared(){}
      public void sizeChanged(int size, int oldSize){}
      public void tableUpdated(){}
      public void tableRange(float min_val, float max_val){}
      public void tableReference(int nRowsRef, int nColsRef, String typeRef, int indexRef, int onsetRef, int sizeRef)
      {
        updating = true;
        updateReference();
        updating = false;
      }
    };
    ftsTableObject.addListener( listener);
    
    addWindowListener( new WindowAdapter() {
	    public void windowClosing(WindowEvent e)
		  {
        close();
      }
    });
    
    /* root panel */
    JPanel rootPanel = new JPanel();
    rootPanel.setLayout(new BoxLayout(rootPanel, BoxLayout.Y_AXIS));
    rootPanel.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createEmptyBorder( 2, 2, 2, 2), BorderFactory.createEtchedBorder()));
    
    /* centrer panel*/
    JPanel centerPanel = new JPanel();
    centerPanel.setLayout(new BoxLayout(centerPanel, BoxLayout.X_AXIS));
    
    /* viewMode panel */
    JPanel viewModePanel = new JPanel();
    viewModePanel.setLayout(new BoxLayout(viewModePanel, BoxLayout.Y_AXIS));
    viewModePanel.setBorder(BorderFactory.createTitledBorder(BorderFactory.createEtchedBorder(), "View Mode"));
    ButtonGroup viewModeGroup = new ButtonGroup();
    
    filledViewButton = new JRadioButton("filled");
    viewModeGroup.add(filledViewButton);
    filledViewButton.addActionListener( new Actions.FilledViewAction());
    viewModePanel.add(filledViewButton);
    
    pointsViewButton = new JRadioButton("points");
    viewModeGroup.add(pointsViewButton);
    pointsViewButton.addActionListener( new Actions.PointsViewAction());
    viewModePanel.add(pointsViewButton);
    
    linesViewButton = new JRadioButton("lines");
    viewModeGroup.add(linesViewButton);
    linesViewButton.addActionListener( new Actions.LinesViewAction());
    viewModePanel.add(linesViewButton); 
    
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
    maximumField.setPreferredSize(new Dimension(140, 28));  
    maximumField.addKeyListener(new KeyListener(){
      public void keyPressed(KeyEvent e){
	      float value;
	      if(e.getKeyCode()==KeyEvent.VK_ENTER)
        {
		      try { 
            value = Float.valueOf(maximumField.getText()).floatValue(); // parse float
		      } catch (NumberFormatException exc) {
            System.err.println("Error:  invalid number format!");
            return;
		      }
          if( tableEditor.getGraphicContext().getVerticalMaximum() != value)
          {
            tableEditor.panel.setMaximumValue(value);
            ftsTableObject.requestSetRange( tableEditor.getGraphicContext().getVerticalMinValue(), value);
          }  
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
    minimumField.setPreferredSize(new Dimension(140, 28));  
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
          if( tableEditor.getGraphicContext().getVerticalMinimum() != value)
          {
            tableEditor.panel.setMinimumValue(value);
            ftsTableObject.requestSetRange( value, tableEditor.getGraphicContext().getVerticalMaxValue());
          }  
        }
      }
      public void keyReleased(KeyEvent e){}
      public void keyTyped(KeyEvent e){}
    });
    
    minimumPanel.add(minimumField);
    rangePanel.add(minimumPanel);    
    
    centerPanel.add(rangePanel);
    rootPanel.add(centerPanel);
        
    /* color panel */
    JPanel colorPanel = new JPanel();
    colorPanel.setLayout(new BoxLayout(colorPanel, BoxLayout.Y_AXIS));
    colorPanel.setBorder(BorderFactory.createTitledBorder(BorderFactory.createEtchedBorder(), "Colors"));
    
    JPanel backgroundPanel = new JPanel();
    backgroundPanel.setLayout(new BoxLayout(backgroundPanel, BoxLayout.X_AXIS));
    backgroundPanel.add(new JLabel("background  ", JLabel.LEFT));
    backgroundButton = new JButton(" ");
    backgroundButton.setBorder(BorderFactory.createEtchedBorder());
    backgroundButton.setUI( new javax.swing.plaf.metal.MetalButtonUI());
    backgroundButton.setPreferredSize(new Dimension(140, 28));
    backgroundButton.setMinimumSize(new Dimension(140, 28));
    backgroundButton.setSize(new Dimension(140, 28));
    backgroundButton.addActionListener( new ActionListener(){
      public void actionPerformed(ActionEvent e)
      {
        tableEditor.panel.changeBackgroundColor();
        backgroundButton.setBackground(tableEditor.getRenderer().getBackColor());
        backgroundButton.setForeground(tableEditor.getRenderer().getBackColor());
      }
    });
    backgroundPanel.add(Box.createHorizontalGlue());
    backgroundPanel.add(backgroundButton);
    
    colorPanel.add(backgroundPanel);
    
    JPanel foregroundPanel = new JPanel();
    foregroundPanel.setLayout(new BoxLayout(foregroundPanel, BoxLayout.X_AXIS));
    foregroundPanel.add(new JLabel("foreground  ", JLabel.LEFT));
    foregroundButton = new JButton(" ");
    foregroundButton.setOpaque(true);
    foregroundButton.setBorder(BorderFactory.createEtchedBorder());
    foregroundButton.setUI( new javax.swing.plaf.metal.MetalButtonUI());
    foregroundButton.setPreferredSize(new Dimension(140, 28));
    foregroundButton.setMinimumSize(new Dimension(140, 28));
    foregroundButton.setSize(new Dimension(140, 28));
    foregroundButton.addActionListener( new ActionListener(){
      public void actionPerformed(ActionEvent e)
      {
        tableEditor.panel.changeForegroundColor();
        foregroundButton.setBackground(tableEditor.getRenderer().getForeColor());
        foregroundButton.setForeground(tableEditor.getRenderer().getForeColor());
      }
    });
    
    foregroundPanel.add(Box.createHorizontalGlue());
    foregroundPanel.add(foregroundButton);
    
    colorPanel.add(Box.createVerticalGlue());
    colorPanel.add(foregroundPanel);
    
    rootPanel.add(colorPanel);
    
    if(ftsTableObject.hasReference())
    {
      /* reference panel */
      JPanel referencePanel = new JPanel();
      referencePanel.setLayout(new BoxLayout(referencePanel, BoxLayout.X_AXIS));
      /*type*/
      JPanel typeRefPanel = new JPanel();
      typeRefPanel.setLayout(new BoxLayout(typeRefPanel, BoxLayout.Y_AXIS));
      typeRefPanel.setBorder(BorderFactory.createTitledBorder(BorderFactory.createEmptyBorder(), "type"));
      String[] types = {"col", "row", "diag", "unwrap", "vec"};
      typeRefCombo = new JComboBox(types);
      typeRefCombo.addItemListener( new ItemListener(){
        public void itemStateChanged(ItemEvent e)
        {
          if(!updating)
            setTypeRef((String)e.getItem());
        };
      });
      typeRefPanel.add(typeRefCombo);
      referencePanel.add(typeRefPanel);
      /*index*/
      JPanel indexRefPanel = new JPanel();
      indexRefPanel.setLayout(new BoxLayout(indexRefPanel, BoxLayout.Y_AXIS));
      indexRefPanel.setBorder(BorderFactory.createTitledBorder(BorderFactory.createEmptyBorder(), "index"));
      indexRefCombo = new JComboBox();
      indexRefCombo.addItemListener( new ItemListener(){
        public void itemStateChanged(ItemEvent e)
        {
          if(!updating)
          {
            String sidx = (String)e.getItem();
            int idx = 0;
            try{
              idx = (Integer.valueOf(sidx)).intValue();
            } catch(NumberFormatException exc){}
            setIndexRef(idx);
          }
        };
      });
      indexRefPanel.add(indexRefCombo);
      referencePanel.add(indexRefPanel);
    
      /*onset*/
      JPanel onsetRefPanel = new JPanel();
      onsetRefPanel.setLayout(new BoxLayout(onsetRefPanel, BoxLayout.Y_AXIS));
      onsetRefPanel.setBorder(BorderFactory.createTitledBorder(BorderFactory.createEmptyBorder(), "onset"));
      onsetRefField = new JTextField();
      onsetRefField.setPreferredSize(new Dimension(40, 23));
      onsetRefField.setSize(new Dimension(40, 23));
      onsetRefField.setBorder(BorderFactory.createEtchedBorder());
      onsetRefField.setEditable(true);
      onsetRefField.setNextFocusableComponent(TableInspector.this);
      onsetRefField.addKeyListener(new KeyListener(){
        public void keyPressed(KeyEvent e){
          int onset = 0;
          if(e.getKeyCode()==KeyEvent.VK_ENTER)
          {
            try { 
              onset = Integer.valueOf(onsetRefField.getText()).intValue();
            } catch (NumberFormatException exc) {
              System.err.println("Error:  invalid number format!");
              return;
            }
            setOnsetRef(onset);
            onsetRefField.transferFocus();
          }
        }
        public void keyReleased(KeyEvent e){}
        public void keyTyped(KeyEvent e){}
      });
      onsetRefPanel.add(onsetRefField);
      referencePanel.add(onsetRefPanel);
    
      /*size*/
      JPanel sizeRefPanel = new JPanel();
      sizeRefPanel.setLayout(new BoxLayout(sizeRefPanel, BoxLayout.Y_AXIS));
      sizeRefPanel.setBorder(BorderFactory.createTitledBorder(BorderFactory.createEmptyBorder(), "size"));
      sizeRefField = new JTextField();
      sizeRefField.setSize(new Dimension(40, 23));
      sizeRefField.setPreferredSize(new Dimension(40, 23));
      sizeRefField.setBorder(BorderFactory.createEtchedBorder());
      sizeRefField.setEditable(true);
      sizeRefField.setNextFocusableComponent(TableInspector.this);
      sizeRefField.addKeyListener(new KeyListener(){
        public void keyPressed(KeyEvent e){
          int size = 0;
          if(e.getKeyCode()==KeyEvent.VK_ENTER)
          {
            try { 
              size = Integer.valueOf(sizeRefField.getText()).intValue();
            } catch (NumberFormatException exc) {
              System.err.println("Error:  invalid number format!");
              return;
            }
            setSizeRef(size);          
            sizeRefField.transferFocus();
          }
        }
        public void keyReleased(KeyEvent e){}
        public void keyTyped(KeyEvent e){}
      });
      sizeRefPanel.add(sizeRefField);
      referencePanel.add(sizeRefPanel);
    
      JPanel extRefPanel = new JPanel();
      extRefPanel.setLayout(new BoxLayout(extRefPanel, BoxLayout.Y_AXIS));
      extRefPanel.setBorder(BorderFactory.createTitledBorder(BorderFactory.createEtchedBorder(), "Mat Reference"));
    
      extRefPanel.add(referencePanel);
      
      /*apply button*/
      /*applyRefButton = new JButton("      ok      ");
      applyRefButton.setBorder(BorderFactory.createEtchedBorder());
      applyRefButton.setUI( new javax.swing.plaf.metal.MetalButtonUI());
      applyRefButton.addActionListener( new ActionListener(){
        public void actionPerformed(ActionEvent e)
        {
          SwingUtilities.invokeLater(new Runnable() {
            public void run()
            {
              setReferenceChanged(false);
              ftsTableObject.requestChangeReference(type_ref, idx_ref, onset_ref, size_ref);
            }
          });
        }
      });
      applyRefButton.setEnabled(false);
      applyRefButton.setAlignmentX((float)0.5);
    
      extRefPanel.add(applyRefButton);*/
    
      rootPanel.add(extRefPanel);
    }
    
    getContentPane().add(rootPanel);
    getContentPane().validate();
    
    validate();
    pack();
  }
  
  void update()
  {
    updating = true;		
    
    /* update display mode */
    switch( tableEditor.getDisplayMode())
    {
      case TableRenderer.POINTS: 
        pointsViewButton.setSelected( true);
        break;
      case TableRenderer.LINES:
        linesViewButton.setSelected( true);    
        break;
      default:
      case TableRenderer.FILLED: 
        filledViewButton.setSelected( true);
        break;
    }
    
    /* update range */
    maximumField.setText(""+tableEditor.getGraphicContext().getVerticalMaxValue());
    minimumField.setText(""+tableEditor.getGraphicContext().getVerticalMinValue());    
    
    /* update colors */
    foregroundButton.setBackground(tableEditor.getRenderer().getForeColor());
    foregroundButton.setForeground(tableEditor.getRenderer().getForeColor());
    backgroundButton.setBackground(tableEditor.getRenderer().getBackColor());
    backgroundButton.setForeground(tableEditor.getRenderer().getBackColor());
    
    if(ftsTableObject.hasReference())
      updateReference();   
    updating = false;
  }
  
  void updateReference()
  {
    idx_ref =ftsTableObject.indexRef;
    onset_ref = ftsTableObject.onsetRef;
    size_ref = ftsTableObject.sizeRef;
    
    if(type_ref != ftsTableObject.typeRef)
    {
      type_ref = ftsTableObject.typeRef;
      SwingUtilities.invokeLater(new Runnable() {
        public void run()
	      {            
          initIndexRefCombo();      
          indexRefCombo.setSelectedIndex(idx_ref);
        }
      });
    }
    else
    {
      if(indexRefCombo.getSelectedIndex() != idx_ref)
        indexRefCombo.setSelectedIndex(idx_ref);
    }
    if(typeRefCombo.getSelectedItem() != type_ref)
      typeRefCombo.setSelectedItem(type_ref);
    onsetRefField.setText(""+onset_ref);
    sizeRefField.setText(""+size_ref);
  }
  
  void initIndexRefCombo()
  {    
    if(type_ref.equals("col") || type_ref.equals("vec"))
      indexRefCombo.setModel(new DefaultComboBoxModel(getRefIndexes(ftsTableObject.nColsRef)));
    else if(type_ref.equals("row") || type_ref.equals("diag") || type_ref.equals("unwrap"))
      indexRefCombo.setModel(new DefaultComboBoxModel(getRefIndexes(ftsTableObject.nRowsRef)));
  }
  
  void setTypeRef(String type)
  {
    if(type_ref==null || !type_ref.equals(type))
    {
      type_ref = type;
      initIndexRefCombo();
      if(idx_ref <= indexRefCombo.getItemCount())
        indexRefCombo.setSelectedIndex(idx_ref);
      else
      {
        indexRefCombo.setSelectedIndex(0);
        idx_ref = 0;
      }
      //setReferenceChanged(true);
      ftsTableObject.requestChangeReference(type_ref, idx_ref, onset_ref, size_ref);
    }
  }
  void setIndexRef(int idx)
  {
    if(idx_ref != idx)
    {
      idx_ref = idx;
      //setReferenceChanged(true);
      ftsTableObject.requestChangeReference(type_ref, idx_ref, onset_ref, size_ref);
    }
  }
  void setOnsetRef(int ons)
  {
    if(onset_ref != ons)
    {
      onset_ref = ons;
      /*setReferenceChanged(true);*/
      ftsTableObject.requestChangeReference(type_ref, idx_ref, onset_ref, size_ref);
    }
  }
  void setSizeRef(int size)
  {
    if(size_ref != size)
    {
      size_ref = size;
      //setReferenceChanged(true);
      ftsTableObject.requestChangeReference(type_ref, idx_ref, onset_ref, size_ref);
    }
  }
  
  /*void setReferenceChanged(boolean changed)
  {
    reference_changed = changed;
    applyRefButton.setEnabled(changed);
  }*/
  
  Vector getRefIndexes(int size)
  {
    Vector vec = new Vector();
    for(int i=0; i < size; i++)
      vec.addElement(""+i);
    return vec;
  }
  
  public void close()
  {
    ftsTableObject.removeListener( listener);
    tableEditor.panel.setCurrentInspector(null);
  }
  
  public static void inspect(TableDisplay tableEditor, Frame frame, Point position)
  {
    if(tableEditor.panel.getCurrentInspector() == null)
    {
      TableInspector inspector = new TableInspector(tableEditor, frame);
      tableEditor.panel.setCurrentInspector(inspector);
      inspector.update();
      inspector.setLocation(frame.getLocation().x + position.x, frame.getLocation().y + position.y);
      inspector.requestFocus();
      inspector.setVisible(true);
    }
  }
}





