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
  private DefaultListModel tools = new DefaultListModel();

  private static Frame topFrame = null;

  static public MaxWindowManager getWindowManager()
  {
    return theWindowManager;
  }

  MaxWindowManager()
  {
  }


  /** Call this method to add a window to 
   * from  the window manager window list
   */

  public void addWindow(Frame window)
  {
    windowOperationCount++;
    windows.addElement(window);
    window.addWindowListener(this);
  }

  /** Call this method to remove a window to
   * from  the window manager window list
   */

  public void removeWindow(Frame window)
  {
    windowOperationCount++;
    windows.removeElement(window);
    window.removeWindowListener(this);
  }

  /** call this method to tell the window 
   * manager that something about the window
   * changed (i.e. the title, for now).
   * Actually, this will be obsolete the day
   * we use a swing list as a menu
   */

  public void windowChanged(Frame window)
  {
    // Provoke a null change in the vector,
    // to get an event for the listeners

    windowOperationCount++;

    if (windows.size() > 0)
      windows.setElementAt(window, windows.indexOf(window));
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

  public ListModel getWindowList()
  {
    return windows;
  }

  public ListModel getToolFinderList()
  {
    return tools;
  }

  public int getWindowOperationCount()
  {
    return windowOperationCount;
  }

  /** 
   * Return a Frame considered top level, for dialogs that have no 
   * "natural" parent frame; it is usually set to the console,
   * for the current console centered interaction.
   */

  public static void setTopFrame(Frame frame)
  {
    topFrame = frame;
  }

  public static Frame getTopFrame()
  {
    return topFrame;
  }

  /** Produce a unique window title for the Name 
   *  the technique is the following (naive, but usefull)
   *  it look all the window titles, and count those
   *  which start exactly with theName; finally,
   *  add this count to theName string and return it 
   */

  public String makeUniqueWindowTitle(String theName)
  {
    int count = 0;

    for (int i = 0; i < windows.size(); i++)
      {
	Frame w = (Frame) windows.elementAt(i);

	if (w.getTitle().startsWith(theName))
	  count++;
      }

    if (count > 0)
      return theName +" (" + count + ")";
    else
      return theName;
  }
  

  /** Call this method to add a ToolFinder to 
   * the window manager tool finder list.
   * A tool finder can't be removed; tools window
   * are not never moved by the window manager.
   */

  public void addToolFinder(MaxToolFinder finder)
  {
    windowOperationCount++;
    tools.addElement(finder);
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

