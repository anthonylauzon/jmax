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
package ircam.jmax.dialogs;

import java.awt.*;
import java.awt.event.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;

/**
 * A class giving a set of standard questions dialogs.
 * Use it thru the static function; they provide 
 * the complete handling.
 */


public class YesOrNo extends Dialog implements ActionListener, KeyListener
{
  Button yesButton;
  Button noButton;
  boolean answer;

  public static boolean ask(Frame frame, String question, String yes, String no)
  {
    boolean answer;
    YesOrNo dialog;

    dialog = new YesOrNo(frame, question, yes, no);
    dialog.setLocation(300, 300);
    dialog.show();

    answer = dialog.answer;

    dialog.dispose();

    return answer;
  }

  public YesOrNo(Frame theFrame, String question, String yes, String no)
  {
    super(theFrame, question, true);

    setLayout(new BorderLayout());
    
    //Create middle section.

    Panel p1 = new Panel();
    Label label = new Label(question);
    p1.add(label);
    add("Center", p1);
    
    //Create bottom row.
    Panel p2 = new Panel();
    // p2.setLayout(new BorderLayout());

    p2.setLayout(new GridLayout(1,3));
    yesButton = new Button(yes);
    yesButton.setBackground(Color.white);
    yesButton.addActionListener(this);
    //p2.add("East", yesButton);
    p2.add(yesButton);

    noButton = new Button(no);
    noButton.setBackground(Color.white);
    noButton.addActionListener(this);
    //p2.add("Center", noButton);
    p2.add(noButton);

    add("South", p2);
    
    addKeyListener(this);

    pack();
  }

  public void actionPerformed(ActionEvent e)
  {    
    if (e.getSource() == yesButton)
      {
	answer = true;
	setVisible(false);
      }
    else if(e.getSource() == noButton)
      {
	answer = false;
	setVisible(false);
      }
  }

  public void keyTyped(KeyEvent e)
  {
    if (e.getKeyChar() == '\r')
      {
	answer = true;
	setVisible(false);
      }
  }

  public void keyReleased(KeyEvent e)
  {
  }
  
  public void keyPressed(KeyEvent e)
  {
  }
}








