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

package ircam.jmax.utils;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

/**
 * A 'software' oscilloscope.
 * This version works in a thread, in a "polling" mode.
 * Usage: create (also statically via the "makeOscilloscope" call)
 * the oscilloscope with two arguments:
 * - The source of data (an object that implements the OscillSource interface)
 * - The refresh rate (in Hz)
 *  In the Oscilloscope window, press the start button to see 
 * your data changing.
 * You can launch an oscilloscope from the TCL console, via
 * "oscilloscope" or
 * "oscilloscope <oscillsource> <freq>" 
 * The first command just launches a test-only, 300 Hz oscilloscope
 * LIMITATIONS:
 * The present version does not allow you to change:
 * - the speed of the "pencil" (every tick is an horizontal pixel)
 * - the zero position and the normalization of the values
 * - the size of the video screen ...
 * Important WARNING:
 * A refresh rate too high can interfere with other threads in the program,
 * and can cause a crash. A ErmesSlider for example can be tracked up to 200Hz.
 */

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
  static Oscilloscope globalOscilloscope;

  /**
   * create a "global" oscilloscope that can be accessed statically via
   * the "globalOscilloscope" field.
   * 
   */
  public static void makeOscilloscope(OscillSource theSource, int freq) {
    if (globalOscilloscope == null)
      globalOscilloscope = new Oscilloscope(theSource, freq);
    else {
      globalOscilloscope.setSource(theSource);
      globalOscilloscope.setFrequency(freq);
    }
  }

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
   * create an oscilloscope without source (yet).
   * It will just have a "test" and a "stop" button.
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

  /**
   * Create an oscilloscope bind to the given source that draw points
   * at a 'freq' rate (in Hz).
   */
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

  /**
   * stop the oscilloscope and delete the window.
   */
  public void stop () {
    if (itsTimingThread != null) 
      itsTimingThread.stop();
    
    setVisible(false);
    dispose();    
  }

  /**
   * start the oscilloscope.
   */
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

  /**
   * Change the Oscilloscope refresh rate.
   */
  public void setFrequency(int freq) {
    if (freq != 0) {
      itsFreq = freq;
      itsTimeout = 1000/freq;
    }
    else System.err.println("Error: null frequency for Oscilloscope");
  }

  /**
   * Set the source of data for this oscilloscope.
   */
  public void setSource(OscillSource theSource) {
    itsSource = theSource;
  }

  /**
   * Fills a sinusoidal table and start the oscilloscope giving it
   * the values in the table (it's a test mode)
   */
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




