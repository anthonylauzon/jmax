package ircam.jmax.editors.patcher;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.objects.*;

//
// The graphic pop-up menu used to change the number of an inlet or an outlet in a subpatcher.
//

public class ErmesObjInOutPop extends JPopupMenu
{
  private ErmesObjInOut itsOwner;
  private ErmesSketchWindow window;  
  private int max = 0;

  class ErmesInOutPopActionListener implements ActionListener
  {
    int idx;

    ErmesInOutPopActionListener(int idx)
    {
      this.idx = idx;
    }

    public void actionPerformed( ActionEvent e)
    { 
      itsOwner.changeNo( idx);
    }
  };
    
  public void setOwner( ErmesObjInOut object) 
  {
    itsOwner = object;
  }
		
  public void Redefine( int numbers)
  {
    JMenuItem aMenuItem;

    if (numbers == max)
      return;

    // Remove all the components

    for (int i = 0; i < max; i++)
      remove(getComponentAtIndex(0));

    // Rebuild the menu

    max = numbers;

    for (int i = 0; i < max; i++)
      {
	aMenuItem = new JMenuItem( Integer.toString( i));

	add(aMenuItem);
	aMenuItem.addActionListener(new ErmesInOutPopActionListener(i));
      }
  }

  //
  // Constructor accepting the number of in/out to show in the popup
  //


  public ErmesObjInOutPop(ErmesSketchWindow window, int numbers) 
  {
    super( "Change:");

    this.window = window;
    itsOwner = null;

    Redefine(numbers);
  }
}



