package ircam.jmax.dialogs;

import java.awt.*;
import java.awt.event.*;
import java.io.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.project.*; // delete me !!!

/**
 * The "system statistics" dialog.
 */

public class StatisticsDialog extends Dialog implements ActionListener, KeyListener{
  Frame parent;
  Button okButton;
  
  public StatisticsDialog(Frame dw) {
    super(dw, "System stats", false);
    parent = (Frame)dw;
    
    // Do a gc before giving statistics (added by mdc).
    
    System.gc();
    
    //Create middle section.
    Panel p1 = new Panel();
    p1.setLayout(new GridLayout(0, 1));

    Label label1 = new Label("jMax "+  MaxApplication.getProperty("jmaxVersion"));

    p1.add(label1);
    Label label2 = new Label("running on " + (String)(System.getProperties().getProperty("os.name")));
    p1.add(label2);
    Label label3 = new Label("host architecture:"+(String)(System.getProperties().getProperty("os.arch")));
    p1.add(label3);
    Label label4 = new Label("Java version" + (String)(System.getProperties().getProperty("java.version")));
    p1.add(label4);
    Label label5 = new Label("FTS SERVER");
    p1.add(label5);
    Label label6;
    if (FtsServer.getServer() !=null)
      label6 = new Label("Fts connection:" + FtsServer.getServer().toString());
    else label6 = new Label("Fts connection: not connected" );
    p1.add(label6);
    Label label7 = new Label("TCL version: 8.00");
    p1.add(label7);
    Label label8 = new Label("Total memory " + Runtime.getRuntime().totalMemory());
    p1.add(label8);
    Label label9 = new Label("Used memory " + (Runtime.getRuntime().totalMemory() - Runtime.getRuntime().freeMemory()));
    p1.add(label9);

    add("Center", p1);
    
    //Create bottom row.
    Panel p2 = new Panel();
    p2.setLayout(new FlowLayout(FlowLayout.RIGHT));
    Button okButton = new Button("OK");
    okButton.addActionListener(this);
    p2.add(okButton);
    add("South", p2);

    addKeyListener(this);

    //Initialize this dialog to its preferred size.
    pack();  

    setLocation(100, 100);
    setVisible(true);
  }

  public void actionPerformed(ActionEvent e){    
    setVisible(false);
    dispose();      
  }
  
  //////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////keyListener--inizio
  public void keyTyped(KeyEvent e){}
  public void keyReleased(KeyEvent e){}
  
  public void keyPressed(KeyEvent e){
    if(e.getKeyCode() == ircam.jmax.utils.Platform.RETURN_KEY){
      setVisible(false);
      dispose();     
    }
  }
  /////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////keyListener--fine
}










