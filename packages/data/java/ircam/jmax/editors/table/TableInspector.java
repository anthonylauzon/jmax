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
  JRadioButton filledViewButton, pointsViewButton, linesViewButton;
  JButton foregroundButton, backgroundButton;
  boolean updating = false;
  
  public TableInspector(TableDisplay editor, Frame frame)
  {
    super(frame, "Table Inspector", true);
    setResizable(false);
    
    tableEditor = editor;
    
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
    maximumField.setPreferredSize(new Dimension(120, 28));  
    maximumField.setMaximumSize(new Dimension(120, 28));
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
            tableEditor.getGraphicContext().getFtsObject().requestSetRange( tableEditor.getGraphicContext().getVerticalMinValue(), value);
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
          if( tableEditor.getGraphicContext().getVerticalMinimum() != value)
          {
            tableEditor.panel.setMinimumValue(value);
            tableEditor.getGraphicContext().getFtsObject().requestSetRange( value, tableEditor.getGraphicContext().getVerticalMinValue());
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
    backgroundButton.setPreferredSize(new Dimension(120, 28));
    backgroundButton.setMinimumSize(new Dimension(120, 28));
    backgroundButton.setSize(new Dimension(120, 28));
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
    foregroundButton.setPreferredSize(new Dimension(120, 28));
    foregroundButton.setMinimumSize(new Dimension(120, 28));
    foregroundButton.setSize(new Dimension(120, 28));
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
    
    updating = false;
  }
    
  public static void inspect(TableDisplay tableEditor, Frame frame, Point position)
  {
    TableInspector inspector = new TableInspector(tableEditor, frame);
    inspector.update();
    inspector.setLocation(frame.getLocation().x + position.x, frame.getLocation().y + position.y);
    inspector.requestFocus();
    inspector.setVisible(true);
  }
}





