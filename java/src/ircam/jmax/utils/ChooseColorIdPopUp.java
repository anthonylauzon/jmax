package ircam.jmax.utils;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

import ircam.jmax.*;

//
// The graphic pop-up menu used to change the number of an inlet or an outlet in a subpatcher.
//

public class ChooseColorIdPopUp extends JPopupMenu
{
  private static ChooseColorIdPopUp  popup = null;
  private NumberChoosenListener listener = null;

  // Do not change this array; the indexes in this array are stored
  // in patches; change in this array will produce a change
  // in stored patches.

  static Color colors[] = 
  {
    Color.yellow,
    Color.blue,
    Color.cyan,
    Color.green,
    Color.magenta,
    Color.orange,
    Color.pink,
    Color.red,
    Color.white,
    Color.black
  };

  class ChooseColorIdUpActionListener implements ActionListener
  {
    int idx;

    ChooseColorIdUpActionListener(int idx)
    {
      this.idx = idx;
    }

    public void actionPerformed( ActionEvent e)
    {
      listener.numberChoosen( idx);
    }
  };
    
  public ChooseColorIdPopUp() 
  {
    super();

    // build the menu

    for (int i = 0; i < colors.length; i++)
      {
	JMenuItem aMenuItem = new JMenuItem( "   ");
	aMenuItem.setBackground(colors[i]);
	aMenuItem.setForeground(colors[i]);
	aMenuItem.setFocusPainted(false);
	add(aMenuItem);
 	aMenuItem.addActionListener(new ChooseColorIdUpActionListener(i));
      }
  }

  public static void choose(Component invoker, NumberChoosenListener listener, Point p)
  {
    if (popup == null)
      popup = new ChooseColorIdPopUp();

    popup.listener = listener;
    popup.show(invoker, p.x - 2, p.y - 2);
  }
}



