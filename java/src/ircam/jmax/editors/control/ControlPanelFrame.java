package ircam.jmax.editors.control;

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import java.beans.*;

import javax.swing.*;
import javax.swing.border.*;
import ircam.jmax.*;
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

  private JPanel exceptionPanel;
  private JPanel dspPanel;
  private JPanel optionsPanel;
  private JPanel debugPanel;

  private FtsDspControl control;
  private IndicatorWithMemory dacSlipIndicator;
  private IndicatorWithMemory invalidFpeIndicator;
  private IndicatorWithMemory divideByZeroFpeIndicator;
  private IndicatorWithMemory overflowFpeIndicator;
  private JCheckBox dspOnButton;

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
    JCheckBox b;

    DspOnControlAdapter(String prop, FtsDspControl control, JCheckBox b)
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

  public ControlPanelFrame()
  {
    super( "Control Panel");

    control = Fts.getDspController();

    setSize( 300, 300);
    JPanel panel = new JPanel();
    panel.setBorder( new EmptyBorder( 5, 5, 5, 5));
    panel.setLayout( new BoxLayout( panel, BoxLayout.X_AXIS));

    exceptionPanel = new JPanel();
    exceptionPanel.setLayout( new BoxLayout( exceptionPanel, BoxLayout.Y_AXIS));
    exceptionPanel.setBorder(BorderFactory.createTitledBorder("Exceptions"));
    exceptionPanel.setAlignmentY(Component.TOP_ALIGNMENT);
    panel.add(exceptionPanel);
    panel.add( Box.createRigidArea(new Dimension(5,0)));
    
    invalidFpeIndicator = new IndicatorWithMemory("Invalid Operand");
    exceptionPanel.add(invalidFpeIndicator);
    new DspControlAdapter("invalidFpe", control, invalidFpeIndicator);

    divideByZeroFpeIndicator = new IndicatorWithMemory("Division By Zero");
    exceptionPanel.add(divideByZeroFpeIndicator);
    new DspControlAdapter("divideByZeroFpe", control, divideByZeroFpeIndicator);

    overflowFpeIndicator = new IndicatorWithMemory("OverFlow");
    exceptionPanel.add(overflowFpeIndicator);
    new DspControlAdapter("overflowFpe", control, overflowFpeIndicator);

    //
    // Temporary code to show sampling rate and fifo size; it should
    // be a bean property editor field, probabily ...

    dspPanel = new JPanel();
    dspPanel.setLayout( new BoxLayout( dspPanel, BoxLayout.Y_AXIS));
    dspPanel.setBorder(BorderFactory.createTitledBorder("DSP"));
    dspPanel.setAlignmentY(Component.TOP_ALIGNMENT);
    panel.add(dspPanel);
    panel.add( Box.createRigidArea(new Dimension(5,0)));

    dacSlipIndicator = new IndicatorWithMemory("Out of sync", "(dac slip...)");
    dspPanel.add(dacSlipIndicator);
    new DspControlAdapter("dacSlip", control, dacSlipIndicator);


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

    optionsPanel = new JPanel();
    optionsPanel.setLayout( new BoxLayout( optionsPanel, BoxLayout.Y_AXIS));
    optionsPanel.setBorder(BorderFactory.createTitledBorder("Run Time"));
    optionsPanel.setAlignmentY(Component.TOP_ALIGNMENT);
    panel.add(optionsPanel);

    dspOnButton = new JCheckBox("Dsp On");
    dspOnButton.setHorizontalTextPosition(JCheckBox.LEFT);
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

    optionsPanel.add(dspOnButton);

    if (MaxApplication.getProperty("debug") != null)
      {
	panel.add( Box.createRigidArea(new Dimension(5,0)));

	debugPanel = new JPanel();
	debugPanel.setLayout( new BoxLayout( debugPanel, BoxLayout.Y_AXIS));
	debugPanel.setBorder(BorderFactory.createTitledBorder("Debug"));
	debugPanel.setAlignmentY(Component.TOP_ALIGNMENT);

	panel.add(debugPanel);

	JButton  dspPrintButton;

	dspPrintButton = new JButton("Print Dsp Chain");
	dspPrintButton.addActionListener( new ActionListener() {
	  public  void actionPerformed( ActionEvent e)
	    {
	      control.dspPrint();
	    }
	});

	debugPanel.add(dspPrintButton);
      }

    getContentPane().add( panel);

    pack();
    validate();

    setVisible(true);
  }
}

