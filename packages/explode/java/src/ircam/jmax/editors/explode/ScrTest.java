package ircam.jmax.editors.explode;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import com.sun.java.swing.*;

/**
 * A TEST class to show a musical editor (extended piano roll)
 */
public class ScrTest extends JFrame implements AAAReadme{
  
  /**
   * constructor. It creates the score panel in a Frame.
   */
  public ScrTest(String title, EventProvider ep) {
    super(title);

    getContentPane().add(new ScrPanel(ep));

    addWindowListener((new WindowAdapter() {

      public void windowClosing(WindowEvent e) {
	System.exit(0);
      }
    }));
    

    //----

    validate();
    pack();
    setVisible(true);
  }

  /**
   * Use this static function to create an instance of the editor
   */
  static public void createScr(String title, EventProvider ep) {
    new ScrTest(title, ep);
  }

  static public void main (String argv[]) {
    createScr("explode", new Explode());
  }
}





