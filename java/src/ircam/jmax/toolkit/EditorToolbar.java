package ircam.jmax.toolkit;

import javax.swing.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;

/**
 * A specific component used as a container of JToggleButtons 
 * corresponding to Tools. It can have a set of listeners
 * to be called when the user selects a new tool.
 * EditorToolbar class keeps a JPopupMenu with all the tools inserted;
 * this Popup can be accessed and used as an alternative for choosing 
 * a tool (example, right-mouse click). A toolbar can be public (the same
 * object is shared between every editor's window, or private (every window
 * has its own local toolbar). The current implementation only allows private
 * toolbars
 */
public class EditorToolbar extends JToolBar implements ActionListener, WindowListener{
  
  /**
   * constructor. It inserts the tools provided by the given
   * ToolbarProvider. The direction parameter specify if the
   * toolbar should be HORIZONTAL or VERTICAL. 
   * For now, the toolbar is private to a window (and the tools are static!)
   */
  public EditorToolbar(ToolbarProvider theProvider, int direction) 
  {
    super();
    setDoubleBuffered(false);
    
    if (direction == VERTICAL)
      setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));

    /** prepare the popup */
    itsPopupMenu = new JPopupMenu();
    JMenuItem item;

    //install the Tools
    Tool aTool;
    for (Enumeration e = theProvider.getTools(); e.hasMoreElements();) 
      {
	aTool = (Tool) e.nextElement();
	if (aTool == null) System.err.println("warning: trying to add a null tool in the toolbar");
	else {
	  addTool(aTool);
	}
      }

    setTool(theProvider.getDefaultTool());

    itsClientGc = theProvider.getGraphicContext();
    itsClientFrame = itsClientGc.getFrame();
    itsClientFrame.addWindowListener(this);  
    
    currentTool.reActivate(itsClientGc);
  }

  public void setTool(Tool t)
  {
    currentTool = t;
    toolNotification(t);
  }
  
  public Tool getTool()
  {
    return currentTool;
  }

  /**
   * Inscribe a new listener to this toolbar
   */
  public void addToolListener(ToolListener tl) 
  {  
    listeners.addElement(tl);
  }

  /**
   * remove a tool change listener
   */
  public void removeToolListener(ToolListener tl) 
  {  
    listeners.removeElement(tl);  
  }
  


  /**
   * utility function. Communicate the tool-change event to the listeners
   */
  private void toolNotification(Tool theTool) 
  {  
    ToolChangeEvent aEvent = new ToolChangeEvent(theTool);
    
    ToolListener aListener;
    for (Enumeration e=listeners.elements(); e.hasMoreElements();) 
      {
	aListener = (ToolListener) e.nextElement();
	aListener.toolChanged(aEvent);
      }
  }
 

  /**
   * add a new tool in the toolbar (and in the popupMenu)
   */ 
  public void addTool(Tool theTool) 
  {
    JToggleButton aButton = new JToggleButton(theTool.getIcon());
    JMenuItem aMenuItem;

    aButton.setToolTipText(theTool.getName());
    aButton.setMargin(new Insets(0,0,0,0));

    add(aButton);
    itsButtonGroup.add(aButton);

    itsPopupMenu.add (aMenuItem = new JMenuItem (theTool.getIcon()));
    
    validate();
    
    itsTools.put(aButton, theTool);
    itsTools.put(aMenuItem, theTool);

    aButton.addActionListener(this);
    aMenuItem.addActionListener(this);
  }  


  /**
   * From the ActionListener interface,
   * called when a user selects a new tool.
   * The new tool will work on the same graphic context then the old.
   */
  public void actionPerformed(ActionEvent e) 
  {    
    Object aSource =  e.getSource();
    Tool aTool = (Tool) itsTools.get(aSource);
    
    currentTool.deactivate();
    aTool.reActivate(itsClientGc);

    setTool(aTool);
    
  }


  // WindowListener interface

  public void windowClosing(WindowEvent e)
  {
  }

  

  public void windowOpened(WindowEvent e)
  {
  }

  public void windowClosed(WindowEvent e)
  {
  }

  public void windowIconified(WindowEvent e)
  {
  }       

  public void windowDeiconified(WindowEvent e)
  {
  }

  public void windowActivated(WindowEvent e)
  {

    currentTool.reActivate(itsClientGc);

  }

  public void windowDeactivated(WindowEvent e)
  {
  }

  //---- Fields and accessors

  public JPopupMenu itsPopupMenu;

  Hashtable itsTools = new Hashtable();
  Tool currentTool = null;

  JFrame itsFrame;
  Frame itsClientFrame;
  GraphicContext itsClientGc;
  Vector listeners = new Vector();

  private ButtonGroup itsButtonGroup = new ButtonGroup();
  public static final int VERTICAL = 0;
  public static final int HORIZONTAL = 1;


}











