package ircam.jmax.editors.control;

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import java.beans.*;

import com.sun.java.swing.*;
import com.sun.java.swing.border.*;

import ircam.jmax.widgets.*;
import ircam.jmax.fts.*;

public class ControlPanelFrame extends JFrame
{
  static ControlPanelFrame controlPanel = null;

  public static ControlPanelFrame open()
  {
    if (controlPanel == null)
      controlPanel = new ControlPanelFrame();

    controlPanel.setVisible(true);

    return controlPanel;
  }

  private DspControl control;
  private IndicatorWithMemory dacSlipIndicator;
  private IndicatorWithMemory invalidFpeIndicator;
  private IndicatorWithMemory divideByZeroFpeIndicator;
  private IndicatorWithMemory overflowFpeIndicator;

  class DspControlAdapter implements PropertyChangeListener
  {
    String prop;
    IndicatorWithMemory ind;

    DspControlAdapter(String prop, DspControl control, IndicatorWithMemory ind)
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

  public ControlPanelFrame()
  {
    super( "Control Panel");

    control = (DspControl) Fts.newRemoteData("dspcontrol_data", null);

    setSize( 300, 300);
    JPanel panel = new JPanel();
    panel.setBorder( new EmptyBorder( 5, 5, 5, 5));

    panel.setLayout( new BoxLayout( panel, BoxLayout.Y_AXIS));

    dacSlipIndicator = new IndicatorWithMemory("Dac Slip");
    panel.add(dacSlipIndicator);
    new DspControlAdapter("dacSlip", control, dacSlipIndicator);

    invalidFpeIndicator = new IndicatorWithMemory("Invalid Operand");
    panel.add(invalidFpeIndicator);
    new DspControlAdapter("invalidFpe", control, invalidFpeIndicator);

    divideByZeroFpeIndicator = new IndicatorWithMemory("Division By Zero");
    panel.add(divideByZeroFpeIndicator);
    new DspControlAdapter("divideByZeroFpe", control, divideByZeroFpeIndicator);

    overflowFpeIndicator = new IndicatorWithMemory("OverFlow");
    panel.add(overflowFpeIndicator);
    new DspControlAdapter("overflowFpe", control, overflowFpeIndicator);

    getContentPane().add( panel);

    pack();
    validate();

    setVisible(true);
  }
}

