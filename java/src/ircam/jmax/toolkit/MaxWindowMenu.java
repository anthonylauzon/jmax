package ircam.jmax.toolkit;

import java.awt.*;
import java.awt.event.*;

import com.sun.java.swing.*;
import com.sun.java.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;

public class MaxWindowMenu extends Menu implements WindowListener, ListDataListener 
{
  MaxEditor frame;
  int windowOperationCount = -1;
  boolean windowIsActive = false;
  ListModel windowList;
  ListModel toolFinders;
  private MaxVector toDispose = new MaxVector();

  abstract class WindowActionListener implements ActionListener
  {
    MenuItem item;

    WindowActionListener(MenuItem item)
    {
      this.item = item;
      toDispose.addElement(this);
    }

    public  abstract void actionPerformed(ActionEvent e);

    public void dispose()
    {
      item.removeActionListener(this);
    }
  }

  /** Build a window menu for frame */

  public MaxWindowMenu(String title, MaxEditor frame)
  {
    super(title);
    
    this.frame = frame;
    windowList = MaxWindowManager.getWindowManager().getWindowList();
    toolFinders = MaxWindowManager.getWindowManager().getToolFinderList();

    windowList.addListDataListener(this);
    toolFinders.addListDataListener(this);
    frame.addWindowListener(this);

    rebuildWindowMenu();
  }

  private void disposeActionListeners()
  {
    for (int i = 0; i < toDispose.size(); i++)
      ((WindowActionListener) toDispose.elementAt(i)).dispose();

    toDispose.removeAllElements();
  }


  public void removeNotify()
  {
    disposeActionListeners();
    windowList.removeListDataListener(this);
    toolFinders.removeListDataListener(this);
    frame.removeWindowListener(this);
    frame = null;
    super.removeNotify();
  }

  private void rebuildWindowMenuIfNeeded()
  {
    if (windowOperationCount < MaxWindowManager.getWindowManager().getWindowOperationCount())
      rebuildWindowMenu();
  }

  private void rebuildWindowMenu()
  {
    // First, dispose all the action listener (Java VM GC bug !)

    disposeActionListeners();

    MenuItem mi;

    windowOperationCount = MaxWindowManager.getWindowManager().getWindowOperationCount();
    removeAll();

    mi = new MenuItem("Stack");
    add(mi);
    mi.addActionListener(new WindowActionListener(mi)
				{public  void actionPerformed(ActionEvent e)
				    { MaxWindowManager.getWindowManager().StackWindows();}});

    mi = new MenuItem("Tile");
    add(mi);
    mi.addActionListener(new WindowActionListener(mi)
				{public  void actionPerformed(ActionEvent e)
				    { MaxWindowManager.getWindowManager().TileWindows();}});

    mi = new MenuItem("Tile Vertical");
    add(mi);
    mi.addActionListener(new WindowActionListener(mi)
				{public  void actionPerformed(ActionEvent e)
				    { MaxWindowManager.getWindowManager().TileVerticalWindows();}});

    addSeparator();

    // Built the tool menu

    for (int i = 0; i < toolFinders.getSize(); i++)
      {
	final MaxToolFinder toolFinder = (MaxToolFinder) toolFinders.getElementAt(i);

	mi = new MenuItem(toolFinder.getToolName());
	add(mi);
	mi.addActionListener(new WindowActionListener(mi)
			     { public  void actionPerformed(ActionEvent e)
			       { toolFinder.open();}});
      }
    
    addSeparator();

    for (int i = 0; i < windowList.getSize(); i++)
      {
	final Frame w = (Frame) windowList.getElementAt(i);

	mi = new MenuItem(w.getTitle());
	add(mi);
	mi.addActionListener(new WindowActionListener(mi)
			     { public  void actionPerformed(ActionEvent e)
			       { w.toFront();}});
      }
  }

  // WindowListener interface

  public void windowClosing(WindowEvent e)
  {
  }

  public void windowOpened(WindowEvent e)
  {
    rebuildWindowMenuIfNeeded();
  }

  public void windowClosed(WindowEvent e)
  {
  }

  public void windowIconified(WindowEvent e)
  {
  }       

  public void windowDeiconified(WindowEvent e)
  {
    rebuildWindowMenuIfNeeded();
  }

  public void windowActivated(WindowEvent e)
  {
    windowIsActive = true;
    rebuildWindowMenuIfNeeded();
  }

  public void windowDeactivated(WindowEvent e)
  {
    windowIsActive = false;
  }

  // ListDataListener

  public void contentsChanged(ListDataEvent e)
  {
    if (windowIsActive)
      rebuildWindowMenu();
  }

  public void intervalAdded(ListDataEvent e)
  {
    if (windowIsActive)
      rebuildWindowMenuIfNeeded();
  }

  public void intervalRemoved(ListDataEvent e) 
  {
    if (windowIsActive)
      rebuildWindowMenuIfNeeded();
  }
}




                  

