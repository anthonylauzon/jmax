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

package ircam.jmax.guiobj;

import java.util.*;
import java.awt.*;
import java.awt.event.*;

//import javax.swing.*;
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

import ircam.jmax.editors.patcher.objects.*;

//
// The graphic pop-up menu used to change the number of an inlet or an outlet in a subpatcher.
//

public class BangControlPanel extends JPanel implements ObjectControlPanel
{
  GraphicObject target = null;
  JSlider durationSlider;
  JLabel durationLabel;
  JButton colorButton;

  public BangControlPanel()
  {
    super();
    setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));

    JLabel colorLabel = new JLabel("Flash Color", JLabel.CENTER);    
    colorLabel.setForeground(Color.black);
    
    colorButton = new JButton();
    colorButton.setPreferredSize(new Dimension(60, 20));
    colorButton.setMaximumSize(new Dimension(60, 20));
    colorButton.setMinimumSize(new Dimension(60, 20));
    colorButton.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  Color color = JColorChooser.showDialog(null,"Choose Bang Color", Color.yellow);
	  
	  if(color != null && color !=  ((Bang)target).getColor())
	    {
	      colorButton.setBackground( color);
	      colorButton.repaint();
	      ((Bang)target).setColor(color);
	    }
	}
      });

    JPanel colorBox = new JPanel();
    colorBox.setLayout( new BoxLayout( colorBox, BoxLayout.X_AXIS));
    colorBox.add( Box.createRigidArea( new Dimension( 20, 0)));    
    colorBox.add( colorLabel);    
    colorBox.add( Box.createHorizontalGlue());    
    colorBox.add( colorButton);    
    colorBox.add( Box.createRigidArea( new Dimension( 6, 0)));    

    JLabel titleLabel = new JLabel("Flash Duration", JLabel.CENTER);
    titleLabel.setForeground(Color.black);
    durationLabel = new JLabel("", JLabel.CENTER);

    JPanel labelBox = new JPanel();
    labelBox.setLayout(new BoxLayout(labelBox, BoxLayout.X_AXIS));
    labelBox.add(Box.createRigidArea(new Dimension(20, 0)));    
    labelBox.add(titleLabel);    
    labelBox.add(Box.createHorizontalGlue());    
    labelBox.add(durationLabel);    
    labelBox.add(Box.createRigidArea(new Dimension(20, 0)));    

    durationSlider = new JSlider(JSlider.HORIZONTAL, 10, 500, 125);
    durationSlider.addChangeListener(new ChangeListener(){
	public void stateChanged(ChangeEvent e) {
	  JSlider source = (JSlider)e.getSource();
	  int duration = (int)source.getValue();
	    
	  if(!source.getValueIsAdjusting())
	    {
	      if((target!=null)&&(((FtsBangObject)target.getFtsObject()).getFlashDuration()!=duration))
		((FtsBangObject)target.getFtsObject()).setFlashDuration(duration);
	    }
      
	  durationLabel.setText(""+(int)duration);
	}
      });

    add( colorBox);
    add( new JSeparator());
    add( labelBox);    
    add( durationSlider);        
    validate();
  }

  public void update(GraphicObject obj)
  {
    target = obj;
    int duration = ((FtsBangObject)obj.getFtsObject()).getFlashDuration();
    durationSlider.setValue(duration);
    durationLabel.setText(""+duration);    

    colorButton.setBackground( ((Bang)target).getColor());
  }
  public void done(){}
}











