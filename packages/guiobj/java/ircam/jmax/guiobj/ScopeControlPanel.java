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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.guiobj;

import java.util.*;
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;

import ircam.jmax.editors.patcher.objects.*;

//
// The graphic pop-up menu used to change the number of an inlet or an outlet in a subpatcher.
//

public class ScopeControlPanel extends JPanel implements ObjectControlPanel
{
  GraphicObject target = null;
  JSlider periodSlider, thresholdSlider;
  JLabel periodLabel, thresholdLabel;
  JRadioButton thresholdItem, autoItem, offItem;    

  public ScopeControlPanel()
  {
    super();
    setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));

    JLabel titleLabel = new JLabel("Period", JLabel.CENTER);
    titleLabel.setForeground(Color.black);
    periodLabel = new JLabel("", JLabel.CENTER);

    JPanel labelBox = new JPanel();
    labelBox.setLayout(new BoxLayout(labelBox, BoxLayout.X_AXIS));
    labelBox.add(Box.createRigidArea(new Dimension(20, 0)));    
    labelBox.add(titleLabel);    
    labelBox.add(Box.createHorizontalGlue());    
    labelBox.add(periodLabel);    
    labelBox.add(Box.createRigidArea(new Dimension(20, 0)));    

    periodSlider = new JSlider(JSlider.HORIZONTAL, 50, 2000, 100);
    periodSlider.setBorder(BorderFactory.createEmptyBorder(0,0,0,10));
    periodSlider.addChangeListener(new ChangeListener(){
	public void stateChanged(ChangeEvent e) {
	  JSlider source = (JSlider)e.getSource();
	  float period = (float)source.getValue();
	    
	  if(!source.getValueIsAdjusting())
	    {
	      if((target!=null)&&(((FtsScopeObject)target.getFtsObject()).getPeriod()!=period))
		((FtsScopeObject)target.getFtsObject()).setPeriod(period);
	    }
      
	  periodLabel.setText(""+(int)period);
	}
      });

    JPanel periodPanel = new JPanel();
    periodPanel.setLayout(new BoxLayout(periodPanel, BoxLayout.Y_AXIS));
    periodPanel.add(labelBox);    
    periodPanel.add(periodSlider);        
    periodPanel.validate();
    add(periodPanel);

    add(new JSeparator());

    ButtonGroup thresholdButtonGroup = new ButtonGroup();
    JPanel thresholdPanel = new JPanel();
    thresholdPanel.setLayout(new BoxLayout(thresholdPanel, BoxLayout.Y_AXIS));
    
    JLabel triggerLabel = new JLabel("Trigger", JLabel.LEFT);
    triggerLabel.setForeground(Color.black);
    JPanel labelPanel = new JPanel();
    labelPanel.setLayout(new BoxLayout(labelPanel, BoxLayout.X_AXIS));
    labelPanel.add(Box.createRigidArea(new Dimension(20, 0)));    
    labelPanel.add(triggerLabel);
    labelPanel.add(Box.createHorizontalGlue());
    thresholdPanel.add(labelPanel);

    autoItem = new JRadioButton("auto");
    autoItem.addItemListener(new ItemListener(){
	    public void itemStateChanged(ItemEvent e)
	    {
		if(e.getStateChange() == ItemEvent.SELECTED)
		    ((FtsScopeObject)target.getFtsObject()).
			setThreshold(FtsScopeObject.THRESHOLD_AUTO);
	    }
	});
    thresholdButtonGroup.add(autoItem);
    JPanel autoPanel = new JPanel();
    autoPanel.setLayout(new BoxLayout(autoPanel, BoxLayout.X_AXIS));
    autoPanel.add(Box.createRigidArea(new Dimension(20, 0)));    
    autoPanel.add(autoItem);
    autoPanel.add(Box.createHorizontalGlue());
    thresholdPanel.add(autoPanel);

    offItem = new JRadioButton("off");
    offItem.addItemListener(new ItemListener(){
	    public void itemStateChanged(ItemEvent e)
	    {
		if(e.getStateChange() == ItemEvent.SELECTED)
		    ((FtsScopeObject)target.getFtsObject()).
			setThreshold(FtsScopeObject.THRESHOLD_OFF);
	    }
	});
    thresholdButtonGroup.add(offItem);    
    JPanel offPanel = new JPanel();
    offPanel.setLayout(new BoxLayout(offPanel, BoxLayout.X_AXIS));
    offPanel.add(Box.createRigidArea(new Dimension(20, 0)));    
    offPanel.add(offItem);
    offPanel.add(Box.createHorizontalGlue());
    thresholdPanel.add(offPanel);

    thresholdItem = new JRadioButton("threshold");
    thresholdItem.addItemListener(new ItemListener(){
	    public void itemStateChanged(ItemEvent e)
	    {
		if(e.getStateChange() == ItemEvent.SELECTED)
		    {
			thresholdSlider.setEnabled(true);
			float th = thresholdSlider.getValue()/(float)100.0;
			thresholdLabel.setText(""+th);
			((FtsScopeObject)target.getFtsObject()).setThreshold(th);
		    }		
		else
		    {
			thresholdSlider.setEnabled(false);
			thresholdLabel.setText("");
		    }
	    }
	});
    thresholdButtonGroup.add(thresholdItem); 

    thresholdLabel  = new JLabel("", JLabel.CENTER);
    thresholdLabel.setPreferredSize(new Dimension(30, 15));
    
    JPanel thresholdBox = new JPanel();
    thresholdBox.setPreferredSize(new Dimension(230, 15));
    thresholdBox.setLayout(new BoxLayout(thresholdBox, BoxLayout.X_AXIS));
    thresholdBox.add(Box.createRigidArea(new Dimension(20, 0)));    
    thresholdBox.add(thresholdItem);    
    thresholdBox.add(Box.createHorizontalGlue());    
    thresholdBox.add(thresholdLabel);    
    thresholdBox.add(Box.createRigidArea(new Dimension(20, 0)));    
    
    thresholdPanel.add(thresholdBox);

    thresholdSlider = new JSlider(JSlider.HORIZONTAL, 1, 99, 1);
    thresholdSlider.setBorder(BorderFactory.createEmptyBorder(0,0,0,10));
    thresholdSlider.addChangeListener(new ChangeListener(){
	public void stateChanged(ChangeEvent e) {
	  JSlider source = (JSlider)e.getSource();
	  float th = source.getValue()/(float)100.0;

	  if(!source.getValueIsAdjusting())
	    {
		if((target!=null))
		    ((FtsScopeObject)target.getFtsObject()).setThreshold(th);
	    }      
	  thresholdLabel.setText(""+th);
	}
      });
    thresholdPanel.add(thresholdSlider);

    add(thresholdPanel);
    validate();
  }

  public void update(GraphicObject obj)
  {
    target = obj;
    float period = ((FtsScopeObject)obj.getFtsObject()).getPeriod();
    periodSlider.setValue((int)period);
    periodLabel.setText(""+(int)period);    

    float threshold = ((FtsScopeObject)obj.getFtsObject()).getThreshold();
    if(threshold==FtsScopeObject.THRESHOLD_AUTO)
	autoItem.setSelected(true);
    else if(threshold==FtsScopeObject.THRESHOLD_OFF)
	offItem.setSelected(true);
    else
	{
	    thresholdItem.setSelected(true);
	    thresholdSlider.setValue((int)(threshold*100));
	}
    revalidate();
  }
}











