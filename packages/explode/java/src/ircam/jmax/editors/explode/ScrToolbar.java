package ircam.jmax.editors.explode;

import com.sun.java.swing.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;

/**
 * a generic class implementing a toolbar. It can have a set of listeners
 * to be called when the user selects a new tool
 */
public class ScrToolbar extends JToolBar implements ActionListener{
  
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
	addTool(aTool);
      }
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
    
    add(aButton);
    validate();
    
    itsTools.put(aButton, theTool);

    aButton.addActionListener(this);  
  }  


  /**
   * From the ActionListener interface,
   * called by a button when the user clicks on it.
   */
  public void actionPerformed(ActionEvent e) 
  {    
    JButton aButton = (JButton) e.getSource();
    ScrTool aTool = (ScrTool) itsTools.get(aButton);
    toolNotification(aTool);
  }

  //---- Fields 
  Hashtable itsTools = new Hashtable();
  Vector listeners = new Vector();

}



