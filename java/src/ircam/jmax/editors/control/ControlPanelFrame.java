package ircam.jmax.editors.control;

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import java.beans.*;

import javax.swing.*;
import javax.swing.border.*;

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

  private FtsDspControl control;
  private IndicatorWithMemory dacSlipIndicator;
  private IndicatorWithMemory invalidFpeIndicator;
  private IndicatorWithMemory divideByZeroFpeIndicator;
  private IndicatorWithMemory overflowFpeIndicator;

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

  public ControlPanelFrame()
  {
    super( "Control Panel");

    control = Fts.getDspController();

    setSize( 300, 300);
    JPanel panel = new JPanel();
    panel.setBorder( new EmptyBorder( 5, 5, 5, 5));

    panel.setLayout( new BoxLayout( panel, BoxLayout.Y_AXIS));

    dacSlipIndicator = new IndicatorWithMemory("Out of sync", "(dac slip...)");
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

    //
    // Temporary code to show sampling rate and fifo size; it should
    // be a bean property editor field, probabily ...

    JPanel samplingRatePanel = new JPanel();

    samplingRatePanel.setLayout( new BorderLayout());
    samplingRatePanel.setOpaque( false);

    JLabel samplingRateLabel = new JLabel("Sampling Rate: ");
    samplingRateLabel.setHorizontalTextPosition(samplingRateLabel.RIGHT);
    samplingRateLabel.setToolTipText("The current global sampling rate");

    JLabel samplingRateText = new JLabel(control.getSamplingRate().toString());
    samplingRateText.setHorizontalTextPosition(samplingRateText.RIGHT);

    samplingRatePanel.add("West", samplingRateLabel);
    samplingRatePanel.add("East", samplingRateText);

    panel.add(samplingRatePanel);

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

    panel.add(fifoSizePanel);

    getContentPane().add( panel);

    pack();
    validate();

    setVisible(true);
  }
}

