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
import java.awt.event.*;
import java.beans.*;

import javax.swing.*;
import javax.swing.border.*;
import ircam.jmax.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.widgets.*;
import ircam.jmax.fts.*;

public class ControlPanel extends JPanel
{
  private JPanel debugPanel;

  private FtsDspControl control;
  private IndicatorWithMemory dacSlipIndicator;
  private IndicatorWithMemory fpeIndicator;

  private IndicatorWithMemory denormalizedFpeIndicator;
  private JToggleButton dspOnButton;
  private JButton  dspPrintButton;

  // (fd) This lock prevents a "setSelected" ont the dspOnButton to cause
  // "itemStateChanged" to repropagates the value to FTS
  private boolean lock = false;


  class DspControlAdapter implements PropertyChangeListener
  {
    String prop;
    IndicatorWithMemory ind;

    DspControlAdapter(String prop, FtsDspControl control, IndicatorWithMemory ind)
    {
      this.prop = prop;
      this.ind  = ind;
      control.addPropertyChangeListener(this);
    }

    public void propertyChange(PropertyChangeEvent event)
    {
      if (prop.equals(event.getPropertyName()))
	if (((Integer) event.getNewValue()).intValue() > 0)
	  ind.setValue(true);
	else
	  ind.setValue(false);
    }
  }

  class DspOnControlAdapter implements PropertyChangeListener
  {
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

  public ControlPanel(Fts fts)
  {
    setSize( 300, 300);
    setLayout( new BoxLayout( this, BoxLayout.X_AXIS));

    JPanel indicatorsPanel = new JPanel();
    indicatorsPanel.setBorder( BorderFactory.createEtchedBorder());
    indicatorsPanel.setLayout( new BoxLayout( indicatorsPanel, BoxLayout.X_AXIS));

    fpeIndicator = new IndicatorWithMemory("FPE", "Floating Point Exceptions");
    fpeIndicator.setAlignmentY(Component.CENTER_ALIGNMENT);
    fpeIndicator.setPreferredSize(new Dimension(200, 23));    
    fpeIndicator.setMaximumSize(new Dimension(200, 23));    
    indicatorsPanel.add(fpeIndicator);

    indicatorsPanel.add( Box.createRigidArea(new Dimension(10, 0)));

    dacSlipIndicator = new IndicatorWithMemory("Out of sync", "Dac Slip");
    dacSlipIndicator.setAlignmentY(Component.CENTER_ALIGNMENT);    
    dacSlipIndicator.setPreferredSize(new Dimension(100, 23));    
    dacSlipIndicator.setMaximumSize(new Dimension(100, 23));    
    indicatorsPanel.add(dacSlipIndicator);

    add( indicatorsPanel);

    add( Box.createHorizontalGlue());

    /*JPanel samplingRatePanel = new JPanel();

      samplingRatePanel.setLayout( new BorderLayout());
      samplingRatePanel.setOpaque( false);

      JLabel samplingRateLabel = new JLabel("Sampling Rate: ");
      samplingRateLabel.setHorizontalTextPosition(samplingRateLabel.RIGHT);
      samplingRateLabel.setToolTipText("The current global sampling rate");

      JLabel samplingRateText = new JLabel(control.getSamplingRate().toString());
      samplingRateText.setHorizontalTextPosition(samplingRateText.RIGHT);

      samplingRatePanel.add("West", samplingRateLabel);
      samplingRatePanel.add("East", samplingRateText);

      dspPanel.add(samplingRatePanel);
      
      JPanel fifoSizePanel = new JPanel();

      fifoSizePanel.setLayout( new BorderLayout());
      fifoSizePanel.setOpaque( false);

      JLabel fifoSizeLabel = new JLabel("Audio Buffer: ");
      fifoSizeLabel.setHorizontalTextPosition(fifoSizeLabel.RIGHT);
      fifoSizeLabel.setToolTipText("The current audio buffer size (latency), in samples");

      JLabel fifoSizeText = new JLabel(control.getFifoSize().toString());
      fifoSizeText.setHorizontalTextPosition(fifoSizeText.RIGHT);

      fifoSizePanel.add("West", fifoSizeLabel);
      fifoSizePanel.add("East", fifoSizeText);

      dspPanel.add(fifoSizePanel);
      dspPanel.add( Box.createVerticalGlue());*/
    JLabel dspLabel = new JLabel("DSP");
    dspLabel.setPreferredSize(new Dimension(35, 20));   
    dspLabel.setMaximumSize(new Dimension(35, 20));   
    dspLabel.setAlignmentY(Component.CENTER_ALIGNMENT);    
    add(dspLabel);

    dspOnButton = new JToggleButton(Icons.get( "_dsp_off_"));
    dspOnButton.setDoubleBuffered( false);
    dspOnButton.setMargin( new Insets(0,0,0,0));
    dspOnButton.setSelectedIcon( Icons.get( "_dsp_on_"));
    dspOnButton.setPreferredSize(new Dimension(25, 25));   
    dspOnButton.setFocusPainted( false);  
    dspOnButton.setAlignmentY(Component.CENTER_ALIGNMENT);    

    add(dspOnButton);

    if (MaxApplication.getProperty("debug") != null)
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

  public void init(Fts fts)
  {
      control = fts.getDspController();
      
      new DspControlAdapter("invalidFpe", control, fpeIndicator);
      new DspControlAdapter("divideByZeroFpe", control, fpeIndicator);
      new DspControlAdapter("overflowFpe", control, fpeIndicator);
      
      new DspControlAdapter("dacSlip", control, dacSlipIndicator);
      dspOnButton.setSelected(control.getDspOn().booleanValue());
      new DspOnControlAdapter("dspOn", control, dspOnButton);

      dspOnButton.addItemListener(new ItemListener() {
	      public void itemStateChanged(ItemEvent e) {
		  
		  if ( !lock)
		      {
			  if (e.getStateChange() == ItemEvent.DESELECTED)
			      control.setDspOn(Boolean.FALSE);
			  else  if (e.getStateChange() == ItemEvent.SELECTED)
			      control.setDspOn(Boolean.TRUE);
		      }
	      }});
      if (MaxApplication.getProperty("debug") != null)
	  {
	      dspPrintButton.addActionListener( new ActionListener() {
		      public  void actionPerformed( ActionEvent e)
		      {
			  control.dspPrint();
		      }
		  });  
	  }
  }
}







