package ircam.jmax.toolkit;

import com.sun.java.swing.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;

/**
 * a generic class implementing an editor's toolbar. It can have a set of listeners
 * to be called when the user selects a new tool.
 * EditorToolbar class keeps a JPopupMenu with all the tools inserted.
 * This Popup can be accessed and used as an alternative for choosing 
 * a tool (example, right-mouse click).
 */
public class EditorToolbar extends JToolBar implements ActionListener, WindowListener{
  
  /**
   * constructor. It inserts the tools provided by the given
   * ToolbarProvider
   */
  public EditorToolbar(ToolbarProvider theProvider, int direction) 
  {
    super();
    setDoubleBuffered(false);
    
    if (direction == VERTICAL)
      setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
    currentContext = theProvider.getGraphicContext();

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

    /*itsFrame = new JFrame("tools");    
      itsFrame.getContentPane().add(this);
      
      itsFrame.pack();
      itsFrame.setVisible(true);*/
    
    setTool(theProvider.getDefaultTool());

  }

  /**
   * another graphic context  wants to be a client of this toolbar */
  public void addClient(GraphicContext gc)
  {
    itsClients.put(gc.getFrame(), gc);
    gc.getFrame().addWindowListener(this);  
  }

  /**
   * remove a client graphic context */
  public void removeClient(GraphicContext gc)
  {
    itsClients.remove(gc.getFrame());
    gc.getFrame().removeWindowListener(this);  
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
   * inscribe a new listener to this toolbar
   */
  public void addToolListener(ToolListener tl) 
  {  
    listeners.addElement(tl);
  }

  /**
   * remove a listener
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
    aTool.reActivate(currentContext);

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

    GraphicContext gc = (GraphicContext) itsClients.get(e.getWindow());
    currentContext = gc;
    currentTool.reActivate(gc);
    //itsFrame.toFront();
  }

  public void windowDeactivated(WindowEvent e)
  {
  }

  //---- Fields 

  public JPopupMenu itsPopupMenu;

  Hashtable itsTools = new Hashtable();
  Tool currentTool = null;
  GraphicContext currentContext;

  JFrame itsFrame;
  Hashtable itsClients = new Hashtable();
  Vector listeners = new Vector();

  private ButtonGroup itsButtonGroup = new ButtonGroup();
  public static final int VERTICAL = 0;
  public static final int HORIZONTAL = 1;
}











