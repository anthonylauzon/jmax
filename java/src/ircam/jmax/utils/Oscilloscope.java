package ircam.jmax.utils;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

public class Oscilloscope extends Frame implements  OscillSource{
  MainPanel itsMainPanel;
  Button itsStartButton;
  Button itsStopButton;
  OscillSource itsSource;
  int values[];
  int noPoints = 200;
  int itsFreq;
  int itsTimeout;
  Thread itsTimingThread;
  Dimension preferredSize = new Dimension(350, 300);
  Dimension minimumSize = new Dimension(350, 300);

  class MainPanel extends Panel{
    int window[];
    int backStore[];
    int windowSize;
    int readPointer;

    public MainPanel(int windowSize) {
      super();
      this.windowSize = windowSize;
      window = new int[windowSize+1];
      backStore = new int[windowSize+1];
      setBackground(Color.black);
      setSize(windowSize, windowSize);
      validate();
      readPointer = 0;
    }

    public void sync(OscillSource theSource) {
      readPointer = (readPointer+1) % getSize().width;
      int temp = theSource.getValue();
      backStore[readPointer] = window[readPointer];
      window[readPointer] = temp;

      paint(readPointer, getGraphics());
    }

    void paint(int where, Graphics g) {
      Dimension d = getSize();
      g.setColor(Color.green);
      g.setXORMode(Color.black);
      int lenght = d.height/2;
      g.drawLine(where, lenght-(backStore[where]), where, lenght-backStore[where]);
      //draw new one
      g.drawLine(where, lenght-(window[where]), where, lenght-(window[where]));
    }

    public void paint(Graphics g) {
      Dimension d = getSize();
      g.setColor(Color.green);
      g.setXORMode(Color.black);
      int lenght = d.height/2;
      for (int i=0; i<windowSize; i++) {
	paint(i, g);
      }
      
      if (itsTimingThread == null || !itsTimingThread.isAlive()) {
	g.setColor(Color.green);
	g.drawLine(0, d.height/2, windowSize, d.height/2);
      }
    }
  }

  /**
   * create an oscilloscope without source (yet)
   */
  public Oscilloscope() {
    super("oscilloscope");
    setSize(preferredSize);
    setLayout(null);
    values = new int[noPoints];
    itsMainPanel = new MainPanel(noPoints);
    itsMainPanel.setLocation(120, 50);
    itsStartButton = new Button("test");
    itsStartButton.setBounds(10, 30, 70, 25);
    itsStartButton.addActionListener(new ActionListener()
				     {
				       public  void actionPerformed(ActionEvent e)
					 { testMode();}});
    add(itsStartButton);
    itsStopButton = new Button("stop");
    itsStopButton.setBounds(10, 60, 70, 25);
    itsStopButton.addActionListener(new ActionListener()
				     {
				       public  void actionPerformed(ActionEvent e)
					 { 
					   stop();}});
    add(itsStopButton);
    add(itsMainPanel);
    validate();
    setVisible(true);
  }

  public Oscilloscope(OscillSource theSource, int freq) {
    super("oscilloscope");
    itsSource = theSource;
    itsFreq = freq;
    itsTimeout = 1000/freq;
    setSize(preferredSize);
    setLayout(null);
    values = new int[noPoints];
    itsMainPanel = new MainPanel(noPoints);
    itsMainPanel.setLocation(120, 50);
    itsStartButton = new Button("start");
    itsStartButton.setBounds(10, 30, 70, 25);
    itsStartButton.addActionListener(new ActionListener()
				     {
				       public  void actionPerformed(ActionEvent e)
					 { start();}});
    add(itsStartButton);
    itsStopButton = new Button("stop");
    itsStopButton.setBounds(10, 60, 70, 25);
    itsStopButton.addActionListener(new ActionListener()
				     {
				       public  void actionPerformed(ActionEvent e)
					 { 
					   stop();
					 }});
    add(itsStopButton);
    add(itsMainPanel);
    validate();
    setVisible(true);
  }

  public void stop () {
    if (itsTimingThread != null) 
      itsTimingThread.stop();
    
    setVisible(false);
    dispose();    
  }

  public void start() {
    if (itsSource == null) System.err.println("Error: no source for oscilloscope");
    if (itsFreq==0) {
      System.err.println("Warning: null frequency for oscilloscope (set to 1 Hz)");
      itsFreq = 1;
    }
    itsTimeout = 1000/itsFreq;
    if (itsTimingThread == null) { //create a new one
      itsTimingThread = 
	(new Thread() {
	  public void run() {
	    while (true) {
	      try {
		sleep(itsTimeout);
	      }
	      catch (Exception e) {}
	      itsMainPanel.sync(itsSource);
	    }
	  }
	});
	  } else itsTimingThread.stop();
    itsTimingThread.start();
  }

  int value_index = 0;
  public int getValue() {
    return values[(value_index++)%noPoints];
  }

  public void setFrequency(int freq) {
    if (freq != 0) {
      itsFreq = freq;
      itsTimeout = 1000/freq;
    }
    else System.err.println("Error: null frequency for Oscilloscope");
  }

  public void setSource(OscillSource theSource) {
    itsSource = theSource;
  }

  public void testMode() {
    // in this mode, the oscilloscope itself is providing the data to be shown
    //(a cosinus wave)
    setSource(this);
    setFrequency(300);
    for (int i=0; i<noPoints; i++) {
      values[i] = (int) (100*(Math.sin(i*2*Math.PI/noPoints)));
    }
    start();
  }

  public Dimension getPreferredSize() {
    return preferredSize;
  }

  public Dimension getMinimumSize() {
    return minimumSize;
  }
}




