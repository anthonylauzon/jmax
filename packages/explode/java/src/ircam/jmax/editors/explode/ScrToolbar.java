package ircam.jmax.editors.explode;

import com.sun.java.swing.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;

/**
 * a generic class implementing a toolbar. It can have a list of listeners
 * that will be called when the user selects another tool in the bar
 */
public class ScrToolbar extends JToolBar implements ActionListener{
  
  Hashtable itsTools = new Hashtable();
  Vector listeners = new Vector();

  /**
   * inscribe a new listener to this toolbar
   */
  public void addToolListener(ToolListener tl) {
    listeners.addElement(tl);
  }

  /**
   * remove a listener
   */
  public void removeToolListener(ToolListener tl) {
    
    listeners.removeElement(tl);
  
  }
  


  /**
   * utility function. Communicate the tool-change to the listeners
   */
  private void toolNotification(ScrTool theTool) {
    
    ToolChangeEvent aEvent = new ToolChangeEvent(theTool);
    
    ToolListener aListener;
    for (Enumeration e=listeners.elements(); e.hasMoreElements();) {
      aListener = (ToolListener) e.nextElement();
      aListener.toolChanged(aEvent);
    }

  }
 

  /**
   * constructor
   */
  public ScrToolbar(ToolbarProvider theProvider) {

    super();
    installTools(theProvider);

  }


  /**
   * installs the tools provided by the toolbar provider
   */
  private void installTools(ToolbarProvider theProvider) {
    
    ScrTool aTool;
    for (Enumeration e = theProvider.getTools(); e.hasMoreElements();) {
      aTool = (ScrTool) e.nextElement();
      addTool(aTool);
    }
  }

  /**
   * add a new tool in the toolbar, given an icon and a ScrTool
   */ 
  public void addTool(ScrTool theTool) {
    JButton aButton = new JButton(theTool.getIcon());
    
    add(aButton);
    validate();
    
    itsTools.put(aButton, theTool);

    aButton.addActionListener(this);
    
  }  



  /**
   * called by a button in the toolbar when the user clicks on it.
   */
  public void actionPerformed(ActionEvent e) {
    
    JButton aButton = (JButton) e.getSource();
    ScrTool aTool = (ScrTool) itsTools.get(aButton);
    toolNotification(aTool);
  
  }

}



