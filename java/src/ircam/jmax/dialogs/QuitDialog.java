//
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 
package ircam.jmax.dialogs;

import java.awt.*;
import java.awt.event.*;
import ircam.jmax.*;

/**
 * A generic "file not saved" dialog
 */

public class QuitDialog extends Dialog
{
  static final public int JUST_QUIT = 0;
  static final public int REVIEW_AND_QUIT = 1;
  static final public int CANCEL = 2;

  int answer = CANCEL;

  public QuitDialog(Frame dw)
  {
    super(dw, "Quit", true);

    Button button;

    setLayout(new BorderLayout());
    
    //Create middle section.
    Panel p1 = new Panel();
    Label label = new Label("There are unsaved documents; you really want to Quit ?");
    p1.add(label);
    add("Center", p1);
    
    //Create bottom row.
    Panel p2 = new Panel();
    p2.setLayout(new GridLayout(1,3));

    button = new Button("Quit");
    button.setBackground(Color.white);
    button.addActionListener(new ActionListener() {
      public  void actionPerformed(ActionEvent e)
	{answer = JUST_QUIT; setVisible(false);	}});
    p2.add(button);

    button = new Button("Review Unsaved And Quit");
    button.setBackground(Color.white);
    button.addActionListener(new ActionListener() {
      public  void actionPerformed(ActionEvent e)
	{answer = REVIEW_AND_QUIT; setVisible(false);	}});
    p2.add(button);

    button = new Button("Cancel");
    button.setBackground(Color.white);
    button.addActionListener(new ActionListener() {
      public  void actionPerformed(ActionEvent e)
	{answer = CANCEL; setVisible(false);	}});
    p2.add(button);
    
    add("South", p2);

    class QuitKeyListener extends KeyAdapter
    {
      public void keyPressed(KeyEvent e)
      {
	if (e.getKeyCode() == KeyEvent.VK_ENTER)
	  {
	    answer = CANCEL; 
	    setVisible(false);
	  }
      }  
    }

    addKeyListener(new QuitKeyListener());

    pack();
    setLocation(300, 300);
    setVisible(true);
  }

  public int getAnswer()
  {
    return answer;
  }
}




