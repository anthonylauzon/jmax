package ircam.jmax.toolkit;

import java.awt.*;
import java.awt.event.*;

import com.sun.java.swing.*;
import com.sun.java.swing.event.*;

import ircam.jmax.*;

public class MaxWindowMenu extends Menu implements WindowListener, ListDataListener 
{
  Frame frame;
  int windowOperationCount = -1;
  boolean windowIsActive = false;
  ListModel windowList;
  ListModel toolFinders;

  /** Build a window menu for frame */

  public MaxWindowMenu(String title, Frame frame)
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


  public void dispose()
  {
    windowList.removeListDataListener(this);
    frame.removeWindowListener(this);
  }

  private void rebuildWindowMenuIfNeeded()
  {
    if (windowOperationCount < MaxWindowManager.getWindowManager().getWindowOperationCount())
      rebuildWindowMenu();
  }

  private void rebuildWindowMenu()
  {
    MenuItem mi;

    windowOperationCount = MaxWindowManager.getWindowManager().getWindowOperationCount();
    removeAll();

    mi = new MenuItem("Stack");
    add(mi);
    mi.addActionListener(new ActionListener()
				{public  void actionPerformed(ActionEvent e)
				    { MaxWindowManager.getWindowManager().StackWindows();}});

    mi = new MenuItem("Tile");
    add(mi);
    mi.addActionListener(new ActionListener()
				{public  void actionPerformed(ActionEvent e)
				    { MaxWindowManager.getWindowManager().TileWindows();}});

    mi = new MenuItem("Tile Vertical");
    add(mi);
    mi.addActionListener(new ActionListener()
				{public  void actionPerformed(ActionEvent e)
				    { MaxWindowManager.getWindowManager().TileVerticalWindows();}});

    addSeparator();

    // Built the tool menu

    for (int i = 0; i < toolFinders.getSize(); i++)
      {
	final MaxToolFinder toolFinder = (MaxToolFinder) toolFinders.getElementAt(i);

	mi = new MenuItem(toolFinder.getToolName());
	add(mi);
	mi.addActionListener(new ActionListener()
			     { public  void actionPerformed(ActionEvent e)
			       { toolFinder.open();}});
      }
    
    addSeparator();

    for (int i = 0; i < windowList.getSize(); i++)
      {
	final Frame w = (Frame) windowList.getElementAt(i);

	mi = new MenuItem(w.getTitle());
	add(mi);
	mi.addActionListener(new ActionListener()
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




                  

