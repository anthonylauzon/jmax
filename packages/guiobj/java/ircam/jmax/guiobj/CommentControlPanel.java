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
// Authors: Riccardo Borghesi, Francois Dechelle, Norbert Schnell.
// 

package ircam.jmax.guiobj;

import java.util.*;
import java.awt.*;
import java.awt.event.*;

//import javax.swing.*;
import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JColorChooser;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSeparator;
import javax.swing.JSlider;

//import javax.swing.event.*;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;
import ircam.jmax.widgets.*;

import ircam.jmax.editors.patcher.objects.*;

public class CommentControlPanel extends JPanel implements ObjectControlPanel
{
  GraphicObject target = null;
  JButton colorButton;
  JLabel alphaLabel;
  JSlider transpSlider;

  public CommentControlPanel()
  {
    super();
    setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));

    /*************** color ******************************/
    JLabel colorLabel = new JLabel("Background Color", JLabel.CENTER);
    colorLabel.setForeground(Color.black);
    colorButton = new JButton();
    colorButton.setPreferredSize(new Dimension(20, 20));
    colorButton.setMaximumSize(new Dimension(20, 20));
    colorButton.setMinimumSize(new Dimension(20, 20));
    colorButton.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  Comment obj = (Comment)target;
	  Color currColor = obj.getColor();
	  Color newColor = JColorChooser.showDialog( null,"Choose Background Color", currColor);
	  
	  if( newColor != null && newColor != currColor){
	    obj.setColor( newColor);
	    colorButton.setBackground( newColor);
	    colorButton.repaint();
	  }
	}
      });

    JPanel colorPanel = new JPanel();
    colorPanel.setPreferredSize( new Dimension(150, 20));
    colorPanel.setLayout( new BoxLayout( colorPanel, BoxLayout.X_AXIS));    
    colorPanel.add( Box.createRigidArea( new Dimension(20, 0)));    
    colorPanel.add( colorLabel);
    colorPanel.add( Box.createHorizontalGlue());    
    colorPanel.add( colorButton);
    colorPanel.add( Box.createRigidArea(new Dimension(5, 0)));    
    colorPanel.validate(); 

    add(colorPanel);

    add( new JSeparator());

    /*********************** transparency *********************/
    JLabel transpLabel = new JLabel("Opacity", JLabel.CENTER);
    transpLabel.setForeground(Color.black);
    alphaLabel = new JLabel("", JLabel.CENTER);
    
    JPanel transpLabelBox = new JPanel();
    transpLabelBox.setLayout(new BoxLayout( transpLabelBox, BoxLayout.X_AXIS));
    transpLabelBox.add( Box.createRigidArea(new Dimension(20, 0)));    
    transpLabelBox.add( transpLabel);    
    transpLabelBox.add( Box.createHorizontalGlue());    
    transpLabelBox.add( alphaLabel);    
    transpLabelBox.add( Box.createRigidArea(new Dimension(5, 0)));    
    
    transpSlider = new JSlider(JSlider.HORIZONTAL, 0, 255, 255);
    transpSlider.setBorder(BorderFactory.createEmptyBorder(0,0,0,10));
    transpSlider.addChangeListener(new ChangeListener(){
	public void stateChanged(ChangeEvent e) {
	  JSlider source = (JSlider)e.getSource();
	  int transp = source.getValue();
	    
	  if(!source.getValueIsAdjusting())
	    {
	      if((target != null)&&(((Comment)target).getTransparency()!= transp))
		((Comment)target).setTransparency( transp);
	    }
      
	  alphaLabel.setText(""+transp);
	}
      });
    JPanel transpPanel = new JPanel();
    transpPanel.setLayout( new BoxLayout( transpPanel, BoxLayout.Y_AXIS));
    transpPanel.add( transpLabelBox);    
    transpPanel.add( transpSlider);        
    transpPanel.validate();

    add(transpPanel);

    add( new JSeparator());

    validate();
    transpPanel.validate();//???
  }

  public void update(GraphicObject obj)
  {
    target = obj;
    colorButton.setBackground( ((Comment)obj).getColor());
    transpSlider.setValue( ((Comment)obj).getTransparency());
  }

  public void done()
  {
  }
}











