package ircam.jmax.utils;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

import ircam.jmax.*;

//
// The graphic pop-up menu used to change the number of an inlet or an outlet in a subpatcher.
//

public class ChooseNumberPopUp extends JPopupMenu
{
  private static ChooseNumberPopUp  popup = new ChooseNumberPopUp();

  private NumberChoosenListener listener = null;
  private int max = 0;
  private int min = 0;

  class ChooseNumberPopUpActionListener implements ActionListener
  {
    int idx;

    ChooseNumberPopUpActionListener(int idx)
    {
      this.idx = idx;
    }

    public void actionPerformed( ActionEvent e)
    { 
      listener.numberChoosen( idx);
    }
  };
    
  private void redefine(int min, int max)
  {
    JMenuItem aMenuItem;

    if ((min == this.min) && (max == this.max))
      return;
    
    // Remove all the components

    for (int i = 0; i < (this.max - this.min); i++)
      remove(getComponentAtIndex(0));

    // Rebuild the menu

    this.min = min;
    this.max = max;

    for (int i = min; i < max; i++)
      {
	aMenuItem = new JMenuItem( Integer.toString( i));

	add(aMenuItem);
	aMenuItem.addActionListener(new ChooseNumberPopUpActionListener(i));
      }
  }

  //
  // Constructor
  //

  public ChooseNumberPopUp() 
  {
    super();
  }

  public static void choose(Component invoker, NumberChoosenListener listener, int min, int max, Point p)
  {
    popup.listener = listener;
    popup.redefine(min, max);
    popup.show(invoker, p.x - 2, p.y - 2);
  }
}



