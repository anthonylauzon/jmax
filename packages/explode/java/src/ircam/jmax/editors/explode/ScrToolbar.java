package ircam.jmax.editors.explode;

import com.sun.java.swing.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;

/**
 * a generic class implementing a toolbar. It can have a set of listeners
 * to be called when the user selects a new tool
 */
public class ScrToolbar extends JToolBar implements ActionListener, WindowListener{
  
  /**
   * constructor. It inserts the tools provided by the given
   * ToolbarProvider
   */
  public ScrToolbar(ToolbarProvider theProvider) 
  {
    super();
    setDoubleBuffered(false);
    
    //install the Tools
    ScrTool aTool;
    for (Enumeration e = theProvider.getTools(); e.hasMoreElements();) 
      {
	aTool = (ScrTool) e.nextElement();
	if (aTool == null) System.err.println("warning: trying to add a null tool in the toolbar");
	else addTool(aTool);
      }

    currentContext = theProvider.getGraphicContext();
    setTool(theProvider.getDefaultTool());

  }


  private static void setTool(ScrTool t)
  {
    currentTool = t;
  }
  
  public static ScrTool getTool()
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
  private void toolNotification(ScrTool theTool) 
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
   * add a new tool in the toolbar
   */ 
  public void addTool(ScrTool theTool) 
  {
    JButton aButton = new JButton(theTool.getIcon());
    
    aButton.setToolTipText(theTool.getName());

    add(aButton);
    validate();
    
    itsTools.put(aButton, theTool);

    aButton.addActionListener(this);  
  }  


  /**
   * From the ActionListener interface,
   * called by a button when the user clicks on it.
   * The new tool will work on the same graphic context then the old.
   */
  public void actionPerformed(ActionEvent e) 
  {    
    JButton aButton = (JButton) e.getSource();
    ScrTool aTool = (ScrTool) itsTools.get(aButton);
    
    currentTool.deactivate();
    aTool.reActivate(currentContext);

    setTool(aTool);
    
    toolNotification(aTool);
  }

  /**
   * creates a toolbar, if it doesnt exist already.
   * Makes the (newly) created toolbar a listener for the
   * Graphic context's window 
   */
  static public void createToolbar(ToolbarProvider theProvider, GraphicContext gc) {
    
    currentContext = theProvider.getGraphicContext();

    if (itsToolbar == null) 
      {
	itsToolbar = new ScrToolbar(theProvider);
	itsFrame = new JFrame("tools");    
	itsFrame.getContentPane().add(itsToolbar);
	
	itsFrame.pack();
	itsFrame.setVisible(true);

	setTool(theProvider.getDefaultTool());
      }

    // maps a given window with its graphic context
    itsClients.put(gc.getFrame(), gc);
    gc.getFrame().addWindowListener(itsToolbar);  

  }
  
  static public ScrToolbar getToolbar() 
  {
    return itsToolbar;
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
    itsFrame.toFront();
  }

  public void windowDeactivated(WindowEvent e)
  {
  }

  //---- Fields 
  static ScrToolbar itsToolbar; 
  Hashtable itsTools = new Hashtable();
  static ScrTool currentTool = null;
  static GraphicContext currentContext;

  static JFrame itsFrame;
  static Hashtable itsClients = new Hashtable();
  Vector listeners = new Vector();

}



