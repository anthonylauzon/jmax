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

package ircam.jmax.editors.console;

// ^^^^ Probabily the Control Frame should add a field with the
// ^^^^ name of the server

import java.util.*;
import java.awt.*;
import java.io.*;
import java.awt.event.*;
import java.beans.*;

import javax.swing.*;
import javax.swing.border.*;
import ircam.jmax.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.widgets.*;
import ircam.jmax.fts.*;
import ircam.fts.client.*;

public class ControlPanel extends JPanel {
  private JPanel debugPanel;

  private FtsDspControl control;

  private MemoryLed fpeLed;
  private MemoryLed syncLed;

  private JToggleButton dspOnButton;
  private JButton  dspPrintButton;

  // (fd) This lock prevents a "setSelected" ont the dspOnButton to cause
  // "itemStateChanged" to repropagates the value to FTS
  private boolean lock = false;


  class DspControlAdapter implements PropertyChangeListener {
    String prop;
    MemoryLed led;

    DspControlAdapter(String prop, FtsDspControl control, MemoryLed led)
    {
      this.prop = prop;
      this.led  = led;
      control.addPropertyChangeListener(this);
    }

    public void propertyChange(PropertyChangeEvent event)
    {
      if (prop.equals(event.getPropertyName()))
	if (((Integer) event.getNewValue()).intValue() > 0)
	  led.setState(true);
	else
	  led.setState(false);
    }
  }

  class DspOnControlAdapter implements PropertyChangeListener {
    String prop;
    JToggleButton b;

    DspOnControlAdapter(String prop, FtsDspControl control, JToggleButton b)
    {
      this.prop = prop;
      this.b    = b;
      control.addPropertyChangeListener(this);
    }

    public void propertyChange(PropertyChangeEvent event)
    {
      if (prop.equals(event.getPropertyName()))
	{
	  synchronized( b)
	    {
	      lock = true;
	      b.setSelected(((Boolean) event.getNewValue()).booleanValue());
	      lock = false;
	    }
	}
    }
  }

  public ControlPanel()
  {
    setSize( 300, 300);
    setLayout( new BoxLayout( this, BoxLayout.X_AXIS));

    JLabel fpeLabel = new JLabel( "FPE");
    fpeLabel.setAlignmentX( LEFT_ALIGNMENT);
    fpeLabel.setAlignmentY( CENTER_ALIGNMENT);

    fpeLed = new MemoryLed();
    fpeLed.setBorder( BorderFactory.createEtchedBorder());
    fpeLed.setAlignmentX( RIGHT_ALIGNMENT);
    fpeLed.setAlignmentY( CENTER_ALIGNMENT);

    add( fpeLabel);
    add( Box.createRigidArea(new Dimension(7,0)));
    add( fpeLed);

    add( Box.createRigidArea(new Dimension(10, 0)));

    JLabel syncLabel = new JLabel( "Out of Sync");
    syncLabel.setAlignmentX( LEFT_ALIGNMENT);
    syncLabel.setAlignmentY( CENTER_ALIGNMENT);

    syncLed = new MemoryLed();
    syncLed.setBorder( BorderFactory.createEtchedBorder());
    syncLed.setAlignmentX( RIGHT_ALIGNMENT);
    syncLed.setAlignmentY( CENTER_ALIGNMENT);

    add( syncLabel);
    add( Box.createRigidArea(new Dimension(7,0)));
    add( syncLed);

    add( Box.createHorizontalGlue());

    JLabel dspLabel = new JLabel("DSP");
    dspLabel.setPreferredSize(new Dimension(35, 20));   
    dspLabel.setMaximumSize(new Dimension(35, 20));   
    dspLabel.setAlignmentY(Component.CENTER_ALIGNMENT);    
    add(dspLabel);

    dspOnButton = new JToggleButton( JMaxIcons.dspOff);
    dspOnButton.setDoubleBuffered( false);
    dspOnButton.setMargin( new Insets(0,0,0,0));
    dspOnButton.setSelectedIcon( JMaxIcons.dspOn);
    dspOnButton.setPreferredSize(new Dimension(25, 25));   
    dspOnButton.setFocusPainted( false);  
    dspOnButton.setAlignmentY(Component.CENTER_ALIGNMENT);    

    add(dspOnButton);

    if (JMaxApplication.getProperty("debug") != null)
      {
	add( Box.createRigidArea(new Dimension(5,0)));

	debugPanel = new JPanel();
	debugPanel.setLayout( new BoxLayout( debugPanel, BoxLayout.Y_AXIS));
	debugPanel.setBorder(BorderFactory.createTitledBorder("Debug"));
	debugPanel.setAlignmentY(Component.TOP_ALIGNMENT);

	add(debugPanel);

	dspPrintButton = new JButton("Print Dsp Chain");
	debugPanel.add(dspPrintButton);
      }

    validate();
  }

  public void init()
  {
    try
      {
	control = new FtsDspControl();
      }
    catch(IOException e)
      {
	System.err.println("ControlPanel: Error in FtsDspControl creation!");
	e.printStackTrace();
      }

    new DspControlAdapter("invalidFpe", control, fpeLed);
    new DspControlAdapter("divideByZeroFpe", control, fpeLed);
    new DspControlAdapter("overflowFpe", control, fpeLed);
      
    new DspControlAdapter("dacSlip", control, syncLed);
    dspOnButton.setSelected(control.getDspOn());
    new DspOnControlAdapter("dspOn", control, dspOnButton);

    dspOnButton.addItemListener(new ItemListener() {
	public void itemStateChanged(ItemEvent e) {
		  
	  if ( !lock)
	    {
	      if (e.getStateChange() == ItemEvent.DESELECTED)
		control.requestSetDspOn(false);
	      else  if (e.getStateChange() == ItemEvent.SELECTED)
		control.requestSetDspOn(true);
	    }
	}});
    if (JMaxApplication.getProperty("debug") != null)
      {
	dspPrintButton.addActionListener( new ActionListener() {
	    public  void actionPerformed( ActionEvent e)
	    {
	      control.dspPrint();
	    }
	  });  
      }
  }

  public FtsDspControl getDspControl()
  {
    return control;
  }
}
