package ircam.jmax;


import java.awt.*;
import java.awt.event.*;
import com.sun.java.swing.*;


/** This class provide a basic service for registering windows.
  
  It export a ListModel with all the Windows on it (may be later
  should become a tree Model ?).

  Only a registration function is provided, the unregistration
  is made by means of a WindowListener.

  The Windows can be any Frame.

  It provide three window managing/layout functions (tile/tile
  vertical/stack/??), derived from the original Enzo Maggi code.


  This class is used by the MaxWindowMenu in ircam.jmax.toolkit.
  */


public class MaxWindowManager implements WindowListener
{
  int windowOperationCount = 0;

  final static int SCREENVERT = java.awt.Toolkit.getDefaultToolkit().getScreenSize().height;
  final static int SCREENHOR  = java.awt.Toolkit.getDefaultToolkit().getScreenSize().width;

  private static MaxWindowManager theWindowManager = new MaxWindowManager();

  private DefaultListModel windows = new DefaultListModel();

  static public MaxWindowManager getWindowManager()
  {
    return theWindowManager;
  }

  MaxWindowManager()
  {
  }

  public void addWindow(Frame window)
  {
    windowOperationCount++;
    windows.addElement(window);
    window.addWindowListener(this);
  }

  public void removeWindow(Frame window)
  {
    windowOperationCount++;
    windows.removeElement(window);
    window.removeWindowListener(this);
  }

  void suspendWindow(Frame window)
  {
    windowOperationCount++;
    windows.removeElement(window);
  }

  void readdWindow(Frame window)
  {
    windowOperationCount++;
    windows.addElement(window);
  }

  void retitleWindow(Frame window)
  {
    windowOperationCount++;
  }

  public ListModel getWindowList()
  {
    return windows;
  }

  public int getWindowOperationCount()
  {
    return windowOperationCount;
  }

  /** This method return a frame good for running a 
    dialog that doesn't have a parent window;
    of course, at least one window must exists.
    */
  
  public Frame getAFrame()
  {
    if (windows.size() > 0)
      return (Frame) windows.elementAt(0);
    else
      return null;
  }

  // Event handler for WindowListener

  public void windowClosing(WindowEvent e)
  {
  }

  public void windowOpened(WindowEvent e)
  {
  }
  
  public void windowClosed(WindowEvent e)
  {
    removeWindow((Frame) e.getWindow());
  }

  public void windowIconified(WindowEvent e)
  {
    suspendWindow((Frame) e.getWindow());
  }

  public void windowDeiconified(WindowEvent e)
  {
    readdWindow((Frame) e.getWindow());
  }

  public void windowActivated(WindowEvent e)
  {
  }

  public void windowDeactivated(WindowEvent e)
  {
  }

  /** Tile vertically the WIndows */

  public void TileVerticalWindows()
  {
    Frame window;
    int x = 7;
    int y = 20;
    int width = (int)java.lang.Math.floor(SCREENHOR / windows.size()) - 10;
    int height = SCREENVERT - 35;

    for (int i = 0; i < windows.size(); i++)
      {
	window = (Frame) windows.elementAt(i);

	if (i > 0)
	  x = x + width +7;

	window.setBounds(x, y, width, height);
      }
  }


  public void TileWindows()
  {
    int width;
    int height;
    int initX, x;
    int initY, y;

    Frame  window;
    boolean changHor = false;
    int z, j;

    int nWindows = windows.size();

    if (nWindows!=0)
      {
	z=1;

	if (nWindows==1)
	  j=1;
	else
	  j=2;

	while(z*j < nWindows)
	  {
	    z++;

	    if(z*j < nWindows)
	      j++;
	  }

	height = (int)java.lang.Math.floor(SCREENVERT/j) - 35;
	width = (int)java.lang.Math.floor(SCREENHOR/z) - 10;
	initX = 7;
	initY = 20;
	x = initX;
	y = initY;

	for (int k=0; k < nWindows; k++)
	  {
	    int res;

	    window = (Frame) windows.elementAt(k);

	    res = (int)java.lang.Math.floor(k/j);

	    if ((res * j) == k)
	      changHor=false;

	    if ((res != 0) && (! changHor))
	      {
		x = initX + (width+7)*res;
		y = initY;
		changHor=true;
	      }
	    else if(k>0)
	      y += height + 25;
	
	    window.setBounds(x, y, width, height);
	  }
      } 
  }


  public void StackWindows()
  {
    Frame window;
    int x, y;
    int height, width;

    if (windows.size() == 0)
      return;

    x = 50;
    y = 50;
    // height = (int)java.lang.Math.floor(SCREENVERT/3);
    // width = (int)java.lang.Math.floor(SCREENHOR/3);

    for (int i = 0; i < windows.size(); i++)
      {
	window = (Frame) windows.elementAt(i);

	// window.setBounds(x, y, width, height);
	window.setLocation(x, y);
	window.toFront();

	x += 20;
	y += 20;
      }
  }
}

