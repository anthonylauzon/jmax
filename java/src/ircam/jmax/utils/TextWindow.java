//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.utils;
import java.awt.*;
import java.awt.event.*;
import java.io.*;

/**
 * The base class for all the Text window - related components.
 * Ex: TextEditor, Ermes Console.
 */
public class TextWindow extends Frame implements KeyListener, WindowListener{
  int NUM_ROWS = 50;
  int NUM_COLS = 70;
  public TextArea itsTextArea;
  public PrintStream itsPrintStream;
  OutputStream itsOutputStream;
  String filler;
  String buffer = new String();

  public TextWindow() {
    addKeyListener(this);
    addWindowListener(this);
  }
  
  public TextWindow(String title) {
    super(title);
    setLayout(new BorderLayout());
    setBackground(Color.white);
    setForeground(Color.blue);
    Font thisFont = new Font(Platform.FONT_NAME, Font.PLAIN, Platform.FONT_SIZE);
    setFont(thisFont);
    filler = new String();
    for (int i = 0; i< NUM_COLS; i++) {
      filler += "_";
    }
    itsOutputStream = new TWOutputStream(this);
    itsPrintStream = new PrintStream(itsOutputStream);
    buffer = "";
    Init();
  }

  ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////keyListener --inizio
  public void keyTyped(KeyEvent e){}
  public void keyReleased(KeyEvent e){}
  
  public void keyPressed(KeyEvent e){
    if(e.getKeyCode() == Platform.DELETE_KEY || e.getKeyCode() == Platform.BACKSPACE_KEY) {
      if (buffer.length() != 0) buffer = buffer.substring(0,buffer.length()-1);
    }
    else if (e.getKeyCode() == Platform.NEWLINECHAR) {
      if (buffer != "") {
	buffer = "";
      }
    }
    else buffer += e.getKeyChar();
  }
  ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////keyListener --fine

  ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////WindowListener --inizio
  public void windowClosing(WindowEvent e){
    setVisible(false);
    dispose();
  }
  public void windowOpened(WindowEvent e){}
  public void windowClosed(WindowEvent e){}
  public void windowIconified(WindowEvent e){}       
  public void windowDeiconified(WindowEvent e){}
  public void windowActivated(WindowEvent e){}
  public void windowDeactivated(WindowEvent e){}  

  ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////WindowListener --fine
  public PrintStream getPrintStream() {
    return itsPrintStream;
  }
  
  public void Init() {
    Rectangle r = getBounds();
    
    itsTextArea = new TextArea(NUM_ROWS, NUM_COLS);
    itsTextArea.setEditable(true);
    itsTextArea.setBackground(Color.white);
    add("Center", itsTextArea);
    validate();
    itsTextArea.setVisible(true);
    itsTextArea.requestFocus();
    addKeyListener(this);
    addWindowListener(this);
  }
  
  public Dimension getPreferredSize() {
    Dimension d = new Dimension();
    d.width = itsTextArea.getFontMetrics(itsTextArea.getFont()).stringWidth(filler);
    d.height = itsTextArea.getFontMetrics(itsTextArea.getFont()).getHeight() * NUM_ROWS;
    return d;
  }
  
  public Dimension getMinimumSize() {
    return getPreferredSize();
  }
}









